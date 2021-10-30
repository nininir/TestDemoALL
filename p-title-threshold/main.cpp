#include<iostream>
#include<opencv2\opencv.hpp>
using namespace cv;
using namespace std;

//算法：
//初始化阈值为T0
//用Ti将全部像素值分为两部分G1和G2，计算两部分的均值分别为m1和m2
//用m1和m2产生新的阈值 Ti = m1 + m22
//迭代上面步骤2和步骤3，直到
//| Ti−Ti−1 | <ΔT


//P-Tile阈值 迭代均值阈值
Mat P_threshold(Mat src,int sigma)
{
	Mat temp = src.clone();
	int hist[256] = {0};
	uchar *temp_data = src.data;
	for (int j = 0; j < src.rows; j++)
	{
		for (int i = 0; i < src.cols; i++)
		{
			int pix_value = temp_data[j*src.step + i];
			hist[pix_value]++;
		}
	}

	int sum = 0;//直方图 像素点 数和
	int med_Num, medValue;
	for (int m = 0; m < 256; m++)
	{
		sum += hist[m];
		if (sum * 2 > src.rows*src.cols)
		{
			med_Num = sum;
			medValue = m;
			break;
		}
	}
	int T0 = medValue;

	vector<int> hist_index;
	int t = 0;
	while (t<256)
	{
		if (hist[t] != 0)
			hist_index.push_back(hist[t]);
		t++;
	}

	//auto thr_max = max_element(begin(hist_index), end(hist_index));
	//auto thr_min = min_element(begin(hist_index), end(hist_index));
	//int T = *thr_max + *thr_min / 2.0;
	
	int a0 = 0;
	int a1 = hist_index.size() - 1;
	//int a01 = (a0 + a1) / 2;
	//int T0 = (hist_index[a0] + hist_index[a1]) / 2;
	int t1 = 50;
	while (t1 >= sigma)
	{
		int m1 = (int)((a0 + T0) / 2);
		int m2 = (int)((a1 + T0) / 2);

		int T1 = (m1 + m2) / 2;
		t1 = T1 - T0;

		T0 = T1;
	} ;

	threshold(src, temp, T0, 255, CV_THRESH_BINARY);
	
	return temp;
}

int main()
{
	Mat src = imread("1.png", 0);
	imshow("src", src);

	P_threshold(src, 255);

	waitKey(0);
	return 0;
}