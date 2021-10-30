#include<opencv2\opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

void main() {
	Mat src, dst;
	src = imread("die_pads.png");

	if (!src.data) {
		printf("error");
		return;
	}

	cvtColor(src, src, CV_BGR2GRAY);
 	imshow("input", src);

	threshold(src, dst, 180, 255, THRESH_BINARY);
	imshow("thre", dst); 

	//提取原件的区域
	Mat open_deal;
	Mat kenel = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
	morphologyEx(dst, open_deal, CV_MOP_OPEN, kenel);
	imshow("open", open_deal);

	//消除孔洞
	Mat result;
	morphologyEx(open_deal, result, CV_MOP_CLOSE, kenel);
	imshow("result", result);

	//提取轮廓
	vector<vector<Point>> contour;
	findContours(result, contour, RETR_TREE, CHAIN_APPROX_SIMPLE);
	//imshow("output", open_deal);
	cout << contour.size() << endl;

	//画出原件的区域的最小外接矩形
	Mat huaxian_img(src.size(),CV_8UC1,Scalar(0));
	for (int i = 0; i < contour.size(); i++)
	{
		RotatedRect rect = minAreaRect(contour[i]);
		Point2f p[4];
		rect.points(p);

		for (int j = 0; j < 4; j++)
		{
			line(huaxian_img, p[j], p[(j + 1) % 4], Scalar(255),1,LINE_AA);
		}
	}

	 imshow("deal_image", huaxian_img);

	Mat kenel_1 = getStructuringElement(MORPH_RECT, Size(3,3), Point(-1, -1));
	dilate(huaxian_img, huaxian_img, kenel_1);
	//imshow("huaxian_img", huaxian_img);

	vector<vector<Point>> contour1;
	findContours(huaxian_img, contour1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contour1.size(); i++)
	{
		RotatedRect rect = minAreaRect(contour1[i]);
		Point2f p[4];
		rect.points(p);

		//double degree = rect.angle;区域角度

		for (int j = 0; j < 4; j++)
		{
			line(src, p[j], p[(j + 1) % 4], Scalar(0,255,255), 1, LINE_AA);
		}
	}

  	imshow("kuoda", src);
	//cout << contour.size() << endl;

		
	waitKey(0);
}

