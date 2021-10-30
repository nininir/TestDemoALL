#include<opencv2\opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

Mat RotateSmooth4Line_2(Mat src)
{
	Mat src_Border;
	copyMakeBorder(src, src_Border, 2, 2, 2, 2, BORDER_CONSTANT, Scalar(0));//��Χ����2��2��
	
	Mat dst = Mat(src_Border.size(), CV_8UC1,Scalar(0));

	int Mask[8][7][2] = {
		{ { -1, 2 },{ 0, 2 },{ 1, 2 },{ 0, 1 },{ 0, 0 },{ 0, -1 },{ 0, -2 } },
		{ { 1, 2 },{ 2, 2 },{ 2, 1 },{ 1, 1 },{ 0, 0 },{ -1, -1 },{ -2, -2 } },
		{ { 2, 1 },{ 2, 0 },{ 2, -1 },{ 1, 0 },{ 0, 0 },{ -1, 0 },{ -2, 0 } },
		{ { 2, -1 },{ 2, -2 },{ 1, -2 },{ 1, -1 },{ 0, 0 },{ -1, 1 },{ -2, 2 } },
		{ { 1, -2 },{ 0, -2 },{ -1, -2 },{ 0, -1 },{ 0, 0 },{ 0, 1 },{ 0, 2 } },
		{ { -1, -2 },{ -2, -2 },{ -2, -1 },{ -1, -1 },{ 0, 0 },{ 1, 1 },{ 2, 2 } },
		{ { -2, -1 },{ -2, 0 },{ -2, 1 },{ -1, 0 },{ 0, 0 },{ 1, 0 },{ 2, 0 } },
		{ { -2, 1 },{ -2, 2 },{ -1, 2 },{ -1, 1 },{ 0, 0 },{ 1, -1 },{ 2, -2 } }
	};

	int src_row = src_Border.rows - 2;
	int src_col = src_Border.cols - 2;

	int Delta_Square = 0;
	int Mask_index_final = 0; //ѡ�е�ģ�� ���

	int MaskPointCount = 0;//src ��ģ���еĵ� ��Ŀ
	int Mask_Sum = 0;//ѡ�е�ģ�� ���غ�


	uchar*data_src_Border = src_Border.data;
	uchar*data_dst = dst.data;

	for (int j = 2; j < src_row; j++)
	{
	  int Mask_PointCount = 0;//ѡ�е�ģ�� ��Ч���ظ���

 		for (int i = 2; i < src_col; i++)
		{
			int Min_Delta_Square = 1000000; //ѡ�е�ģ�� ����ɢ��ֵ
			for (int m = 0; m < 8; m++)
			{
				int MaskSum = 0;//�������غ�
				int Mask_Square_Sum = 0;//���� ���� ƽ����
				int MaskPointCount = 0;

				for (int n = 0; n < 7; n++)
				{
					int x = i + Mask[m][n][0];
					int y = j + Mask[m][n][1];

					//Ѱ������ַ�ʽ

					uchar t = data_src_Border[y*src_Border.step + x];
					//uchar t = src_Border.at<uchar>(y, x);
					//uchar t = src_Border.at<uchar>(Point(x,y));

					//����������ĵ���ͼ���ڲ���MaskPointCount��һ
					if (t != 0)
					{
						MaskSum += t;
						Mask_Square_Sum += t*t;
						++MaskPointCount;
					}
				}

				//������������������ɢ��ֵ
				Delta_Square = (Mask_Square_Sum - MaskSum * MaskSum / MaskPointCount) / MaskPointCount;

				if (Min_Delta_Square > Delta_Square)
				{
					Min_Delta_Square = Delta_Square;
					Mask_index_final = m;
					Mask_Sum = MaskSum;
					Mask_PointCount = MaskPointCount;
				}
			}

			data_dst[j*src_Border.step + i] = (uchar)(Mask_Sum / Mask_PointCount);
		}
	}	
	return dst;
}

Mat RotateSmooth_Rect(Mat src,int Mask_R)
{
	int MaskSize = Mask_R * 2 + 1;
	cout << "ģ�� �ߴ��� ��" << MaskSize << endl;
	Mat dst(src.size(), CV_8UC1,Scalar(0));
	uchar *dst_data = dst.data;

	uchar *src_data = src.data;

	//˫��ѭ�� �������ص�
	for (int j = 0; j < src.rows; j++)
	{
		for (int  i = 0; i < src.cols; i++)
		{
			int sum_final, count_final;		
			int Min_mask_Delta = 1000000;
			//˫��ѭ�� ��λ 3*3��תģ�� ������ �仯λ��center
			for (int m = -Mask_R; m <= Mask_R; m++)
			{
				for (int n = -Mask_R; n <= Mask_R; n++)
				{
					//�޳� ���м�� 3*3 ģ��
					if (m != 0 && n != 0)
					{
						int mask_Sum = 0;//ģ������ �ۼӺ�
						int mask_Square_Sum = 0;//ģ������ ƽ���ۼӺ� 
						int mask_Count = 0;// ģ���� �� ͼ���ڲ��� ���ص�����
						int mask_Delta = 0;//����ɢ��ֵ
						//˫��ѭ�� ��ĳ��centerΪ����  ����3*3 ģ�� �������е����꣨�õ�ƫ������
						for (int a = -Mask_R; a <= Mask_R; a++)
						{
							for (int b = -Mask_R; b <= Mask_R; b++)
							{
								//���� �������ص� ƫ�ƺ� ������
								int x = i + n + b;
								int y = j + m + a;

								//��֤ �������ص��� ͼ���ڲ�
								if (x >= 0 && x < src.cols&& y >= 0 && y < src.rows)
								{
									mask_Sum += src_data[y*src.step + x];
									mask_Square_Sum += src_data[y*src.step + x] * src_data[y*src.step + x];
									mask_Count++;
								}
							}
						}
						mask_Delta = (mask_Square_Sum - mask_Sum * mask_Sum / mask_Count) / mask_Count;
						
						if (Min_mask_Delta > mask_Delta)
						{
							Min_mask_Delta = mask_Delta;
							sum_final = mask_Sum;
							count_final = mask_Count;
						}
					}
				}
			}
			dst_data[j*dst.step + i] = sum_final / count_final;
		}
	}
	return dst;
}


Mat RotateSmooth4Line(Mat src)
{
	Mat dst(src.size(), CV_8UC1,Scalar(0));
	uchar *dst_data = dst.data;

	uchar *src_data = src.data;

	int Mask[8][7][2] = {
		{ { -1, 2 },{ 0, 2 },{ 1, 2 },{ 0, 1 },{ 0, 0 },{ 0, -1 },{ 0, -2 } },
		{ { 1, 2 },{ 2, 2 },{ 2, 1 },{ 1, 1 },{ 0, 0 },{ -1, -1 },{ -2, -2 } },
		{ { 2, 1 },{ 2, 0 },{ 2, -1 },{ 1, 0 },{ 0, 0 },{ -1, 0 },{ -2, 0 } },
		{ { 2, -1 },{ 2, -2 },{ 1, -2 },{ 1, -1 },{ 0, 0 },{ -1, 1 },{ -2, 2 } },
		{ { 1, -2 },{ 0, -2 },{ -1, -2 },{ 0, -1 },{ 0, 0 },{ 0, 1 },{ 0, 2 } },
		{ { -1, -2 },{ -2, -2 },{ -2, -1 },{ -1, -1 },{ 0, 0 },{ 1, 1 },{ 2, 2 } },
		{ { -2, -1 },{ -2, 0 },{ -2, 1 },{ -1, 0 },{ 0, 0 },{ 1, 0 },{ 2, 0 } },
		{ { -2, 1 },{ -2, 2 },{ -1, 2 },{ -1, 1 },{ 0, 0 },{ 1, -1 },{ 2, -2 } }
	};

	for (int j = 0; j < src.rows; j++)
	{
		for (int i = 0; i < src.cols; i++)
		{
			int mask_Index = 0;//ѡ�� ��ģ�� ���
			int mask_Count_Final = 0;//ѡ��ģ������ ���ظ���
			int mask_Sum_final = 0;//ѡ�е�ģ�� ���� ��
			int Min_Delta_Square = 10000000;

			for (int m = 0; m < 9; m++)
			{
				int maskSum = 0;//ģ�� �������ص�֮��
				int maskSquareSum = 0;//ģ�� �������� ƽ����
				int maskCount = 0;//ģ�� �������ظ���
				int maskDaltaSquare = 0;// ���� ����ɢ��ֵ
				

				if (m != 8)
				{
					for (int n = 0; n < 7; n++)
					{
						int x = i + Mask[m][n][0];
						int y = j + Mask[m][n][1];//mask �е�x���� ��ͼ���� ��cols

						//ģ����������ص� ��֤��ԭͼ��
						if (x >= 0 && x < src.cols  && y >= 0 && y < src.rows)
						{
							/*maskSum += src.at<uchar>(Point(x, y));
							maskSquareSum += src.at<uchar>(Point(x, y)) *src.at<uchar>(Point(x, y));*/
							maskSum += src_data[y*src.step + x];
							maskSquareSum += src_data[y*src.step + x] * src_data[y*src.step + x];
							maskCount++;
						}
					}
					maskDaltaSquare = (maskSquareSum - maskSum*maskSum / maskCount) / maskCount;
				}
				else
				{
					for (int a = -1; a < 2; a++)
					{
						for (int b = -1; b < 2; b++)
						{
							int x = j + a;
							int y = i + b;

							if (y >= 0 && y < src.cols  && x >= 0 && x < src.rows)
							{
								maskSum += src_data[x*src.step + y];
								maskSquareSum += src_data[x*src.step + y] * src_data[x*src.step + y];
								maskCount++;
							}
						}
					}
					maskDaltaSquare = (maskSquareSum - maskSum*maskSum / maskCount) / maskCount;
				}

				if (Min_Delta_Square > maskDaltaSquare)
				{
					Min_Delta_Square = maskDaltaSquare;
					mask_Index = m;
					mask_Sum_final = maskSum;
					mask_Count_Final = maskCount;
				}
			}
			dst_data[j*dst.step + i] = (uchar)(mask_Sum_final / mask_Count_Final);
		}
	}

	return dst;
}

int main()
{
	Mat src = imread("1.jpg",0);

	Mat result = RotateSmooth4Line_2(src);
	imshow("result", result);

	waitKey(0);
	return 0;
}