#pragma once
#ifndef ROUTE_H
#define ROUTE_H

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <stdio.h> 
#include "Agent.h"
#include "Environment.h"

using namespace cv;

struct Checkpoints { Point* checkpoints; int numCheckpoints; };
struct Path { int cp_id[20] = { 0 }; int length = 0; };

class Route {
public:
	Route();
	int makeRoute(bool useQ = false);


	~Route();

private:
	//Dijkstra
	int minDistance(int* dist, bool* sptSet, int size);
	int saveRoute(int* parent, Path* paths, int j, int routenr, int checkpointindex = -1);
	void saveRoutes(int* dist, int n, int* parent, Path* paths, int sourcecheckpointindex);
	Path* dijkstra(int** graph, int src, Checkpoints checkpoints);


	//Checkpoint creation
	Checkpoints generateCheckpoints(Mat* img, Mat* cpimg);
	void brushfire(Mat *img);
	void markCenterOfRooms(Mat* img);
	void removeUnnecessaryPoints(Mat* img);
	void mergePoints(Mat* img, int kernelsize = 5);
	void removeImpossibles(Mat* img);

	int grey = 100;
	int numCheckpoints = 0;


	//Graphing of routes/paths
	int** makeGraph(Mat* img, Point* checkpoints, int numCheckpoints);
	int checkpointDist(Mat* img, Point p1, Point p2);
	float eucDist(Point p1, Point p2);


	//Helpers
	bool isVisited(int* visited, int visited_index, int routeindex);
	void drawStep(Mat* img, Point p1, Point p2, Scalar color, bool draw = false);
	Scalar newColor(int call);


	//Routeplanning Algorithm
	void routePlanner(Checkpoints checkpoints, Mat* img, int** graph, bool draw = false);
};



#endif // !ROUTE_H
