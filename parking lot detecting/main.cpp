#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/core.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int getContours(Mat& inputImg, Mat& outImg);
Mat imgGray, mask, imgBlur, imgCanny, imgThr, imgDil, imgHsv, masked, imgWB;


int main(int argc, char* argv[]) {
    string path = "/Users/jonathan/Desktop/hw4_public/public4.jpg";
    //string path = argv[1];
    Mat img = imread(path);

    GaussianBlur(img, imgBlur, Size(3, 3), 3, 0);
    cvtColor(imgBlur, imgGray, COLOR_BGR2GRAY);
    cvtColor(imgBlur,imgHsv,CV_BGR2HSV);//轉成hsv平面

    Mat masking = Mat::zeros(img.rows,img.cols, CV_8U);
    inRange(imgHsv,Scalar(0,0,165) , Scalar(179,30,255), masking);
    img.copyTo(masked,masking ); //將原圖片經由遮罩過濾後，得到結果dst

    //threshold(imgGray, imgThr, 150, 255, THRESH_TRUNC);
    adaptiveThreshold(imgGray, imgThr, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 27, 16);
    medianBlur(imgThr, mask, 5);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(mask, imgDil, kernel);
    Mat imgContour = Mat::zeros(img.rows,img.cols, CV_8U);
    //getContours(imgDil, imgContour);

    Mat imgHough = Mat::zeros(img.rows,img.cols, CV_8U);
    Canny(masked, imgCanny, 50, 200, 3);
    // Standard Hough Line Transform
    vector<Vec2f> lines; // will hold the results of the detection
    HoughLines(imgCanny, lines, 1, CV_PI/2, 100, 0, 0 ); // runs the actual detection
    // Draw the lines
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( imgHough, pt1, pt2, Scalar(255,0,255), 3, LINE_MAX);
    }
    threshold(imgHough, imgWB, 150, 255, THRESH_BINARY_INV);
    int totalParkingLot = getContours(imgWB, imgContour);
    //cout << totalParkingLot << endl;
    vector<Point> tlPoints;
    vector<Point> brPoints;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(imgContour, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
    for (size_t t = 0; t < contours.size(); ++t) {
        // 最大外接矩形
        Rect rect = boundingRect(contours[t]);
        //rectangle(img, rect, Scalar(0, 0, 255));

        tlPoints.push_back(rect.tl());
        brPoints.push_back(rect.br());
    }

    int totalCar = 0;
    for(int i = 0; i < totalParkingLot; i++)
    {
        Mat cropped = imgDil(Range(tlPoints[i].y, brPoints[i].y), Range(tlPoints[i].x, brPoints[i].x));
        int count = countNonZero(cropped);
        //cout << count << endl;
        if(count > 900)
            totalCar++;
    }
    cout << totalCar << ' ' << totalParkingLot-totalCar;
//    Mat cropped = imgDil(Range(tlPoints[10].y, brPoints[10].y), Range(tlPoints[10].x, brPoints[10].x));
//    int count = countNonZero(cropped);
//    cout << count << endl;



    imshow("img", img);
    imshow("Canny", imgCanny);
    //imshow("cropped", cropped);
    imshow("imgContour", imgContour);
    imshow("imgmedBlur", mask);
    imshow("imgDil", imgDil);
    imshow("imgHough", imgHough);
//    imshow("imgHoughP", imgHoughP);
//    imshow("masked", masked);
    waitKey(0);
    return 0;
}

int getContours(Mat& inputImg, Mat& outImg) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    int count = 0;
    findContours(inputImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    int maxArea = 0;
    //cout << contours.size() << endl;
    for(int i = 0; i < contours.size(); i++){
        int area = contourArea(contours[i]);
        if(area > maxArea)
            maxArea = area;
    }

    for(int i = 0; i < contours.size(); i++){
        int area = contourArea(contours[i]);
        if(area > maxArea-(maxArea/2))
        {
            count++;
            drawContours(outImg, contours, i, Scalar(255, 0, 255), 1);
        }
    }
    return count;
}


/////////////////////////// 0 179  0 30  177 255 /////////////////////
//
//#include <opencv2/opencv.hpp>
//#include <opencv2/imgproc/imgproc_c.h>
//#include <iostream>
//
//using namespace cv;
//using namespace std;
//
//
//Mat hsv, mask, imgBlur; //轉到hsv平面
//
//int hmin = 0, smin=0, vmin=0;
//int hmax = 0, smax=0, vmax=0;
//
//
//int main(int argc, char* argv[]) {
//    string path = "/Users/jonathan/Desktop/hw4_public/public3.jpg";
//    //string path = argv[1];
//    Mat img = imread(path);
//    GaussianBlur(img, imgBlur, Size(3, 3), 3, 0);
//    cvtColor(imgBlur,hsv,CV_BGR2HSV);//轉成hsv平面
//
//    namedWindow("trackbars", (640, 200));
//    createTrackbar("hue min", "trackbars", &hmin, 179);
//    createTrackbar("hue max", "trackbars", &hmax, 179);
//    createTrackbar("sat min", "trackbars", &smin, 255);
//    createTrackbar("sat max", "trackbars", &smax, 255);
//    createTrackbar("val min", "trackbars", &vmin, 255);
//    createTrackbar("val max", "trackbars", &vmax, 255);
//
//    while(true)
//    {
//        Scalar lower(hmin, smin, vmin);
//        Scalar upper(hmax, smax, vmax);
//        inRange(hsv, lower, upper, mask);
//        imshow("mask", mask);
//        waitKey(1);
//    }
//
//    return 0;
//}

