#include "Route.h"

using namespace std;



Route::Route() {}


int Route::minDistance(int* dist, bool* sptSet, int size)
{

	int min = INT_MAX, min_index;

	for (int v = 0; v < size; v++)
		if (sptSet[v] == false &&
			dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}


int Route::saveRoute(int* parent, Path* paths, int j, int routenr, int checkpointindex)
{
	if (parent[j] == -1)
	{
		return checkpointindex;
	}
	checkpointindex++;				//Aware, ++ comes before operations!!
	int routelength = saveRoute(parent, paths, parent[j], routenr, checkpointindex);
	paths[routenr].cp_id[checkpointindex] = j;
	paths[routenr].length++;
	return routelength;
}

void Route::saveRoutes(int* dist, int n, int* parent, Path* paths, int sourcecheckpointindex)
{
	//Routenr is the same as route_goal
	for (int routenr = 0; routenr < n; routenr++)
	{
		//cout << "Route nr: " << routenr << " from source index: " << sourcecheckpointindex << endl;
		//paths[routenr].cp_id[0] = sourcecheckpointindex;
		//cout << paths[routenr].cp_id[0] << " ";
		int routelength = saveRoute(parent, paths, routenr, routenr);

		paths[routenr].cp_id[paths[routenr].length] = sourcecheckpointindex;
		paths[routenr].length++;

		//cout << paths[routenr].cp_id[routelength];

		//cout << endl << endl;
	}
}


Path* Route::dijkstra(int** graph, int src, Checkpoints checkpoints)
{
	int size = checkpoints.numCheckpoints;
	cout << "Calculating dijkstra from source: " << src << "  With size: " << size << endl;
	// The output array. dist[i] will hold the shortest distance from src to i 
	int* dist = new int[size];

	// sptSet[i] will true if vertex 
	// i is included / in shortest path tree or shortest distance from src to i is finalized 
	bool* sptSet = new bool[size];

	// Parent array to store shortest path tree 
	int* parent = new int[size];

	// Initialize all distances as INFINITE and stpSet[] as false 
	for (int i = 0; i < size; i++)
	{
		//Set the parent at source position to -1 to mark the end of a route.
		parent[src] = -1;
		dist[i] = INT_MAX;
		sptSet[i] = false;
	}

	// Distance of source vertex from itself is always 0 
	dist[src] = 0;

	// Find shortest path for all vertices 
	for (int count = 0; count < size - 1; count++)
	{
		// Pick the minimum distance vertex from the set of vertices not yet processed. 
		// u is always equal to src in first iteration. 
		int u = minDistance(dist, sptSet, size);

		// Mark the picked vertex as processed 
		sptSet[u] = true;

		// Update dist value of the adjacent vertices of the picked vertex. 
		for (int v = 0; v < size; v++)
		{
			// Update dist[v] only if is 
			// not in sptSet, there is  an edge from u to v, and total weight of path from 
			// src to v through u is smaller than current value of  dist[v] 
			if (!sptSet[v] && graph[u][v] && dist[u] + graph[u][v] < dist[v])
			{
				parent[v] = u;
				dist[v] = dist[u] + graph[u][v];
			}
		}
	}

	Path* paths = new Path[checkpoints.numCheckpoints];

	saveRoutes(dist, size, parent, paths, src);


	delete[] dist;
	delete[] parent;
	delete[] sptSet;
	return paths;
}



void Route::brushfire(Mat *img) {
	int newcolor = 0;
	int edgecolor = 0;
	for (int i = 0; i < 20; i++)
	{
		edgecolor = newcolor;
		newcolor += 10;
		if (newcolor > 200)
			newcolor = 200;
		for (int y = 0; y < img->rows; y++)
		{
			for (int x = 0; x < img->cols; x++)
			{
				//Check if pixel is on current edge
				if (img->at<uchar>(y, x) == edgecolor)
				{
					for (int yoff = -1; yoff < 2; yoff++)
					{
						for (int xoff = -1; xoff < 2; xoff++)
						{
							int xpos = x + xoff;
							int ypos = y + yoff;
							//Check if legal kernel
							if (xpos > 0 && xpos < img->cols && ypos > 0 && ypos < img->rows)
							{
								if (img->at<uchar>(ypos, xpos) > edgecolor)
								{
									img->at<uchar>(ypos, xpos) = newcolor;
								}
							}
						}
					}
				}

			}
		}
	}
}


void Route::markCenterOfRooms(Mat* img) {
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			//Each pixel
			int pixelcolor = img->at<uchar>(y, x);
			int horizontallowers = 0;
			int verticallowers = 0;

			//Check if between wall above and below
			for (int yoff = -2; yoff < 3; yoff++)
			{
				int ypos = y + yoff;
				if (ypos > 0 && ypos < img->rows && yoff != 0)
				{
					if (img->at<uchar>(ypos, x) < pixelcolor)
						verticallowers++;
				}
			}

			//Check if between wall left and right
			for (int xoff = -2; xoff < 3; xoff++)
			{
				int xpos = x + xoff;
				if (xpos > 0 && xpos < img->cols && xoff != 0)
				{
					if (img->at<uchar>(y, xpos) < pixelcolor)
						horizontallowers++;
				}
			}
			if (horizontallowers > 2 || verticallowers > 2)
				img->at<uchar>(y, x) = 255;
		}
	}
}


void Route::removeUnnecessaryPoints(Mat* img) {
	Mat superimg = img->clone();
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			//Each pixel
			int pixelcolor = img->at<uchar>(y, x);
			int horizontalwhites = 0;
			int verticalwhites = 0;

			if (pixelcolor == 255)
			{
				for (int yoff = -1; yoff < 2; yoff++)
				{
					int ypos = y + yoff;
					if (ypos > 0 && ypos < img->rows && yoff != 0)
					{
						if (img->at<uchar>(ypos, x) == 255)
							verticalwhites++;
					}
				}
				if (verticalwhites > 1)
					superimg.at<uchar>(y, x) = 100; // img->at<uchar>(y, x+1);

				//Check if between wall left and right
				for (int xoff = -1; xoff < 2; xoff++)
				{
					int xpos = x + xoff;
					if (xpos > 0 && xpos < img->cols && xoff != 0)
					{
						if (img->at<uchar>(y, xpos) == 255)
							horizontalwhites++;
					}
				}
				if (horizontalwhites > 1)
					superimg.at<uchar>(y, x) = 100; // img->at<uchar>(y + 1, x);
			}
		}
	}
	*img = superimg.clone();
	superimg.release();
}


void Route::mergePoints(Mat* img, int kernelsize) {
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			if (img->at<uchar>(y, x) == 255)
			{
				//New white pixel
				int meanx = x;
				int meany = y;
				int whites = 1;
				//only set pixels to 254

				for (int yoff = -kernelsize; yoff < kernelsize + 1; yoff++)
				{
					for (int xoff = -kernelsize; xoff < kernelsize + 1; xoff++)
					{
						int xpos = x + xoff;
						int ypos = y + yoff;
						//Check if legal kernel
						if (xpos > 0 && xpos < img->cols && ypos > 0 && ypos < img->rows)
						{
							if (img->at<uchar>(ypos, xpos) == 255)
							{
								meanx += xpos;
								meany += ypos;
								whites++;
								img->at<uchar>(ypos, xpos) = grey;
							}
						}
					}
				}
				//If in cluster
				if (whites > 1)
				{
					meanx = meanx / whites;
					meany = meany / whites;
					img->at<uchar>(y, x) = grey;	//removes first whites pixel
					img->at<uchar>(meany, meanx) = 254;			//Not 255 so pixels can't traverse!!
				}
			}
		}
	}
}

void Route::removeImpossibles(Mat* img) {
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			if (img->at<uchar>(y, x) > 250)
			{
				bool closetowall = false;

				//Check kernel for adjecent wall
				for (int yoff = -1; yoff < 2 + 1; yoff++)
				{
					for (int xoff = -1; xoff < 2 + 1; xoff++)
					{
						int xpos = x + xoff;
						int ypos = y + yoff;
						//Check if legal kernel
						if (xpos > 0 && xpos < img->cols && ypos > 0 && ypos < img->rows)
						{

							if (img->at<uchar>(ypos, xpos) == 0)
							{
								img->at<uchar>(y, x) = grey;
								closetowall = true;
							}
						}
					}
				}
				if (not closetowall)
				{
					numCheckpoints++;
				}
			}
		}
	}
}
Checkpoints Route::generateCheckpoints(Mat* img, Mat* cleanimg)
{
	//img should be imported as greyscale, thus no need to change any colors.
	*cleanimg = img->clone();

	//Draw rooms gradually brighter further from walls
	brushfire(img);

	//Draw all centers white
	markCenterOfRooms(img);

	//Remove unnessecary points horizontally or vertically between points
	removeUnnecessaryPoints(img);

	//Finally merge close points
	mergePoints(img);

	//Remove the "impossible" ones right next to a wall
	removeImpossibles(img);


	int checkpointIndex = 0;
	numCheckpoints += 1;//+1 for starting point
	Point* checkpoints = new Point[numCheckpoints];	
	//Add starting point
	checkpoints[checkpointIndex++] = Point(img->cols / 2, img->rows / 2);


	//PPS turn back to the clean image
	cv::cvtColor(*cleanimg, *cleanimg, COLOR_GRAY2BGR);
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			if (img->at<uchar>(y, x) > 250)
			{
				checkpoints[checkpointIndex++] = Point(x, y);
				cleanimg->at<Vec3b>(y, x) = (0, 100, 255);
			}
		}
	}



	//namedWindow("brushfire", WINDOW_NORMAL);
	//cv::imshow("brushfire", *cleanimg);
	//cv::waitKey();
	img->release();

	return Checkpoints{ checkpoints, numCheckpoints };
}

float Route::eucDist(Point p1, Point p2) {
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

int Route::checkpointDist(Mat* img, Point p1, Point p2)
{
	int dist;
	Vec3b black{ 0,0,0 };
	if (p1 != p2) {
		//Check if line is legal.
		bool wallInDaWay = false;
		LineIterator lineIterator = LineIterator(*img, p1, p2, 8);
		for (int k = 0; k < lineIterator.count; ++k, ++lineIterator) {
			if (img->at<Vec3b>(lineIterator.pos()) == black) {
				wallInDaWay = true;
				break;
			}
		}

		if (!wallInDaWay) {
			dist = eucDist(p1, p2);		//Float to dist. Don't care that much about . precision
		}
		else
			dist = 0;		//0 is later used to signify illegal move.
	}
	else
		dist = 0; //Distance to oneself is always 0!

	return dist;
}

int** Route::makeGraph(Mat* img, Point* checkpoints, int numCheckpoints)
{
	//Create an empty sqaure table
	int** dist = new int* [numCheckpoints];
	for (int i = 0; i < numCheckpoints; i++)
	{
		dist[i] = new int[numCheckpoints];
	}

	//Fill distance-graph
	for (int i = 0; i < numCheckpoints; ++i) {
		for (int j = 0; j < numCheckpoints; ++j) {
			dist[i][j] = checkpointDist(img, checkpoints[i], checkpoints[j]);
			//cout << dist[i][j] << " ";
		}
		//cout << endl;
	}


	return dist;
}


bool Route::isVisited(int* visited, int visited_index, int routeindex)
{
	for (int i = 0; i < visited_index; i++)
	{
		if (visited[i] == routeindex)
			return true;
	}

	return false;
}
void Route::drawStep(Mat* img, Point p1, Point p2, Scalar color, bool draw)
{

	line(*img, p1, p2, color, 1);

	imshow("image", *img);
	waitKey(0);


}
Scalar Route::newColor(int call)
{
	Scalar green{ 0, 250, 0 };
	Scalar red{ 0,0,250 };

	if (call % 2 == 0)
		return green;
	return red;
}


void Route::routePlanner( Checkpoints checkpoints, Mat* img, int** distances, bool draw)
{
	Path* pathsFromOrigin = dijkstra(distances, 0, checkpoints);
	int distancetravelled = 0;
	int calls = 0;
	int size = checkpoints.numCheckpoints;
	int* visited = new int[size];
	int visited_index = 0;
	Path* routes_from_position = pathsFromOrigin;
	int currentpos_index = 0;
	cv::namedWindow("image", WINDOW_GUI_NORMAL);
	int bestScore = INT_MIN;
	int bestScoreIndex= 0;
	int moves = 0;

	while (visited_index < checkpoints.numCheckpoints)
	{
		bestScore = INT_MIN;
		if (visited_index != 0) //Doesn't run untill point 1.
		{
			currentpos_index = bestScoreIndex;
			routes_from_position = dijkstra(distances, currentpos_index, checkpoints);
		}

		//Find unvisited checkpoint furthest from origo.
		for (int i = 0; i < checkpoints.numCheckpoints; i++)
		{
			int score = pathsFromOrigin[i].length - routes_from_position[i].length;
			if (score >= bestScore)
			{
				if (!isVisited(visited, visited_index, i) && i != currentpos_index)
				{
					bestScore = score;
					bestScoreIndex = i;
				}

			}
		}

		Scalar color = newColor(calls++);  
		for (int i = routes_from_position[bestScoreIndex].length - 1; i > 0; i--)
		{
			Point pos = checkpoints.checkpoints[routes_from_position[bestScoreIndex].cp_id[i]];
			Point pos_ = checkpoints.checkpoints[routes_from_position[bestScoreIndex].cp_id[i - 1]];
			int nextindex = routes_from_position[bestScoreIndex].cp_id[i - 1];

			if (!isVisited(visited, visited_index, nextindex)) {
				visited[visited_index++] = nextindex;			///WHATTTTTT????????????????????????????????????????????
			}

			if (draw)
				drawStep(img, pos, pos_, color, true);

			moves++;
			distancetravelled += eucDist(pos, pos_);
		}
	}


	cout << endl << "Traversed all points by only " << moves << " moves" << endl 
		<< "Distance travelled: " << distancetravelled << endl;
}



int Route::makeRoute(bool useQ) {


	namedWindow("image", WINDOW_GUI_NORMAL);
	Mat img = imread("./floorplan.png", IMREAD_GRAYSCALE);
	namedWindow("image", WINDOW_GUI_NORMAL);
	imshow("image", img);
	waitKey();

	Mat result;
	Checkpoints checkpoints = generateCheckpoints(&img, &result);
	//Brushfire releases original image.

	//Generate graph
	int** distances = makeGraph(&result, checkpoints.checkpoints, checkpoints.numCheckpoints);

	


	if (!useQ) {
		routePlanner( checkpoints, &result, distances, false);

	}
	else {
		Environment env;
		env.distances = makeGraph(&result, checkpoints.checkpoints, checkpoints.numCheckpoints);
		env.initEnv(checkpoints.numCheckpoints);


		Agent agent(&env);
		int* route = agent.run();

		for (int i = 0; i < 60 - 1; i++) {
			drawStep(&result, checkpoints.checkpoints[route[i]], checkpoints.checkpoints[route[i + 1]], false);
		}
	}


	

	return 0;

}



Route::~Route() {}