
#ifndef FUZZYCONTROLLER_H
#define FUZZYCONTROLLER_H

#include <iostream>

class FuzzyController {
public:
    FuzzyController();
    void showSignsOfLife();
    double decodeLidar(double *lidarData, int returncode);
    double controller(double dir,double *lidarData, int steeringorspeed );
    ~FuzzyController();
protected:

};


#endif //FUZZYCONTROLLER_H
