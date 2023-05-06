#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
#include <iomanip>
using namespace cv;
using namespace std;

Mat mask, imgHSV;
const double pi = 3.14159265358979323846;

void findLine(Mat& img, vector<Point> &points, Point_<double> &pointOnLine, double &k);


int main(int argc, char* argv[]) {
    string path = "/Users/jonathan/Desktop/hw2.v3/public/data/public6.jpg";
    //string path = argv[1];
    Mat img = imread(path);
    Mat imgCopy = img.clone();

    blur(img, img, Size(3,3));
    threshold(img, mask, 125, 255, THRESH_BINARY);
    cvtColor(mask, imgHSV, COLOR_RGBA2GRAY);
    Canny(imgHSV, imgHSV, 60, 120);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(imgHSV, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
    drawContours(imgCopy, contours, -1, Scalar(0, 255, 0), -1);

    vector<Point> pointsOfLine1;
    vector<Point> pointsOfLine2;
    int x, y;
    int x1, y1;
    int preY, i;

//    for(int t= 0; t < contours.size(); t++)
//    {
//        for(int j= 0; j < contours[t].size();j++) // run until j < contours[i].size();
//        {
//            cout << contours[t][j] << endl; //do whatever
//        }
//    }
    //cout << contours.size() << endl;
    for(i = 0; i < contours.size(); i++) // storing lower line points
    {
        if( (preY - contours[i][0].y) > 25 ) // 25: the minimal difference of upper and lower line that I set
        {
            //cout << "break out " << preY << " " <<  contours[i][0].y << endl;
            break;
        }

        for(int j= 0; j < contours[i].size();j+=7) // run until j < contours[i].size();
        {
            x = contours[i][j].x;
            y = contours[i][j].y;
            pointsOfLine1.emplace_back(x, y);
        }
        preY = y;
    }
    //cout << "i=" << i <<endl;


    for(; i < contours.size(); i++) // storing upper line points
    {
        //cout << "i=" << i <<endl;
        for(int j= 0; j < contours[i].size();j+=7) // run until j < contours[i].size();
        {
            //cout << contours[i][j].x << " " << contours[i][j].y << endl;
            x1 = contours[i][j].x;
            y1 = contours[i][j].y;
            pointsOfLine2.emplace_back(x1, y1);
        }
    }

    for(int p = 0; p < pointsOfLine1.size(); p++) // lower line circle test
        circle(img, pointsOfLine1[p], 1, Scalar(0, 255, 0), 1, 8, 0);

    for(int p = 0; p < pointsOfLine2.size(); p++) // upper line circle test
        circle(img, pointsOfLine2[p], 1, Scalar(0, 255, 0), 1, 8, 0);


    Point_<double> pointOnLine1, pointOnLine2;
    double k1, k2;
    findLine(img, pointsOfLine1, pointOnLine1, k1);// lower line
    findLine(img, pointsOfLine2, pointOnLine2, k2);// upper line


    double radian1 = atan(k1);
    double radian2 = atan(k2);
    double m = tan((pi/2) - (radian1+radian2)/2);
    Point center = Point( ceil(img.cols/2), ceil(img.rows/2) );

    Point_<double> upperPoint, lowerPoint;
    upperPoint.x = (k2 * pointOnLine2.x - m * center.x + center.y - pointOnLine2.y) / (k2 - m);
    upperPoint.y = (k2*m*(center.x - pointOnLine2.x) - k2 * center.y + m * pointOnLine2.y) / (m - k2);
    lowerPoint.x = (k1 * pointOnLine1.x - m * center.x + center.y - pointOnLine1.y) / (k1 - m);
    lowerPoint.y = (k1*m*(center.x - pointOnLine1.x) - k1 * center.y + m * pointOnLine1.y) / (m - k1);
    line(img, upperPoint, lowerPoint, Scalar(0, 255, 0), 1, LINE_8, 0);


    double dis = sqrt((upperPoint.x-lowerPoint.x) * (upperPoint.x-lowerPoint.x) + (upperPoint.y-lowerPoint.y) * (upperPoint.y-lowerPoint.y));

    cout << fixed << setprecision(4) << dis << endl; // small 1.3 ~ 2.4

    imshow("Image", img);
    imshow("ImageHSV", imgHSV);
    imshow("mask", mask);
    imshow("imgCopy", imgCopy);
    waitKey(0);
    return 0;
}

void findLine(Mat& img, vector<Point> &points, Point_<double> &pointOnLine, double &k)
{
    Vec4f line_para;
    fitLine(points, line_para, DIST_L2, 0, 0.01, 0.01);
    //cout<< "line_para1=" << line_para << endl;
    pointOnLine.x = line_para[2];
    pointOnLine.y = line_para[3];
    k = line_para[1] / line_para[0];

    Point_<double> endPoint1, endPoint2;
    endPoint1.x = 0;
    endPoint1.y = k * (0 - pointOnLine.x) + pointOnLine.y;
    endPoint2.x = img.cols;
    endPoint2.y = k * (img.cols - pointOnLine.x) + pointOnLine.y;

    line(img, endPoint1, endPoint2, Scalar(0, 255, 0), 1, LINE_8, 0);
}
