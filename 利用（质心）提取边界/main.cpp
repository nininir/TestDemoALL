#include<opencv2\opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

void main() {
	Mat src, dst;
	src = imread("E:/win10/28.bmp");
	if (!src.data) {
		printf("error");
		return;
	}

	medianBlur(src, src, 3);

	Mat src_gray;
	cvtColor(src, src_gray, CV_BGR2GRAY);

    //形态学操作 噪点移除
	//Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	//erode(src_gray, src_gray, kernel);
	//dilate(src_gray, src_gray, kernel);
	//imshow("54", src_gray);
	

	Mat thre;
	threshold(src_gray, thre, 90, 255, THRESH_BINARY);
	thre = 255 - thre;
	imshow("input", thre);

	vector<vector<Point>> contours;
	findContours(thre, contours, RETR_TREE, CHAIN_APPROX_NONE);

	std::vector<std::vector<cv::Point> >::iterator itc = contours.begin();

	while (itc != contours.end())
	{
		cv::Moments mom = moments(Mat(*itc));
		//if (mom.m00 == 0)
		//{
		//	double area = cv::contourArea(*itc);
		//	double ll = cv::arcLength(*itc,false);
		//	itc = contours.erase(itc);
		//	continue;
		//}// 噪点移除
		double x = thre.cols / 2 - mom.m10 / mom.m00;
		double y = thre.rows / 2 - mom.m01 / mom.m00;
		double c = sqrt(pow(x, 2) + pow(y, 2));
		if (c > 35)
		{
			itc = contours.erase(itc);
		}
		else
		{
			itc++;
		}
		cout << "x= " << mom.m10 / mom.m00 << " "
			<< "y= " << mom.m01 / mom.m00 << endl;
	}

	Mat drawImage = Mat::zeros(thre.size(), CV_8U);
	//imshow("5", drawImage);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(255);
		drawContours(drawImage, contours, i, color, 2, 8);
	}
	imshow("result", drawImage);

	waitKey(0);
} 