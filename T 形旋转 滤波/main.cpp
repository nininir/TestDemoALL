#include<opencv2\opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

Mat RotateSmooth4Line_2(Mat src)
{
	Mat src_Border;
	copyMakeBorder(src, src_Border, 2, 2, 2, 2, BORDER_CONSTANT, Scalar(0));//外围扩充2行2列
	
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
	int Mask_index_final = 0; //选中的模板 标号

	int MaskPointCount = 0;//src 在模板中的点 数目
	int Mask_Sum = 0;//选中的模板 像素和


	uchar*data_src_Border = src_Border.data;
	uchar*data_dst = dst.data;

	for (int j = 2; j < src_row; j++)
	{
	  int Mask_PointCount = 0;//选中的模板 有效像素个数

 		for (int i = 2; i < src_col; i++)
		{
			int Min_Delta_Square = 1000000; //选中的模板 亮度散布值
			for (int m = 0; m < 8; m++)
			{
				int MaskSum = 0;//计算像素和
				int Mask_Square_Sum = 0;//计算 像素 平方和
				int MaskPointCount = 0;

				for (int n = 0; n < 7; n++)
				{
					int x = i + Mask[m][n][0];
					int y = j + Mask[m][n][1];

					//寻点的三种方式

					uchar t = data_src_Border[y*src_Border.step + x];
					//uchar t = src_Border.at<uchar>(y, x);
					//uchar t = src_Border.at<uchar>(Point(x,y));

					//如果遍历到的点在图像内部，MaskPointCount加一
					if (t != 0)
					{
						MaskSum += t;
						Mask_Square_Sum += t*t;
						++MaskPointCount;
					}
				}

				//遍历结束，计算亮度散布值
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
	cout << "模板 尺寸是 ：" << MaskSize << endl;
	Mat dst(src.size(), CV_8UC1,Scalar(0));
	uchar *dst_data = dst.data;

	uchar *src_data = src.data;

	//双层循环 遍历像素点
	for (int j = 0; j < src.rows; j++)
	{
		for (int  i = 0; i < src.cols; i++)
		{
			int sum_final, count_final;		
			int Min_mask_Delta = 1000000;
			//双层循环 定位 3*3旋转模板 的中心 变化位置center
			for (int m = -Mask_R; m <= Mask_R; m++)
			{
				for (int n = -Mask_R; n <= Mask_R; n++)
				{
					//剔除 最中间的 3*3 模板
					if (m != 0 && n != 0)
					{
						int mask_Sum = 0;//模板像素 累加和
						int mask_Square_Sum = 0;//模板像素 平方累加和 
						int mask_Count = 0;// 模板中 在 图像内部的 像素点数量
						int mask_Delta = 0;//亮度散步值
						//双层循环 以某个center为中心  构建3*3 模板 遍历所有点坐标（得到偏移量）
						for (int a = -Mask_R; a <= Mask_R; a++)
						{
							for (int b = -Mask_R; b <= Mask_R; b++)
							{
								//计算 遍历像素点 偏移后 的坐标
								int x = i + n + b;
								int y = j + m + a;

								//保证 遍历像素点在 图像内部
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
			int mask_Index = 0;//选中 的模板 标号
			int mask_Count_Final = 0;//选中模板标号中 像素个数
			int mask_Sum_final = 0;//选中的模板 像素 和
			int Min_Delta_Square = 10000000;

			for (int m = 0; m < 9; m++)
			{
				int maskSum = 0;//模板 所有像素点之和
				int maskSquareSum = 0;//模板 所有像素 平方和
				int maskCount = 0;//模板 所有像素个数
				int maskDaltaSquare = 0;// 计算 亮度散步值
				

				if (m != 8)
				{
					for (int n = 0; n < 7; n++)
					{
						int x = i + Mask[m][n][0];
						int y = j + Mask[m][n][1];//mask 中的x坐标 在图像中 是cols

						//模板遍历的像素点 保证在原图内
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