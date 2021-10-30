#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

//��ԭͼ�����ӽ�������
//ͼ��ģ����ӽ���������ͨ�������ȡ���ص�,������Ϊ�����ȵ�͵����ȵ���ʵ�ֵ�
//srcImageΪԴͼ��nΪ��/�����ص���������غ���ͼ��
Mat addSaltNoise(const Mat srcImage, int n)
{
	Mat dstImage = srcImage.clone();

	for (int k = 0; k < n; k++)
	{
		//���ȡֵ���У��õ����ص�(i,j)
		int i = rand() % dstImage.rows;
		int j = rand() % dstImage.cols;

		//ͼ��ͨ���ж�
		if (dstImage.channels() == 1)//�޸����ص�(i,j)������ֵ
		{
			dstImage.at<uchar>(i, j) = 255;     //������
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
		//���ȡֵ����
		int i = rand() % dstImage.rows;
		int j = rand() % dstImage.cols;
		//ͼ��ͨ���ж�
		if (dstImage.channels() == 1)
		{
			dstImage.at<uchar>(i, j) = 0;       //������
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
	//������ ģ��ߴ� ֱ����������Ƴߴ�
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

		//������������ѡȡ��ǰģ���С ��������ֵ
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