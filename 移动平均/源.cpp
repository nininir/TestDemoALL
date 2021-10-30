#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
//#####当感兴趣的物体与图像尺寸相比较小(或较细)时，基于移动平均的阈值处理会工作的很好######

//Exponential Moving Average:指数平滑移动平均线
void EMA(Mat &src,int N,double b)
{
	Mat src1;
	src1 = src.clone();
	Mat initialRect = src1(Rect(0, 0, N, 1));
	Scalar m0 = mean(initialRect);
	double m_last = m0[0];
	
	uchar *iData = src1.data;
	for (int j = 0; j < src1.rows; j++)
	{
		for (int i = 0; i < src1.cols; i++)
		{
			double X = iData[j*src1.step + i];
			double m_next = 2 * X / (N + 1) + (N - 1)*m_last / (N + 1);
			if (X > m_next*b)
				iData[j*src1.step + i] = 255;
			else
				iData[j*src1.step + i] = 0;
			m_last = m_next;
		}
	}
	Mat ss;
}

int main()
{
	Mat src = imread("1.tif", 0);
	if (!src.data)
	{
		cout << "imread error!";
		return -1;
	}

	imshow("src", src);

	EMA(src, 20, 0.2);

	waitKey(0);
	return 0;
}