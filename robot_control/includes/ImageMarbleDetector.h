#pragma once

#ifndef IMAGEMARBLEDETECTOR_H
#define IMAGEMARBLEDETECTOR_H

#include <iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;


struct MarbleParameters { int* coms; int amount; bool noBall = false; };
struct Marbles { int xcom = 0; int significance = 0; };
struct Circles { Point center; int rad =-1; };


constexpr double IMAGEXANGLE = 0.5448;

class ImageMarbleDetector {
public:
	ImageMarbleDetector();
	void showSignsOfLife();
	double optimizedCIM(Mat* img, bool draw=false);
	~ImageMarbleDetector();


private:
	int whitepixels = 0;
	int floorcolor;
	int pixelcolor; int kernelcolor;
	int xpos; int ypos;
	int adjecentblacks = 0;
	int lastx; int lasty;
	int coms[3];


	//Rotating mask filter
	double* calculateAverages(int listOfRGBs[9][3]);
	double calculateAverageVariance(int listOfRGBs[9][3]);
	int indexOfSmallestInList(double list[], int size);
	void rotateAroundPixelAtPosition(Mat* image, int row, int col);
	Mat rotatingMaskFilter(Mat* img);


	//Preprocessing
	void removeFloors(Mat* img);
	void removeOutliers(Mat* img, int minblacks);
	void removeInsignificant(Mat* img);
	void removeShades(Mat* img);

	//Helper functions	
	void colorPixel(Mat* img, int x, int y, int color);
	int imAt(Mat* img, int x, int y);
	bool isPixelInImage(Mat* image, int row, int col);


	//Marblefinder
	void shineMarker(Mat* img);
	Marbles *segmentsCOM(int* xvals, int points, int segments, int width);
	Circles* defineCircles(Mat* img, Marbles *marbles, int size);
	Circles* findParameters(Mat* img);
};


#endif //IMAGEMARBLEDETECTOR_H