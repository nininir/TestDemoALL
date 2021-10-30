#include<iostream>
#include<opencv2\opencv.hpp>

using namespace cv;
using namespace std;

Mat src, blur_img, graying, cannyimg;
//霍夫圆变换参数
int centerdist, cannyvalue, roundness, minradius, maxradius;

//滑动条回调函数
void on_centerdist(int,void*);
void on_cannyvalue(int, void*);
void on_roundness(int, void*);
void on_minradius(int, void*);
void on_maxradius(int, void*);

//储存圆轮廓
std::vector<Vec3f> circles;

int main() 
{
	system("color 02");
	//霍夫圆变换的参数初始值
	centerdist = 10;
	cannyvalue = 100;
	roundness = 30;
	minradius = 15;
	maxradius = 50;

	namedWindow("canny", CV_WINDOW_AUTOSIZE);
	namedWindow("dst", CV_WINDOW_AUTOSIZE);

	//创建各个参数的滑动条
	createTrackbar("cannyvalue", "canny", &cannyvalue, 300, on_cannyvalue);
	on_cannyvalue(0, 0);

	createTrackbar("centerdist", "canny", &centerdist, 100, on_centerdist);
	on_centerdist(0, 0);

	createTrackbar("roundness", "canny", &roundness, 100, on_roundness);
	on_roundness(0, 0);

	createTrackbar("minradius", "canny", &minradius, 100, on_minradius);
	on_minradius(0, 0);

	createTrackbar("maxradius", "canny", &maxradius, 100, on_maxradius);
	on_maxradius(0, 0);
	

	waitKey();
	return 0;

}

void on_cannyvalue(int, void*)
{
	src = imread("3.jpg");
	
	GaussianBlur(src, blur_img, Size(3, 3),0);

	//namedWindow("gauss",CV_WINDOW_AUTOSIZE);
	//imshow("gauss", blur_img);

	cvtColor(src, graying, CV_BGR2BGRA);

	Canny(graying, cannyimg, cannyvalue / 2, cannyvalue, 3);

	//imshow("canny", cannyimg);

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	//膨胀
	morphologyEx(cannyimg, cannyimg, MORPH_DILATE, element);

	imshow("canny", cannyimg);	
}
void on_centerdist(int, void*)
{
	src = imread("3.jpg");
	//清空原来的轮廓
	circles.clear();

	//霍夫圆
	HoughCircles(cannyimg, circles, HOUGH_GRADIENT, 1, centerdist, cannyvalue, roundness, minradius, maxradius);

	//画出找到的霍夫圆
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		circle(src, center, radius, Scalar(0, 0, 255), 2, 8, 0);
	}

	putText(src, to_string((unsigned int)circles.size()), Point(1, 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 2, 8);
	imshow("dst", src);
}
void on_roundness(int,void*)
{
	src = imread("3.jpg");
	circles.clear();
	// HoughCircles:第二个参数是每个圆由三个浮点数表示：圆心坐标(x,y)和半径.
	HoughCircles(cannyimg, circles, HOUGH_GRADIENT, 1, centerdist, cannyvalue, roundness, minradius, maxradius);
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

		int radius = cvRound(circles[i][2]);
		circle(src, center, radius, Scalar(0, 0, 255), 2, 8, 0);
	}
	putText(src, to_string((unsigned int)circles.size()), Point(1, 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 255, 0), 2, 8);
	imshow("dst", src);
}

void on_minradius(int, void*)
{
	src = imread("3.jpg");
	circles.clear();

	HoughCircles(cannyimg, circles, HOUGH_GRADIENT, 1, centerdist, cannyvalue, roundness, minradius, maxradius);
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

		int radius = cvRound(circles[i][2]);
		circle(src, center, radius, Scalar(0, 0, 255), 2, 8, 0);
	}
	putText(src, to_string((unsigned int)circles.size()), Point(1, 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 255, 0), 2, 8);
	imshow("dst", src);
}

void on_maxradius(int, void*)
{
	src = imread("3.jpg");
	circles.clear();

	HoughCircles(cannyimg, circles, HOUGH_GRADIENT, 1, centerdist, cannyvalue, roundness, minradius, maxradius);
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

		int radius = cvRound(circles[i][2]);
		circle(src, center, radius, Scalar(0, 0, 255), 2, 8, 0);
	}
	putText(src, to_string((unsigned int)circles.size()), Point(1, 25), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 255, 0), 2, 8);
	imshow("dst", src);
}