
#include "fl/Headers.h"
#include "FuzzyController.h"

using namespace fl;

FuzzyController::FuzzyController() {}

void FuzzyController::showSignsOfLife() {
    std::cout << "Guys!! I AM ALIVE! YES!" << std::endl;
}

double FuzzyController::decodeLidar(double *lidarData, int returncode)
{
    double smallestValue = 10.0;
    double oldestSmallestValue = 10.0;
    double index;
    double angle = 0;
    int length = 200;
    double closestValue[1] = {0};
    for (int i = 0; i < length; i++)
    {
        oldestSmallestValue = lidarData[i];
        if (oldestSmallestValue < smallestValue)
        {
            smallestValue = oldestSmallestValue;
            index = i;
        }
    }
    angle = index * 0.0227 -2.27;
    closestValue[0] = smallestValue; 
    closestValue[1] = angle;
    if (returncode == 0)
    {
        return closestValue[0];
    }
    else
    {
        return closestValue[1];
    }
}

std::tuple<double,double> FuzzyController::controller(double dir, double *lidarData)
{
    Engine* engine = FllImporter().fromFile("includes/FuzzyController.fll");
    scalar obstloc;
    std::string status;
    if (not engine->isReady(&status))
        throw Exception("[engine error] engine is not ready:n" + status, FL_AT);
        
    InputVariable* obstacle = engine->getInputVariable("ClosestObstacleDir");
    InputVariable* distanceToObstacle = engine->getInputVariable("distanceToObstacleDist");
    //InputVariable* direction = engine->getInputVariable("direction");
    OutputVariable* steer = engine->getOutputVariable("mSteer");
    OutputVariable* speed = engine->getOutputVariable("mSpeed");
        
    double obst = decodeLidar(lidarData,1);
    //obstloc = obst;
    std::cout << "ClosestObstacle Angle: " << obst << std::endl;
    obstacle->setValue(obst);
    obst = decodeLidar(lidarData,0);
    std::cout << "ClosestObstacle Range: " << obst << std::endl;
    distanceToObstacle->setValue(obst);
    //direction->setValue(dir);
    engine->process();
    dir += steer->getValue();
    FL_LOG("obstacle.input = " << Op::str(obst) << 
        " => " << "steer.output = " << Op::str(steer->getValue()));
    
    return std::make_tuple(steer->getValue(),speed->getValue());//steerandspeed;      
}

//sudo g++ obstacleavoidance.cpp -I/home/mads/fuzzylite/fuzzylite -L/home/mads/fuzzylite/fuzzylite/release/bin -lfuzzylite-static -o oa

FuzzyController::~FuzzyController() {}
