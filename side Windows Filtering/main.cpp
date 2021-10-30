#include<opencv2\opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

//针对灰度图的均值滤波+CVPR 2019的SideWindowFilter
//其他种类的滤波直接换核即可
//初始化 模板
//int cnt[8] = { 0,0,0,0,0,0,0,0 };
//vector<int> filter[8];
//void InitFilter(int radius)
//{
//	int n = radius * 2 + 1;
//	for (int i = 0; i < 8; i++)
//	{
//		cnt[i] = 0;
//		filter[i].clear();
//	}
//	for (int i = 0; i < 8; i++)
//	{
//		for (int x = 0; x < n; x++)
//		{
//			for (int y = 0; y < n; y++)
//			{
//				if (i == 0 && x <= radius && y <= radius)
//				{
//					filter[i].push_back(1);
//				}
//				else if (i == 1 && x <= radius && y >= radius)
//				{
//					filter[i].push_back(1);
//				}
//				else if (i == 2 && x >= radius && y <= radius)
//				{
//					filter[i].push_back(1);
//				}
//				else if (i == 3 && x >= radius && y >= radius)
//				{
//					filter[i].push_back(1);
//				}
//				else if (i == 4 && x <= radius)
//				{
//					filter[i].push_back(1);
//				}
//				else if (i == 5 && x >= radius)
//				{
//					filter[i].push_back(1);
//				}
//				else if (i == 6 && y >= radius)
//				{
//					filter[i].push_back(1);
//				}
//				else if (i == 7 && y <= radius)
//				{
//					filter[i].push_back(1);
//				}
//				else
//				{
//					filter[i].push_back(0);
//				}
//			}
//		}
//	}
//	for (int i = 0; i < 8; i++)
//	{
//		int sum = 0;
//		for (int j = 0; j < filter[i].size(); j++) sum += filter[i][j] == 1;
//		cnt[i] = sum;
//	}
//}

vector<Mat> Mask;

void InitFilter_1(int radius)
{
	int kernel = radius * 2 + 1;	
	for (int i = 0; i < 8; i++)
	{
		Mat temp = Mat::zeros(Size(kernel, kernel), CV_8UC1);
		Mask.push_back(temp);
	}

	for (int j = 0; j < kernel; j++)
	{
		for (int i = 0; i < kernel; i++)
		{
			//第1个 滤波模板
			if (i <= radius && j <= radius )
			{
				Mask[0].at<uchar>(j, i) = 1;
			}
			//第2个 滤波模板
			if (i >= radius && i < kernel && j <= radius )
			{
				Mask[1].at<uchar>(j, i) = 1;
			}
			//第3个 滤波模板
			if (i <= radius && j >= radius  && j < kernel)
			{
				Mask[2].at<uchar>(j, i) = 1;
			}

			if (i >= radius && i < kernel && j >= radius  && j < kernel)
			{
				Mask[3].at<uchar>(j, i) = 1;
			}
			//第5个 滤波模板
			if (j <= radius )
			{
				Mask[4].at<uchar>(j, i) = 1;
			}

			if (j >= radius && j < kernel)
			{
				Mask[5].at<uchar>(j, i) = 1;
			}
			
			if (i >= radius && i < kernel)
			{
				Mask[6].at<uchar>(j, i) = 1;
			}

			if (i <= radius)
			{
				Mask[7].at<uchar>(j, i) = 1;
			}
		}
	}
	/*imshow("dsf", Mask[0]);
	waitKey(0);*/
}

//求和
int add_mask(Mat mask, int radius)
{
	int Sum = 0;
	uchar *mask_data = mask.data;
	for (int j = 0; j < mask.rows; j++)
	{
		for (int i = 0; i < mask.cols; i++)
		{
			Sum += mask_data[j*mask.step + i];
		}
	}	
	return Sum;
}

Mat SideWindowFilter(Mat src, int radius)
{
	InitFilter_1(radius);

	if (src.channels() == 1)
	{
		Mat dst(src.size(), CV_8UC1, Scalar(0));

		uchar *src_data = src.data;
		uchar *dst_data = dst.data;

		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				if (i < radius || i + radius >= src.cols || j < radius || j + radius >= src.rows)
				{
					dst_data[j*src.step + i] = src_data[j*src.step + i];
					continue;
				}

				int m_final = 0;
				float Min_Pix = 3000.0;
				int val_final = 0;
				for (int m = 0; m < 8; m++)
				{
					float val = 0.0;
					uchar *mask_data = Mask[m].data;
					int Mask_Sum = add_mask(Mask[m], radius);

					for (int b = -radius; b <= radius; b++)
					{
						for (int a = -radius; a <= radius; a++)
						{
							int y = j + b;
							int x = i + a;

							int mask_y = radius + b;
							int mask_x = radius + a;

							////遍历点 在图像内部
							//if (x >= 0 && x < src.cols && y >= 0 && y < src.rows)
							//{
							//}
							//float tt =(float) (src_data[y*src.step + x]);// 这样写 便于 调试
							val += ((float)(src_data[y*src.step + x]) * mask_data[mask_y*Mask[m].step + mask_x]) / Mask_Sum;
						}
					}
					float t = abs(val - src_data[j*src.step + i]);
					if (Min_Pix > t)
					{
						Min_Pix = t;
						m_final = m;
						val_final = val;
					}
				}
				dst_data[j*src.step + i] = (uchar)val_final;
			}
		}
		return dst;
	}
	else if (src.channels() == 3)
	{
		Mat dst(src.size(), CV_8UC3);

		int ncols = 3 * src.cols;

		for (int c = 0; c < 3; c++)
		{
			for (int j = 0; j < src.rows; j++)
			{
				for (int i = 0; i < ncols; i++)
				{
					//注意 3通道的图像 指针遍历时  指针的类型为 Vec3b
					uchar * src_data = src.data;
					uchar * dst_data = dst.data;

					if (i < radius || i + radius >= ncols || j < radius || j + radius >= src.rows)
					{
						dst_data[j*dst.step + i + c] = src_data[j*src.step + i + c];
						continue;
					}
					int m_final = 0;
					float Min_Pix = 3000.0;
					int val_final = 0;
					for (int m = 0; m < 8; m++)
					{
						float val = 0.0;
						uchar *mask_data = Mask[m].data;
						int Mask_Sum = add_mask(Mask[m], radius);

						for (int b = -radius; b <= radius; b++)
						{
							for (int a = -radius; a <= radius; a++)
							{
								int y = j + b;
								int x = i + a;

								int mask_y = radius + b;
								int mask_x = radius + a;

								//uchar  tt = (src_data1[y*src.step + x]);// 这样写 便于 调试
								val += ((float)(src_data[y*src.step + x + c]) * mask_data[mask_y*Mask[m].step + mask_x]) / Mask_Sum;
							}
						}
						float t = abs(val - src_data[j*src.step + i + c]);
						if (Min_Pix > t)
						{
							Min_Pix = t;
							m_final = m;
							val_final = val;
						}
					}
					dst_data[j*src.step + i + c] = (uchar)val_final;
			   }
			}
		}
		return dst;
	}
}

int main()
{
	//InitFilter(1);
	//InitFilter_1(1);
	Mat src = imread("2.bmp", 0);

	imshow("src", src);

	Mat result = SideWindowFilter(src, 5);

	imshow("result", result);
	waitKey(0);
	return 0;
}