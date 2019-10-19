//
// Created by Sina P. Soltani on 19/10/2019.
//

#include "LidarMarbleDetector.h"

LidarMarbleDetector::LidarMarbleDetector(){}

LidarMarbleDetector::LidarMarbleDetector(double *data, int size, int width, int height) {
    _imageWidth = width;
    _imageHeight = height;
    _size = size;
    
    _numSegments = 0;
    _numPtsInSegment = new int[_size];
    _segments = new Point*[_size];
    for (int j = 0; j < _size; ++j) {
        _segments[j] = new Point[_size];
    }
    
    _lidarData = new double[_size];
    setLidarData(data);
}

void LidarMarbleDetector::drawCircle(Mat * img, Point center, double r){
    circle(*img,center,r,Scalar(70,189,65),10);
}

bool LidarMarbleDetector::isInRange(double range){
    return range < MAX_LIDAR_RANGE;
}

double LidarMarbleDetector::determinant(Point a, Point b){
    return double (b.y - a.y)/(b.x - a.x);
}

double LidarMarbleDetector::solve2LinEq(double a1, double b1, double a2, double b2){
    return (b2 - b1)/(a1 - a2);
}

double LidarMarbleDetector::calculateDistance(double x1, double y1, double x2, double y2){
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

double LidarMarbleDetector::getBiggestDiff(double * arr, int size){
    double max = arr[0];
    int indexOfMax = 0;
    int indexOfMin = 0;
    double min = arr[0];
    for (int i = 0; i < size; ++i) {
        if(arr[i] > max){
            max = arr[i];
            indexOfMax = i;
        }
        if(arr[i] < min){
            min = arr[i];
            indexOfMin = i;
        }
    }
    //cout << "Max: #" << indexOfMax << " " << max << endl;
    //cout << "Min: #" << indexOfMin << " " << min << endl;
    return max - min;
}

void LidarMarbleDetector::perpendicularBisector(Point a, Point b, double *slopePtr, double *interceptPtr){
    double slope = double (b.y - a.y)/(b.x - a.x);
    double nSlope = -1/slope;
    Point midpoint = Point((a.x + b.x)/2,(a.y + b.y)/2);
    double midX = double (a.x + b.x)/2;
    double midY = double (a.y + b.y)/2;

    double intercept = -nSlope * midX + midY;

    *slopePtr = nSlope;
    *interceptPtr = intercept;
}

auto LidarMarbleDetector::calculateCenterAndRadiusOfCircle(Point a, Point b, Point c){

    Point points[3] = {a, b, c};
    double slopes[3] = {0, 0, 0};
    double intercepts[3] = {0, 0, 0};

    double slope, intercept;
    double *slopePtr, *interceptPtr;
    slopePtr = &slope;
    interceptPtr = &intercept;

    int perbendicularBisectorNum = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if(i < j){
                perpendicularBisector(points[i],points[j],slopePtr,interceptPtr);

                slopes[perbendicularBisectorNum] = *slopePtr;
                intercepts[perbendicularBisectorNum] = *interceptPtr;
                perbendicularBisectorNum++;
                //cout << "a = " << *slopePtr << " " << "b = " << *interceptPtr << endl;
            }
        }
    }

    double x = 0, y = 0, radius = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if(i < j){
                x = solve2LinEq(slopes[i],intercepts[i],slopes[j],intercepts[j]);
                y = slopes[i] * x + intercepts[i];
                //cout << "x = " << x << endl;
                //cout << "y = " << y << endl;
                //cout << calculateDistance(x,y,a.x,a.y) << endl;
                //cout << calculateDistance(x,y,b.x,b.y) << endl;
                //cout << calculateDistance(x,y,c.x,c.y) << endl;
                radius = calculateDistance(x,y,a.x,a.y);
            }
        }
    }

    Point center = Point((int)round(x), (int)round(y));

    struct circle {Point center; double radius;};
    return circle{center,radius};
}

bool LidarMarbleDetector::checkForCircles(int numPts, Point* points){

    auto * slopes = new double[numPts];

    if(numPts >= 3){
        for (int i = 0; i < numPts; ++i) {
            slopes[i] = determinant(points[0],points[numPts - 1 - i]);
            /*if(i == 0){
                cout << points[0] << " - " << points[numPts - 1 - i] << " => " << determinant(points[0],points[numPts - 1 - i]) << endl;
            }*/
        }

        double maxDiff = getBiggestDiff(slopes, numPts);
        //cout << "maxDiff: " << maxDiff << endl;
        if(maxDiff > SLOPE_DIFF)
            return true;
    }

    return false;
}

void LidarMarbleDetector::checkSegments(Mat * image){
    for (int i = 0; i < _numSegments; ++i) {
        //cout << "SEGMENT # " << i << endl;
        if (checkForCircles(_numPtsInSegment[i], _segments[i])) {
            Point first = _segments[i][0];
            Point middle = _segments[i][_numPtsInSegment[i] / 2];
            Point last = _segments[i][_numPtsInSegment[i] - 1];
            auto circle = calculateCenterAndRadiusOfCircle(first, middle, last);
            //cout << "RADIUS  " << circle.radius << endl;
            if (circle.radius < MAX_RADIUS)
                drawCircle(image, circle.center, circle.radius);
        }
    }
}

void LidarMarbleDetector::getLidarSegments() {
    Mat image(2000,2000,CV_8UC3,Scalar(255,255,255));

    _numSegments = 0;
    int pointIndex = 0;

    Point startPoint = Point(image.cols / 2, image.rows / 2);
    Point prevEndPoint = Point((int)(100 * _lidarData[0] * sin(ROTATION_OFFSET) + startPoint.x), (int)(100 * _lidarData[0] * cos(ROTATION_OFFSET) + startPoint.y));

    for (int i = 0; i < _size; ++i) {

        int distance = (int) (100 * _lidarData[i]);
        double angle = i * ANGULAR_PREC + ROTATION_OFFSET;

        Point endPoint = Point((int)(distance * sin(angle) + startPoint.x), (int)(distance * cos(angle) + startPoint.y));

        if(isInRange(_lidarData[i])) {
            if(i > 0 && i < _size - 1 && norm(endPoint - prevEndPoint) < THRESHOLD){
                _segments[_numSegments][pointIndex++] = prevEndPoint;
                _segments[_numSegments][pointIndex] = endPoint;
                // add prevEndPoint and endPoint to array
                // decrease indexOfNewestElement to point to last element so it replaces it on next visit
                // [1964,1113], [1925,1087]
                //              [1925,1087], [1886,1063]
            } else {
                // new segment
                _numPtsInSegment[_numSegments++] = ++pointIndex;
                pointIndex = 0;
            }
            prevEndPoint = endPoint;
        }
    }
    /*
    for (int i = 0; i < numSegments; ++i) {
        cout << "Segment #" << i << endl;
        for (int j = 0; j < numPtsInSegment[i]; ++j) {
            cout << segments[i][j] << endl;
        }
        cout << endl;
    }*/
}

Mat LidarMarbleDetector::plotLidarData() {
    Mat image(2000,2000,CV_8UC3,Scalar(255,255,255));
    Scalar color = {0,0,0};
    Point startPoint = Point(image.cols / 2, image.rows / 2);
    Point prevEndPoint = Point((int)(100 * _lidarData[0] * sin(ROTATION_OFFSET) + startPoint.x), (int)(100 * _lidarData[0] * cos(ROTATION_OFFSET) + startPoint.y));

    for (int i = 0; i < _size; ++i) {

        int distance = (int) (100 * _lidarData[i]);
        double angle = i * ANGULAR_PREC + ROTATION_OFFSET;

        Point endPoint = Point((int)(distance * sin(angle) + startPoint.x), (int)(distance * cos(angle) + startPoint.y));

        /*if(true || isInRange(_lidarData[i])) {*/
        if(i > 0 && norm(endPoint - prevEndPoint) < THRESHOLD){
            line(image, prevEndPoint, endPoint, color,5);
            //cout << prevEndPoint << endPoint << endl;
        } else {
            //cout << endl << "New Segment" << endl;
        }

        circle(image, endPoint, 1, color, 5);
        line(image,startPoint,endPoint,color);
        prevEndPoint = endPoint;
        //}
    }
    return image;
}

void LidarMarbleDetector::onSetData(){
    _image = plotLidarData();
    getLidarSegments();
    checkSegments(&_image);

    namedWindow("Lidar Plot", WINDOW_NORMAL);
    resizeWindow("Lidar Plot",_imageWidth,_imageHeight);
    imshow("Lidar Plot", _image);
    waitKey(1);
}

void LidarMarbleDetector::setLidarData(double *data) {
    _lidarData = data;
    onSetData();
}

LidarMarbleDetector::~LidarMarbleDetector(){}
