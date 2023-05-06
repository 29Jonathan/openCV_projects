#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

Mat imgHSV, mask, mask1, cleanImage, dstImage, imgCopy2;

void getContours(Mat&, Mat&);
RotatedRect getEllipse(Mat&, Mat&);
Mat rotation(Mat&, RotatedRect&);
//void getCenter(Mat&, Mat&);

int main(int argc, char* argv[]) {

    //string path = "/Users/jonathan/Desktop/HW1/testing source/public_08.png";
    string path = argv[1];
    Mat img = imread(path);
    Mat imgCopy = img.clone();

    cvtColor(img, imgHSV, COLOR_BGR2HSV);
    inRange(imgHSV, Scalar(0, 0, 221), Scalar(180, 30, 255), mask); /* Keeping only the white part of imgHSV -> "mask" */
    getContours(mask, imgCopy); /* get Contours and fill it with purple color(255, 0, 255) -> "imgCopy" */
    inRange(imgCopy, Scalar(255, 0, 255), Scalar(255, 0, 255), mask1); /* keeping only the purple color of imgCopy -> "mask1" */

    cleanImage = Mat::zeros(img.size(), img.type());
    img.copyTo(cleanImage, mask1); /* Masking img with mask1 -> "cleanImage" */

    /////////////////first part done//////////////////

    imgCopy2 = Mat::zeros(img.size(), img.type());
    RotatedRect box = getEllipse(mask1, imgCopy2); /* Fitting an ellipse and return the ellipse */
    dstImage = rotation(cleanImage, box); /* rotate the image */

    /////////////////second part done//////////////////

    imwrite(argv[2], dstImage);
    string outPath = argv[2];
//    imshow("Image", img);
//    imshow("ImageM", mask1);
//    imshow("ImageCOPY2", imgCopy2);
//    imshow("cleanImage", cleanImage);
//    imshow("dstImage", dstImage);
//    waitKey(0);
    return 0;
}

void getContours(Mat& inputImg, Mat& outImg) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(inputImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    //cout << contours.size() << endl;
    for(int i = 0; i < contours.size(); i++){
        int area = contourArea(contours[i]);
        //cout << area << endl;
        if(area > 50000)
            drawContours(outImg, contours, i, Scalar(255, 0, 255), -1);
    }
}

RotatedRect getEllipse(Mat& inputImg, Mat& outImg)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(inputImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    if(contours.size() > 1){
        cout << "Something wrong.";
        exit(1);
    }

    RotatedRect box = fitEllipse(contours[0]);
    ellipse(outImg, box, Scalar(255, 0, 255), 1, __LINE__);
    return box;
}

Mat rotation(Mat& inputImg, RotatedRect& box)
{
    Mat outImg;
    float xc    = box.center.x;
    float yc    = box.center.y;
    float theta = box.angle;
    //cout << xc << ' ' << yc << ' ' << theta << endl;

    if(theta > 90.0)
        theta = theta - 180.0;

    Point2f pt(xc, yc);          /*point from where to rotate */
    Mat r = getRotationMatrix2D(pt, theta, 1.0);      /*Mat object for storing after rotation */
    warpAffine(inputImg, outImg, r, Size(inputImg.cols, inputImg.rows)); /* applied an affine transformation to image. */
    return outImg;
}

//void getCenter(Mat& inputImg, Mat& outImg)
//{
//    Moments m = moments(inputImg,true);
//    Point p(m.m10/m.m00, m.m01/m.m00);
//    cout<< Mat(p)<< endl;
//    circle(outImg, p, 3, Scalar(255,0,255), -1);
//}
