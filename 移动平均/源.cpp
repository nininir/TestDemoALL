#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
//#####������Ȥ��������ͼ��ߴ���Ƚ�С(���ϸ)ʱ�������ƶ�ƽ������ֵ����Ṥ���ĺܺ�######

//Exponential Moving Average:ָ��ƽ���ƶ�ƽ����
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