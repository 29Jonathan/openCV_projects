#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <iostream>

using namespace cv;
using namespace std;

Mat imgGray, imgBlur, imgCanny, imgDil;
Mat r,r2,y,b,black; //各顏色的閥值
Mat hsv; //轉到hsv平面
Mat dst; //結果圖片


void getContours(Mat& inputImg, Mat& outImg) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(inputImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    //cout << contours.size() << endl;
    for(int i = 0; i < contours.size(); i++){
        int area = contourArea(contours[i]);
        //cout << area << endl;
        if(area >2300)
            drawContours(outImg, contours, i, Scalar(255, 0, 255), 1);
    }
}

//參考答案
//
//public1
//262 17 337 84
//
//public2
//78 218 175 308
//192 39 310 153
//194 172 313 297
//
//public3
//46 90 194 218
//229 55 396 223
//420 1 639 217
//
//public4
//495 242 593 345
//606 368 708 469
//718 495 823 597
int main(int argc, char* argv[]) {
    string path = "/Users/jonathan/Desktop/public/2.jpeg";
    //string path = argv[1];
    Mat img = imread(path);

    Mat mask=Mat::zeros(img.rows,img.cols, CV_8U); //為了濾掉其他顏色

    GaussianBlur(img, imgBlur, Size(3, 3), 3, 0);
    cvtColor(imgBlur,hsv,CV_BGR2HSV);//轉成hsv平面

    inRange(hsv,Scalar(0,80,46) , Scalar(10,255,255), r);

    inRange(hsv,Scalar(155,80,46) , Scalar(180,255,255), r2);

    inRange(hsv,Scalar(15,50,46) ,Scalar(27,255,255), y);

    inRange(hsv,Scalar(103,100,46) , Scalar(125,255,255), b);

    //inRange(hsv,Scalar(0,0,0) , Scalar(180,255,50), black);


    mask=r+r2+y+b;//全部的二值化圖累加起來就變成遮罩

    img.copyTo(dst,mask ); //將原圖片經由遮罩過濾後，得到結果dst

    //cvtColor(dst, imgGray, COLOR_BGR2GRAY);
    GaussianBlur(mask, imgBlur, Size(3, 3), 3, 0);
    Canny(imgBlur, imgCanny, 25, 75);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(4, 4));
    dilate(imgCanny, imgDil, kernel);
    Mat imgCopy = Mat::zeros(img.rows,img.cols, CV_8U);
    getContours(imgDil, imgCopy);

    vector<Point> tlPoints;
    vector<Point> brPoints;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(imgCopy, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
    for (size_t t = 0; t < contours.size(); ++t) {
        // 最大外接矩形
        Rect rect = boundingRect(contours[t]);
        rectangle(img, rect, Scalar(0, 0, 255));

        tlPoints.push_back(rect.tl());
        brPoints.push_back(rect.br());
    }


//    Mat cornerStrength;
//    cornerHarris(imgCopy, cornerStrength, 3, 3, 0.01);
//    threshold(cornerStrength, cornerStrength, 0.01, 255, THRESH_BINARY);
//    imshow("corner", cornerStrength);


//
//    for (size_t t = 0; t < contours.size(); t++)
//        for (size_t j = t+1; j < contours.size(); j++)
//        {
//            if(tlPoints[t].x > tlPoints[j].x){
//                Point temp = tlPoints[t];
//                tlPoints[t] = tlPoints[j];
//                tlPoints[j] = temp;
//                temp = brPoints[t];
//                brPoints[t] = brPoints[j];
//                brPoints[j] = temp;
//            }
//            else if((tlPoints[t].x == tlPoints[j].x) && (tlPoints[t].y > tlPoints[j].y))
//            {
//                Point temp = tlPoints[t];
//                tlPoints[t] = tlPoints[j];
//                tlPoints[j] = temp;
//                temp = brPoints[t];
//                brPoints[t] = brPoints[j];
//                brPoints[j] = temp;
//            }
//        }
//
//    for (size_t t = 0; t < contours.size(); ++t)
//        cout << tlPoints[t].x << ' ' << tlPoints[t].y << ' ' << brPoints[t].x << ' ' << brPoints[t].y << endl;


//15 27 50 255 46 255 yellow
//103 125 100 255 46 255 blue
//155 179 80 255 46 255 red
//0 10 80 255 46 255 red
//0 179 0 255 0 50 black

    imshow("imgCopy", imgCopy);
    imshow("mask", mask);
//    imshow("dst", dst);
    imshow("img", img);
    imshow("imgCanny", imgCanny);
    waitKey(0);
    return 0;
}












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
//    string path = "/Users/jonathan/Desktop/public/public2.jpg";
//    //string path = argv[1];
//    Mat img = imread(path);
//    GaussianBlur(img, imgBlur, Size(3, 3), 3, 0);
//    cvtColor(imgBlur,hsv,CV_BGR2HSV);//轉成hsv平面
//
////15 27 50 255 46 255
////103 125 100 255 46 255
////155 179 80 255 46 255
////0 10 80 255 46 255
////0 179 0 255 0 50
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
//



//
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
//#include <iostream>
//using namespace cv;
//using namespace std;
//Mat src, src_gray;
//int thresh = 200;
//int max_thresh = 255;
//const char* source_window = "Source image";
//const char* corners_window = "Corners detected";
//void cornerHarris_demo( int, void* );
//int main( int argc, char** argv )
//{
////    CommandLineParser parser( argc, argv, "{@input | building.jpg | input image}" );
////    src = imread( samples::findFile( parser.get<String>( "@input" ) ) );
//    string path = "/Users/jonathan/Desktop/public/public2.jpg";
//    //string path = argv[1];
//    Mat src = imread(path);
//    if ( src.empty() )
//    {
//        cout << "Could not open or find the image!\n" << endl;
//        cout << "Usage: " << argv[0] << " <Input image>" << endl;
//        return -1;
//    }
//    cvtColor( src, src_gray, COLOR_BGR2GRAY );
//    namedWindow( source_window );
//    createTrackbar( "Threshold: ", source_window, &thresh, max_thresh, cornerHarris_demo );
//    imshow( source_window, src );
//    cornerHarris_demo( 0, 0 );
//    waitKey();
//    return 0;
//}
//void cornerHarris_demo( int, void* )
//{
//    int blockSize = 2;
//    int apertureSize = 3;
//    double k = 0.04;
//    Mat dst = Mat::zeros( src.size(), CV_32FC1 );
//    cornerHarris( src_gray, dst, blockSize, apertureSize, k );
//    Mat dst_norm, dst_norm_scaled;
//    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
//    convertScaleAbs( dst_norm, dst_norm_scaled );
//    for( int i = 0; i < dst_norm.rows ; i++ )
//    {
//        for( int j = 0; j < dst_norm.cols; j++ )
//        {
//            if( (int) dst_norm.at<float>(i,j) > thresh )
//            {
//                circle( dst_norm_scaled, Point(j,i), 5,  Scalar(0), 2, 8, 0 );
//            }
//        }
//    }
//    namedWindow( corners_window );
//    imshow( corners_window, dst_norm_scaled );
//}
