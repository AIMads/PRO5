#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>

#include <opencv2/opencv.hpp>

#include <iostream>
#include "includes/FuzzyController.h"
#include "includes/LidarMarbleDetector.h"
#include "includes/ImageMarbleDetector.h"
#include "includes/ParticleFilter.h"

#define NUM_DATA_POINTS 200
#define NUM_REDUNDANT_POINTS 5
#define IMG_SCL_FCTR (double) 10/1.41735

double * lidarData = new double[NUM_DATA_POINTS];
float * reducedLidarData = new float[NUM_LIDAR_DATA_PTS];
Mat image;
float robDX = 0;
float robDY = 0;

void statCallback(ConstWorldStatisticsPtr &_msg) {
    (void)_msg;
    // Dump the message contents to stdout.
    //  std::cout << _msg->DebugString();
    //  std::cout << std::flush;
}

void poseCallback(ConstPosesStampedPtr &_msg) {
    // Dump the message contents to stdout.
    //  std::cout << _msg->DebugString();
    
    for (int i = 0; i < _msg->pose_size(); i++) {
        if (_msg->pose(i).name() == "pioneer2dx") {
            
            robDX = _msg->pose(i).position().x();
            robDY = _msg->pose(i).position().y();
            /*
            std::cout << std::setprecision(2) << std::fixed << std::setw(6)
            << _msg->pose(i).position().x() << std::setw(6)
            << _msg->pose(i).position().y() << std::setw(6)
            << _msg->pose(i).position().z() << std::setw(6)
            << _msg->pose(i).orientation().w() << std::setw(6)
            << _msg->pose(i).orientation().x() << std::setw(6)
            << _msg->pose(i).orientation().y() << std::setw(6)
            << _msg->pose(i).orientation().z() << std::endl;*/
        }
    }
}

void cameraCallback(ConstImageStampedPtr &msg) {
    std::size_t width = msg->image().width();
    std::size_t height = msg->image().height();
    const char *data = msg->image().data().c_str();
    cv::Mat im(int(height), int(width), CV_8UC3, const_cast<char *>(data));

    image = im.clone();

    cv::cvtColor(im, im, cv::COLOR_RGB2BGR);

    /*
    mutex.lock();
    cv::imshow("camera", im);
    mutex.unlock();
    */
}

void lidarCallback(ConstLaserScanStampedPtr &msg) {
     //  std::cout << ">> " << msg->DebugString() << std::endl;
     float angle_min = float(msg->scan().angle_min());
     //  double angle_max = msg->scan().angle_max();
     float angle_increment = float(msg->scan().angle_step());

     float range_min = float(msg->scan().range_min());
     float range_max = float(msg->scan().range_max());

     int sec = msg->time().sec();
     int nsec = msg->time().nsec();

     int nranges = msg->scan().ranges_size();
     int nintensities = msg->scan().intensities_size();

     assert(nranges == nintensities);

     int width = 400;
     int height = 400;
     float px_per_m = 200 / range_max;

     cv::Mat im(height, width, CV_8UC3);
     im.setTo(0);
     for (int i = 0; i < nranges; i++) {
        float angle = angle_min + i * angle_increment;
        float range = std::min(float(msg->scan().ranges(i)), range_max);
        //    double intensity = msg->scan().intensities(i);
        cv::Point2f startpt(200.5f + range_min * px_per_m * std::cos(angle),
        200.5f - range_min * px_per_m * std::sin(angle));
        cv::Point2f endpt(200.5f + range * px_per_m * std::cos(angle),
        200.5f - range * px_per_m * std::sin(angle));
        cv::line(im, startpt * 16, endpt * 16, cv::Scalar(255, 255, 255, 255), 1,
        cv::LINE_AA, 4);
        lidarData[i] = range;
     }
     cv::circle(im, cv::Point(200, 200), 2, cv::Scalar(0, 0, 255));
     cv::putText(im, std::to_string(sec) + ":" + std::to_string(nsec),
     cv::Point(10, 20), cv::FONT_HERSHEY_PLAIN, 1.0,
     cv::Scalar(255, 0, 0));
     /*
     mutex.lock();
     cv::imshow("lidar", im);
     mutex.unlock();
     */
}

int main(int _argc, char **_argv) {
    // Load gazebo
    gazebo::client::setup(_argc, _argv);
    
    // Create our node for communication
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    // Listen to Gazebo topics
    gazebo::transport::SubscriberPtr statSubscriber =
    node->Subscribe("~/world_stats", statCallback);

    gazebo::transport::SubscriberPtr poseSubscriber =
    node->Subscribe("~/pose/info", poseCallback);

    gazebo::transport::SubscriberPtr cameraSubscriber =
    node->Subscribe("~/pioneer2dx/camera/link/camera/image", cameraCallback);

    gazebo::transport::SubscriberPtr lidarSubscriber =
    node->Subscribe("~/pioneer2dx/hokuyo/link/laser/scan", lidarCallback);

    // Publish to the robot vel_cmd topic
    gazebo::transport::PublisherPtr movementPublisher =
    node->Advertise<gazebo::msgs::Pose>("~/pioneer2dx/vel_cmd");

    // Publish a reset of the world
    gazebo::transport::PublisherPtr worldPublisher =
    node->Advertise<gazebo::msgs::WorldControl>("~/world_control");
    gazebo::msgs::WorldControl controlMessage;
    controlMessage.mutable_reset()->set_all(true);
    worldPublisher->WaitForConnection();
    worldPublisher->Publish(controlMessage);

    Mat floorPlan = imread("../models/bigworld/meshes/floor_plan.png");
    resize(floorPlan,floorPlan,Size(847,564),IMG_SCL_FCTR,IMG_SCL_FCTR,INTER_NEAREST);
    
    float speed = 0.0;
    float dir = 0.0;
    float targetDir = NO_TARGET;

    bool imageConfirmed = false;

    // Object init
    FuzzyController fc;
    LidarMarbleDetector lmd = LidarMarbleDetector(lidarData,NUM_DATA_POINTS,400,400);
    ImageMarbleDetector imd;
    //ParticleFilter pf = ParticleFilter(&floorPlan);

    // Loop
    while (true) {
        gazebo::common::Time::MSleep(100);
        
        std::tie(dir,speed) = fc.controller(dir,lidarData);

        //std::cout << "Direction: " << dir << std::endl;


        // Change speed and direction based on fuzzycontrol
        int indexOfRays = 0;
        for (int i = 0; i < NUM_DATA_POINTS - NUM_REDUNDANT_POINTS; i += 5) {
            reducedLidarData[indexOfRays++] = (float) lidarData[i];
        }
        

        targetDir = lmd.setLidarData(lidarData);
        //pf.setData(speed, dir, reducedLidarData);
        //pf.updateRobotPos(robDX,robDY);
        
        
        std::cout << "TargetDir: " << targetDir << std::endl;
        if(targetDir != 3)
        {
            dir = -targetDir;
        }
        if(dir == -targetDir && !imageConfirmed){
            float imagedir = imd.optimizedCIM(&image,false);
            imageConfirmed = false;
            if(std::abs(imagedir) < 12 * 2.27 / 100){
                dir = -targetDir;
                imageConfirmed = true;
                std::cout << "Image confirmed!" << std::endl;
            }
        }
        
        // Generate a pose
        ignition::math::Pose3d pose(double(speed), 0, 0, 0, 0, double(dir));

        // Convert to a pose message
        gazebo::msgs::Pose msg;
        gazebo::msgs::Set(&msg, pose);
        movementPublisher->Publish(msg);
    }

    // Make sure to shut everything down.
    gazebo::client::shutdown();
}
