#include<iostream>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

int OtsuThreshold(Mat src)
{
	int threshold;

	int height = src.rows; 
	int width = src.cols;

	//histogram  
	float histogram[256] = { 0 };
	uchar * s_data = src.data;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (s_data[i*src.step + j] != 0)//排除直方图中的0像素值 
				histogram[s_data[i*src.step + j]]++;
		}
	}
	
	int m = 0, n = 0; //m:第一个有像素的灰度值 n:最后一个有像素的灰度值
	long sum_value = 0;//非零像素个数
	bool singel = true;
	for (int i = 1; i < 256; i++)
	{
		if (histogram[i] != 0 && singel)
		{
			m = i;
			singel = false;
		}

		if (histogram[i] != 0)
		{
			sum_value++;
		}
	}

	for (int j = 255; j >= 0; j--)
	{
		if (histogram[j] != 0)
		{
			n = j;
			break;
		}
	}

	////normalize histogram  
	//for (int i = 0; i < 256; i++)
	//{
	//	histogram[i] = histogram[i] / sum_value;
	//}

	////average pixel value  
	//float avgValue = 0;
	//for (int i = m; i <= n; i++)
	//{
	//	avgValue += i*histogram[i];
	//}

	//float maxVariance = 0;
	//float w = 0, u = 0;
	//for (int i = m; i <= n; i++)
	//{
	//	if (histogram[i] != 0)
	//	{
	//		float variance, t;
	//		w += histogram[i];
	//		u += i*histogram[i];

	//		t = avgValue*w - u;
	//		
	//		if (w == 1)
	//		{
	//			variance = 0;
	//		}
	//		variance = t*t / (w*(1 - w));
	//		if (variance > maxVariance)
	//		{
	//			maxVariance = variance;
	//			threshold = i;
	//		}
	//	}
	//}

	float  sum0, sum1, cnt0, cnt1, w0, w1, u0, u1, u, variance;
	float maxVariance = 0;
	for (int i = m+1; i <= n; i++)
	{
		 sum0 = 0;
	     sum1 = 0;
		 cnt0 = 0;
		 cnt1 = 0;
		 w0 = 0;
		 w1 = 0;

		 if (histogram[i] != 0)
		 {
			 for (int j = m; j < i; j++)
			 {
				 cnt0 += histogram[j];
				 sum0 += j * histogram[j];
			 }

			 u0 = (double)sum0 / cnt0;
			 w0 = (double)cnt0 / sum_value;

			 for (int j = i; j <= n; j++) 
			 {
				 cnt1 += histogram[j];
				 sum1 += j * histogram[j];
			 }

			 u1 = (double)sum1 / cnt1;
			 w1 = (double)cnt1 / sum_value;

			 u = u0 * w0 + u1 * w1; //图像的平均灰度
		
			 variance = w0 * w1 *  (u0 - u1) * (u0 - u1);
			 if (variance > maxVariance)
			 {
				 maxVariance = variance;
				 threshold = i;
			 }
		 }
	}

	return threshold;
}

void edge_threshold(Mat &src)
{
	Mat src1 = src.clone();
	Mat dst, thre, dst_x, dst_y;

	medianBlur(src1, src1, 3);

	Mat kernel_x = (Mat_<int>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
	Mat kernel_y = (Mat_<int>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);

	filter2D(src1, dst_x, CV_16SC1, kernel_x);
	filter2D(src1, dst_y, CV_16SC1, kernel_y);

	convertScaleAbs(dst_x, dst_x);
	convertScaleAbs(dst_y, dst_y);

	dst = dst_x + dst_y;

	/*Laplacian(src1, dst, CV_16SC1);
	convertScaleAbs(dst, dst);*/

	//只取拉普拉斯计算后 正数
	//signed char * d_data = (signed char *)dst.data;
	//for (int j = 0; j < dst.rows; j++)
	//{
	//	for (int i = 0; i < dst.cols; i++)
	//	{
	//		int pix = d_data[j*dst.step / sizeof(signed char) + i];
	//		if (pix < 0)
	//		{
	//			d_data[j*dst.step / sizeof(signed char) + i] = 0;
	//		}
	//	}
	//}
	//dst.convertTo(dst, CV_8UC1);

	int hist[256] = { 0 };
	uchar * d_data = dst.data;
	for (int j = 0; j < dst.rows; j++)
	{
		for (int i = 0; i < dst.cols; i++)
		{
			if (d_data[j*dst.step + i] != 0)
			{
				hist[d_data[j*dst.step + i]]++;	
			}
		}
	}
	long sum = 0, Amount = 0;
	double percent;
		
	int T = 0;
	for (int k = 0; k < 256; k++)
	{
		Amount += hist[k];
	}

	for (int i = 0; i < 256; i++)
	{
		sum += hist[i];
		if (sum >= Amount*0.9)
		{
			T = i;
			break;
		}
	}

	threshold(dst, thre, T, 1, THRESH_BINARY);
	Mat dd;
	dd = thre * 255;

	Mat src2;
	src2 = thre.mul(src1);//取thre 中的像素值为1的像素点。

	Mat src3;
	
	threshold(src1, src3, 120, 255, THRESH_BINARY);
	imshow("result", src3);
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
	edge_threshold(src);


	waitKey(0);
	return 0;
}