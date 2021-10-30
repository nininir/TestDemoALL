#include<opencv2\opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

void hole_filling(Mat &src)
{
	Mat src0 = src.clone();
	
	//��ֵͼ��
	threshold(src0, src0, 200, 1, THRESH_BINARY | THRESH_OTSU);

	Mat ddd,sss;
	//ȡ��
	Mat I;
	I = 1 - src0;
	sss = 255 * I;
	
	//���ͼ��
	Mat F(src.size(), CV_8UC1, Scalar(0));
	/*uchar *f_data = F.data;
	uchar *s_data = src0.data;
	for (int j = 0; j < F.rows; j++)
	{
		for (int i = 0; i < F.cols; i++)
		{
			if (j == 0 || j == F.rows - 1)
			{
				f_data[j*F.step + i] = 1 - s_data[j*F.step + i];
			}
			else if (i == 0 || i == F.cols - 1)
			{
				f_data[j*F.step + i] = 1 - s_data[j*F.step + i];
			}
		}
	}*/

	//�����ṹԪF
	erode(I, F, getStructuringElement(MORPH_RECT, Size(33, 55)));
	
	Mat F1;
	F1 = F.clone();
	ddd = 255 * F;

	//���ͱ��ͼ��
	Mat mask_pre, mask_after, dilation;
	long t, t1;
	//�� 0 ���ص����
	do 
	{
		mask_pre = F1.clone();
		dilate(F1, dilation, getStructuringElement(MORPH_RECT, Size(3, 3)));
		bitwise_and(dilation, I, F1);
		ddd = F1 * 255;

		 t = countNonZero(mask_pre);
		 t1 = countNonZero(F1);
	} while (t != t1);
	

	//ȡ��
	Mat	_andImage;
	_andImage = 1 - F1;

	//�׶����
	Mat dst;
	bitwise_and(_andImage, I, dst);

	dst = 255 * dst;
	imshow("dst", dst);
}

int main()
{
	Mat src, dst;
	
	//ע�� �׶�����һ����0 (����Ϊ��)��������
	src = imread("1.tif", 0);
	/*src = 255 - src;*/
	if (!src.data)
	{
		cout << "image is not read!";
		return -1;
	}

	hole_filling(src);

	imshow("src", src);
	waitKey(0);
	return 0;
}
