#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

//给原图像增加椒盐噪声
//图象模拟添加椒盐噪声是通过随机获取像素点,并设置为高亮度点和低亮度点来实现的
//srcImage为源图像，n为椒/盐像素点个数，返回含噪图像
Mat addSaltNoise(const Mat srcImage, int n)
{
	Mat dstImage = srcImage.clone();

	for (int k = 0; k < n; k++)
	{
		//随机取值行列，得到像素点(i,j)
		int i = rand() % dstImage.rows;
		int j = rand() % dstImage.cols;

		//图像通道判定
		if (dstImage.channels() == 1)//修改像素点(i,j)的像素值
		{
			dstImage.at<uchar>(i, j) = 255;     //盐噪声
		}
		else
		{
			dstImage.at<Vec3b>(i, j)[0] = 255;
			dstImage.at<Vec3b>(i, j)[1] = 255;
			dstImage.at<Vec3b>(i, j)[2] = 255;
		}
	}

	for (int k = 0; k < n; k++)
	{
		//随机取值行列
		int i = rand() % dstImage.rows;
		int j = rand() % dstImage.cols;
		//图像通道判定
		if (dstImage.channels() == 1)
		{
			dstImage.at<uchar>(i, j) = 0;       //椒噪声
		}
		else
		{
			dstImage.at<Vec3b>(i, j)[0] = 0;
			dstImage.at<Vec3b>(i, j)[1] = 0;
			dstImage.at<Vec3b>(i, j)[2] = 0;
		}
	}
	return dstImage;
}

int adaptive_process(const Mat &src1,const uchar *s_data, int j, int i ,int minSize, int maxSize)
{
	int Z_min, Z_max, Z_med;
	//逐渐增大 模板尺寸 直至到最大限制尺寸
	for (int R_C_size = minSize / 2; R_C_size <= maxSize / 2; R_C_size++)
	{
		vector<int> model_pix_value;
		for (int m = -R_C_size; m < R_C_size; m++)
		{
			for (int n = -R_C_size; n < R_C_size; n++)
			{
				int x = i + m;
				int y = j + n;

				int pix = s_data[y*src1.step + x];
				model_pix_value.push_back(pix);
			}
		}
		sort(model_pix_value.begin(), model_pix_value.end());

		Z_med = model_pix_value[R_C_size*R_C_size * 2 + 1];
		Z_max = model_pix_value[R_C_size*R_C_size * 4 - 1];
		Z_min = model_pix_value[0];

		int A1, A2;
		A1 = Z_med - Z_min;
		A2 = Z_med - Z_max;

		//满足条件，则选取当前模板大小 所处理后的值
		if (A1 > 0 && A2 < 0)
		{
			break;
		}
	}

	int current_pix_Value;
	current_pix_Value = s_data[j*src1.step + i];

	int B1, B2;
	B1 = current_pix_Value - Z_min;
	B2 = current_pix_Value - Z_max;

	if (B1 > 0 && B2 < 0)
	{
		return current_pix_Value;
	}
	else
		return Z_med;
}

Mat adaptive_Medain_filter(Mat &src, int minSize, int maxSize)
{
	Mat src1 = src.clone();

	copyMakeBorder(src1, src1, maxSize / 2, maxSize / 2, maxSize / 2, maxSize / 2, BorderTypes::BORDER_REFLECT);

	Mat dst = src1.clone();
 
	uchar* s_data = src1.data;
	uchar *d_data = dst.data;
	for (int j = maxSize / 2; j < src1.rows - maxSize / 2; j++)
	{
		for (int i = maxSize / 2; i < src1.cols - maxSize / 2; i++)
		{
			int Processed_value;
			Processed_value = adaptive_process(src1, s_data, j, i, minSize, maxSize);
			d_data[j*dst.step + i] = Processed_value;
		}
	}
	return dst;
}

int main()
{
	Mat src = imread("01.jpg", 0);
	if (!src.data)
	{
		cout << "imread is error!";
		return -1;
	}

	int size;
	size = src.rows*src.cols;
	src = addSaltNoise(src, size*0.4);

	Mat dst;
	adaptive_Medain_filter(src, 3, 7);

	namedWindow("src", 2);
	namedWindow("dst", 2);
	imshow("src", src);
	imshow("dst", dst);

	waitKey(0);
	return 0;
}