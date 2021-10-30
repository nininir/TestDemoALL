#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

Mat src, dst, med_img, result;
int min_Value = 1;
int max_Value = 30;

void callBack(int, void*)
{
	Mat thre;
	adaptiveThreshold(med_img, thre, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, min_Value, min_Value);

	imshow("adthreshold", thre);
}

int main()
{
	src = imread("1.bmp", 0);
	if (!src.data)
	{
		cout << "imread is error!";
		return -1;
	}


	Point2f center = Point2f(src.cols / 2, src.rows / 2); //极坐标在图像中的原点
	warpPolar(src, dst, Size(300 , 3274), center, center.x, INTER_LINEAR + WARP_POLAR_LINEAR);

	blur(dst, med_img, Size(3, 3));

	createTrackbar("blocksize:","adthreshold", &min_Value, max_Value, callBack);
	createTrackbar("C:", "adthreshold", &min_Value, max_Value, callBack);

	callBack(min_Value, 0);
	callBack(min_Value, 0);

	imshow("src", src);
	waitKey(0);
	return 0;
}