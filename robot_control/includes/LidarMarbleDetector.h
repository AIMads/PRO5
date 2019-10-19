//
// Created by Sina P. Soltani on 19/10/2019.
//

#ifndef PLAYGROUND_LIDARMARBLEDETECTOR_H
#define PLAYGROUND_LIDARMARBLEDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

#define MAX_LIDAR_RANGE 10
#define SLOPE_DIFF 0.75
#define MAX_RADIUS 70
#define THRESHOLD 60
#define ROTATION_OFFSET 1
#define FOV (2.27 - (-2.27))
#define NUM_DATAPTS 200
#define ANGULAR_PREC FOV / NUM_DATAPTS

using namespace std;
using namespace cv;

using namespace std;
using namespace cv;

struct LidarSegments{int numSegments; int * numPtsInSegment; Point ** segments;};

class LidarMarbleDetector {
public:
    LidarMarbleDetector();
    LidarMarbleDetector(double * data, int size, int width, int height);

    void drawCircle(Mat * img, Point center, double r);

    bool isInRange(double range);
    double determinant(Point a, Point b);
    double solve2LinEq(double a1, double b1, double a2, double b2);
    double calculateDistance(double x1, double y1, double x2, double y2);
    double getBiggestDiff(double * arr, int size);

    void perpendicularBisector(Point a, Point b, double *slopePtr, double *interceptPtr);
    auto calculateCenterAndRadiusOfCircle(Point a, Point b, Point c);

    bool checkForCircles(int numPts, Point* points);
    void checkSegments(LidarSegments lidarSegments, Mat * image);

    LidarSegments getLidarSegments(double * data, int numDataPoints);
    Mat plotLidarData();

    void onSetData();
    void setLidarData(double * data);

    ~LidarMarbleDetector();

protected:
    double * _lidarData;
    int _size;
    int _imageWidth;
    int _imageHeight;
};


#endif //PLAYGROUND_LIDARMARBLEDETECTOR_H
