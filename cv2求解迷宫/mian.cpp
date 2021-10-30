#include<opencv2\opencv.hpp>	
#include<iostream>
#include<vector>

using namespace cv;
using namespace std;

void main() {
	Mat src, gray;
	src = imread("06.jpg");
	if (!src.data)
	{
		printf("error");
		return;
	}


	int thre_value = 175;
	threshold(src, gray, thre_value, 255, THRESH_BINARY);
	imshow("input", gray);

	Mat copy = src.clone();
	//imshow("kcopy", copy);

	cvtColor(copy, copy, CV_BGR2GRAY);
	//Mat thre;
	threshold(copy, copy, thre_value, 255, THRESH_BINARY_INV);//反转颜色
	imshow("0", copy);

	vector<vector<Point>> contours;
	//vector<Vec4i> hierarchy;

	findContours(copy, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	//画轮廓在全黑图像上
	Mat draw;
	draw = Mat::zeros(src.size(), CV_8UC1);
	

	drawContours(draw, contours, 0, Scalar(255), -1);// -1表示绘制轮廓内部，若是正数则表示轮廓线条粗细
	imshow("make", draw);

	
    //for (int i = 0; i < contours.size(); i++)
	//{
	//drawContours(draw, contours,i,Scalar(0, 255, 255),1,LINE_8,hierarchy,0,Point(0,0));
	//}
	//imshow("00", draw);


	Mat dilated, eroded;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(15,15));//why：模板的大小直接影响 腐蚀、膨胀的结果 进而影响结果

	//膨胀
	dilate(draw, dilated, kernel, Point(-1, -1), 1);//最后一个数字代表膨胀次数
	imshow("dilate", draw);

	//腐蚀
	erode(dilated, eroded, kernel, Point(-1, -1), 1);//可以尝试腐蚀两次，注意观察区别
	imshow("eroded", eroded);

	Mat diff;
	absdiff(dilated, eroded, diff);
	imshow("diff", diff);
	
	vector<vector<Point>> contourss2;
	findContours(diff, contourss2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	drawContours(src, contourss2, 0, Scalar(255,0,0), 8);
	//最后一个数字表示轮廓绘制位置，如果为正表示绘制线条粗细，为 -1 说明绘制轮廓内部。
	imshow("result", src);

	waitKey(0);
}