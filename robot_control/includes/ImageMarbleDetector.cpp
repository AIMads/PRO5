#include "ImageMarbleDetector.h"



ImageMarbleDetector::ImageMarbleDetector() {}


void ImageMarbleDetector::showSignsOfLife() {
	std::cout << "Me three!" << std::endl;
}


bool ImageMarbleDetector::isPixelInImage(Mat* image, int row, int col) {
	return 0 <= row && row < image->rows && 0 <= col && col < image->cols;
}
double* ImageMarbleDetector::calculateAverages(int listOfRGBs[9][3]) {

	static double averages[3];

	for (int channel = 0; channel < 3; channel++) {
		int sum = 0;
		int divisor = 9;
		for (int i = 0; i < 9; i++) {
			sum += listOfRGBs[i][channel];
			if (listOfRGBs[i][0] == 0 && listOfRGBs[i][1] == 0 && listOfRGBs[i][2] == 0) {
				divisor--;
			}
		}
		double avg = 0;
		if (divisor != 0) {
			avg = (double)sum / divisor;
		}
		averages[channel] = avg;
	}
	return averages;
}
double ImageMarbleDetector::calculateAverageVariance(int listOfRGBs[9][3]) {

	double* means = calculateAverages(listOfRGBs);
	// Flatten liste af RGB'er s� listerne med 0'er ikke er der l�ngere
	// eller tjek om det er en liste med 0'er og s� t�l iteratoren ned? det tror jeg ogs� man kan g�re
	double variances[3];

	for (int i = 0; i < 3; ++i) {
		double sum = 0;
		int numValidPixels = 0;
		for (int j = 0; j < 9; ++j) {
			if (!(listOfRGBs[i][0] == 0 && listOfRGBs[i][1] == 0 && listOfRGBs[i][2] == 0)) {
				sum += (listOfRGBs[j][i] - means[i]) * (listOfRGBs[j][i] - means[i]);
				numValidPixels++;
			}
		}
		double variance = (numValidPixels > 1) ? sum / (numValidPixels - 1) : 100;
		variances[i] = variance;
	}

	double avgVariance = 0;
	for (int k = 0; k < 3; ++k) {
		avgVariance += variances[k];
	}
	avgVariance /= 3;

	return avgVariance;
}
int ImageMarbleDetector::indexOfSmallestInList(double list[], int size) {
	int index = 0;
	double smallestVal = list[index];
	for (int i = 0; i < size; ++i) {
		if (list[i] < smallestVal) {
			index = i;
			smallestVal = list[i];
		}
	}
	return index;
}
void ImageMarbleDetector::rotateAroundPixelAtPosition(Mat* image, int row, int col) {

	double maskVariances[9];
	double maskAverages[9][3] = { {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0} };

	int rgbForPixelsInMask[9][3] = { {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0} };

	for (int rowOffset = 0; rowOffset < 3; rowOffset++) {
		for (int colOffset = 0; colOffset < 3; colOffset++) {

			int index = 0;

			for (int yOffset = -2; yOffset <= 0; yOffset++) {
				for (int xOffset = -2; xOffset <= 0; xOffset++) {

					int pixelRow = row + yOffset + rowOffset;
					int pixelCol = col + xOffset + colOffset;

					//cout << "[" << pixelRow << ", " << pixelCol << "]";

					if (isPixelInImage(image, pixelRow, pixelCol)) {
						//cout << " Inside";
						// If the position of a point in a given mask is inside the image, the RGB values of the pixel at that point are saved
						for (int i = 0; i < 3; i++) {
							rgbForPixelsInMask[index][i] = (int)image->at<Vec3b>(pixelRow, pixelCol)[i];
						}

					}
					else {
						//cout << " Outside";
					}
					index++;
					//cout << endl;
				}
			}
			// After run-through of a single mask
			// Calculate dispersion for mask and save in array maskDispersions (furthermore calculate dispersion for all color channels and save the average of these dispersions
			double variance = calculateAverageVariance(rgbForPixelsInMask);
			maskVariances[3 * rowOffset + colOffset] = variance;
			// Calculate average of mask and save in array maskAverages
			double* averages = calculateAverages(rgbForPixelsInMask);
			for (int j = 0; j < 3; j++) {
				//cout << "avg " << *(averages + j) << endl;
				maskAverages[3 * rowOffset + colOffset][j] = *(averages + j);
			}
			//cout << endl;
		}
	}
	// After run-through of all masks
	// Find mask with lowest dispersion and return average of that mask
	//coutList(maskAverages);
	int indexOfMaskWithSmallestVariance = indexOfSmallestInList(maskVariances, 9);
	auto r = (uchar)maskAverages[indexOfMaskWithSmallestVariance][0];
	auto g = (uchar)maskAverages[indexOfMaskWithSmallestVariance][1];
	auto b = (uchar)maskAverages[indexOfMaskWithSmallestVariance][2];

	Vec3b filteredPixel = { b,g,r };
	//cout << "b " << image->at<Vec3b>(row,col) << endl;
	image->at<Vec3b>(row, col) = filteredPixel;
	//cout << "a " << image->at<Vec3b>(row,col) << endl;
	//cout << endl;
}
void ImageMarbleDetector::removeOutliers(Mat* img, int minblacks)
{
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = img->at<uchar>(y, x);
			if (pixelcolor != 0)	//No need to fill already black ones
			{
				for (int yoff = -1; yoff < 2; yoff++)
				{
					for (int xoff = -1; xoff < 2; xoff++)
					{
						xpos = x + xoff;
						ypos = y + yoff;
						if (xpos > 0 && ypos > 0 && xpos < img->cols && ypos < img->rows)
						{
							kernelcolor = pixelcolor = img->at<uchar>(ypos, xpos);
							if (kernelcolor == 0)
							{
								adjecentblacks++;
							}
						}
					}
				}

				if (adjecentblacks > minblacks)
				{
					img->at<uchar>(y, x) = 0;
				}
				adjecentblacks = 0;
			}
		}
	}
}

void ImageMarbleDetector::colorPixel(Mat* img, int x, int y, int color)
{
	img->at<uchar>(y, x) = color;
	return;
}

void ImageMarbleDetector::removeInsignificant(Mat* img) {
	int whiteneightbors = 0;
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = img->at<uchar>(y, x);
			if (pixelcolor == 255)	
			{
				for (int yoff = -1; yoff < 2; yoff++)
				{
					for (int xoff = -1; xoff < 2; xoff++)
					{
						xpos = x + xoff;
						ypos = y + yoff;
						if (xpos > 0 && ypos > 0 && xpos < img->cols && ypos < img->rows )
						{
							if (!(xpos == 0 && ypos == 0)) {
								kernelcolor = img->at<uchar>(ypos, xpos);
								if (kernelcolor == 255)
								{
									whiteneightbors++;
								}
							}
							
						}
					}
				}

				if (whiteneightbors < 5)
				{
					img->at<uchar>(y, x) = 0;
				}
				whiteneightbors = 0;
			}
		}
	}
}
int ImageMarbleDetector::imAt(Mat* img, int x, int y)
{
	return img->at<uchar>(y, x);
}

void ImageMarbleDetector::removeShades(Mat* img)
{
	for (int y = 0; y < img->rows; y++)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = imAt(img, x, y);
			if (pixelcolor > 0 && pixelcolor < 255)
				colorPixel(img, x, y, 0);
		}
	}
}

void ImageMarbleDetector::removeFloors(Mat* img)
{
	//cout << "Rows: " << img->rows << "   Cols: " << img->cols << endl;
	floorcolor = (img->at<uchar>(img->rows - 1, img->cols / 2) + img->at<uchar>(img->rows - 1, img->cols / 2 + 1) + img->at<uchar>(img->rows - 1, img->cols / 2 + 1)) / 3;
	int ballcolor = img->at<uchar>(img->rows / 2, img->cols / 2);
	//cout << "floorcolor: " << floorcolor << "   Ballcolor: " << ballcolor << endl;

	for (int y = img->rows - 1; y >= 0; y--)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = img->at<uchar>(y, x);
			if (floorcolor - pixelcolor < 20)
			{
				img->at<uchar>(y, x) = 0;
			}

		}
	}
}

Mat ImageMarbleDetector::rotatingMaskFilter(Mat* image) {
	Mat filteredImage = image->clone();

	for (int i = 0; i < filteredImage.rows; i++) {
		for (int j = 0; j < filteredImage.cols; j++) {
			rotateAroundPixelAtPosition(&filteredImage, i, j);
		}
	}

	return filteredImage;
}

void ImageMarbleDetector::shineMarker(Mat* img)
{
	for (int y = img->rows - 1; y > 0+1; y--)
	{
		for (int x = 0; x < img->cols; x++)
		{
			pixelcolor = img->at<uchar>(y, x);
			if (pixelcolor != 255 && pixelcolor != 0)	//No need to fill already black ones
			{
				int pixelabove = img->at<uchar>(y-1, x);
				if (pixelabove - pixelcolor > 10 && pixelabove != 255) {
					img->at<uchar>(y, x) = 255;
					img->at<uchar>(y-1, x) = 255;
				}
			}
		FINISHPIXEL:
			//
			int i;
		}
	}
}

Marbles* ImageMarbleDetector::segmentsCOM(int* xvals, int points, int segments, int width) {
	Marbles* marbles = new Marbles[segments];

	int segmentindex = 1;
	for (int i = 0; i < width; i++) {
		if (i > segmentindex * (width/segments)) {
		segmentindex++;
		}
		marbles[segmentindex-1].xcom += xvals[i] * i;
		marbles[segmentindex - 1].significance += xvals[i];
	}


	for (int i = 0; i < segments; i++) {
		if (marbles[i].significance != 0)
			marbles[i].xcom /= marbles[i].significance;
		else
			marbles[i].xcom = -1;
		//cout << "Segment" << i << "  xcom: " << marbles[i].xcom << "  Significance: " << marbles[i].significance << endl;
	}

	return marbles;
}

Circles* ImageMarbleDetector::findParameters(Mat* img) {
	int points = 0;
	int* buffer = new int[img->cols]{ 0 };	

	for (int y = 0; y < img->rows; y++) {
		for (int x = 0; x < img->cols; x++) {
			if (img->at<uchar>(y, x) == 255) {
				buffer[x] += 1;
				points++;
			}
		}
	}

	Marbles *marbles = segmentsCOM(buffer, points, 16, img->cols);

	Circles* circles = defineCircles(img, marbles, 16);
	delete[] buffer;
	delete[] marbles;
	return circles;
}


//Returns parameters of closest marble
Circles* ImageMarbleDetector::defineCircles(Mat* img, Marbles *marbles, int size) {
	Circles* circles = new Circles[size+1];		//So atleast 1 marble with the default deathmarked rad of -1
	int circlesindex = 0;


	int xcom_ = 0;
	int sig_ = 0;
	int dia_ = 0;
	
	for (int i = 0; i < size; i++) {
		if (marbles[i].significance < 3) {
			if (sig_ > 0) {
				int x = xcom_ / sig_;
				int y = img->rows / 2;
				circles[circlesindex].center = Point(x, y);
				circles[circlesindex].rad = dia_ / 2;
				circlesindex++;
				circle(*img, Point(x, y), dia_/2, 255);



				xcom_ = 0;
				sig_ = 0;
				dia_ = 0;
			}			
		}
		else {
			xcom_ += marbles[i].xcom* marbles[i].significance;
			sig_ += marbles[i].significance;
			dia_ += img->cols / size;
		}
	}
	if (sig_ > 0) {
		int x = xcom_ / sig_;
		int y = img->rows / 2;
		circles[circlesindex].center = Point(x, y);
		circles[circlesindex].rad = dia_ / 2;
		circlesindex++;
		circle(*img, Point(x, y), dia_ / 2, 255);
	}



	return circles;
}


double ImageMarbleDetector::optimizedCIM(Mat* img, bool draw)
{
	//Binarify
	Mat cImg;
	cImg = img->clone();
	cImg = rotatingMaskFilter(img);

	cvtColor(cImg, cImg, COLOR_BGR2GRAY);
	removeFloors(&cImg);
	shineMarker(&cImg);
	removeInsignificant(&cImg);
	removeShades(&cImg);


	Circles* circles = findParameters(&cImg);
	if (circles == NULL)	//When no marbles in image
		return NULL;

	int i = 0;
	int biggestrad = 0;
	int biggestradindex = 0;
	while (circles[i].rad != -1) {
		if (draw) {
			circle(*img, circles[i].center, circles[i].rad, (0,0,255));
		}
		if (circles[i].rad > biggestrad) {
			biggestrad = circles[i].rad;
			biggestradindex = i;
		}	
		i++;
	}
	double reply = (double) circles[biggestradindex].center.x / img->cols;
	reply = (double) reply * (double) IMAGEXANGLE - ((double) IMAGEXANGLE / 2);

	if (draw)
	{
		namedWindow("image", WINDOW_GUI_NORMAL);
		cv::imshow("image", *img);
		cv::waitKey();
	}

	delete[] circles;
	cImg.release();
	img->release();

	return reply;
}


ImageMarbleDetector::~ImageMarbleDetector() {}
