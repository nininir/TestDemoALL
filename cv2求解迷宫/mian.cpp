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
	threshold(copy, copy, thre_value, 255, THRESH_BINARY_INV);//��ת��ɫ
	imshow("0", copy);

	vector<vector<Point>> contours;
	//vector<Vec4i> hierarchy;

	findContours(copy, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	//��������ȫ��ͼ����
	Mat draw;
	draw = Mat::zeros(src.size(), CV_8UC1);
	

	drawContours(draw, contours, 0, Scalar(255), -1);// -1��ʾ���������ڲ��������������ʾ����������ϸ
	imshow("make", draw);

	
    //for (int i = 0; i < contours.size(); i++)
	//{
	//drawContours(draw, contours,i,Scalar(0, 255, 255),1,LINE_8,hierarchy,0,Point(0,0));
	//}
	//imshow("00", draw);


	Mat dilated, eroded;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(15,15));//why��ģ��Ĵ�Сֱ��Ӱ�� ��ʴ�����͵Ľ�� ����Ӱ����

	//����
	dilate(draw, dilated, kernel, Point(-1, -1), 1);//���һ�����ִ������ʹ���
	imshow("dilate", draw);

	//��ʴ
	erode(dilated, eroded, kernel, Point(-1, -1), 1);//���Գ��Ը�ʴ���Σ�ע��۲�����
	imshow("eroded", eroded);

	Mat diff;
	absdiff(dilated, eroded, diff);
	imshow("diff", diff);
	
	vector<vector<Point>> contourss2;
	findContours(diff, contourss2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	drawContours(src, contourss2, 0, Scalar(255,0,0), 8);
	//���һ�����ֱ�ʾ��������λ�ã����Ϊ����ʾ����������ϸ��Ϊ -1 ˵�����������ڲ���
	imshow("result", src);

	waitKey(0);
}