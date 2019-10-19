
#ifndef FUZZYCONTROLLER_H
#define FUZZYCONTROLLER_H
#include <tuple>
#include <iostream>

class FuzzyController {
public:

    FuzzyController();
    void showSignsOfLife();
    double decodeLidar(double *lidarData, int returncode);
    std::tuple<double,double> controller(double dir,double *lidarData);
    ~FuzzyController();
protected:

};


#endif //FUZZYCONTROLLER_H
