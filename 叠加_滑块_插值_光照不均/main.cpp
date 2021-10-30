#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

void un_light(Mat &input, int blockSize)
{
	if (input.channels() == 3) cvtColor(input, input, CV_BGR2GRAY);

	double average = mean(input)[0];//表示第一个通道的均值

									//每一块的尺寸大小
	int row_new = ceil(double(input.rows) / double(blockSize)); // ceil()向上取整函数,ceil()是向正无穷大舍入，ceil(-10.5) == -10  ceil(10.5) == 11
	int col_new = ceil(double(input.cols) / double(blockSize));

	Mat blockImage;
	blockImage = Mat::zeros(row_new, col_new, CV_32FC1);

	//分块操作 
	for (int i = 0; i < row_new; i++)
	{
		for (int j = 0; j < col_new; j++)
		{
			//每一块沿着原图进行“移动”，将块内元素均值赋予对应的锚点处
			int rowmin = i*blockSize;
			int rowmax = (i + 1)*blockSize;
			if (rowmax > input.rows)rowmax = input.rows;
			int colmin = j*blockSize;
			int colmax = (j + 1)*blockSize;
			if (colmax > input.cols) colmax = input.cols;

			Mat imageRoi = input(Range(rowmin, rowmax), Range(colmin, colmax));

			double tem_aver = mean(imageRoi)[0];
			blockImage.at<float>(i, j) = tem_aver;

		}
	}

	blockImage = blockImage - average;
	Mat blockImage2;
	resize(blockImage, blockImage2, input.size(), 0, 0, INTER_CUBIC);

	Mat input2;
	input.convertTo(input2, CV_32FC1);
	Mat dst = input2 - blockImage2;
	dst.convertTo(input, CV_8UC1);
	namedWindow("dst", CV_WINDOW_AUTOSIZE);
	imshow("dst", dst);

}

void move_deal(Mat &src)
{
	if (src.channels() == 3) cvtColor(src, src, CV_BGR2GRAY);
	Mat src1 = src.clone();

	float average = mean(src)[0];//表示第一个通道的均值

	vector<vector<float>> array_mean;//所有 像素点对应的像素均值

	uchar *s_data = src.data;

	for (int j = 0; j < src.rows; j++)
	{
		vector<float> all_mean;

		for (int i = 0; i < src.cols; i++)
		{
			int kernel_pix_sum = 0;
			int num_pix = 0;//像素个数
			float kernel_mean = 0.0;
			//5*5 模板 内遍历像素
			for (int n = -12; n < 13; n++)
			{
				for(int m = -12; m < 13; m++)
				{
					int x = i + m;
					int y = j + n;

					//所遍历像素 在原图内部
					if (x >= 0 && x < src.cols && y >= 0 && y < src.rows)
					{
						kernel_pix_sum += s_data[y*src.step + x];
						num_pix++;
					}
				}
			}
			if (num_pix == 0) cout << "kernel has no one!";
			kernel_mean = (float)kernel_pix_sum / num_pix;

			all_mean.push_back(kernel_mean);//存入容器
		}

		array_mean.push_back(all_mean);
	}

	int size_row = array_mean.size();  //获取行数
	int size_col = array_mean[0].size();  //获取列数

	vector<vector<float>> seed;//储存差异点
	for (int j = 15; j < size_row; j = j + 55)
	{
		vector<float> mean_cols_num;
		for (int i = 15; i < size_col; i = i + 55)
		{
			float p = array_mean[j][i] - average;
			mean_cols_num.push_back(p);
		}
		seed.push_back(mean_cols_num);
	}
	
	int seed_row = seed.size();
	int seed_col = seed[0].size();

	Mat difference_mean(Size(seed_col, seed_row), CV_32FC1, Scalar(0.0));
	float *d_data = (float *)difference_mean.data;

	for (int j = 0; j < difference_mean.rows; j++)
	{
		for (int i = 0; i < difference_mean.cols; i++)
		{
			d_data[j*difference_mean.step / sizeof(float) + i] = seed[j][i];
		}
	}

	Mat blockImage;
	resize(difference_mean, blockImage, src.size(), 0, 0, CV_INTER_CUBIC);

	Mat input2;
	src.convertTo(input2, CV_32FC1);

	Mat dst = input2 - blockImage;
	dst.convertTo(dst, CV_8UC1);
	
	namedWindow("dst", 2);
	imshow("dst", dst);
}

int main()
{

	Mat src = imread("3.png", 0);
	if (!src.data)
	{
		cout << "imread is fail!";
		return -1;
	}

	imshow("src", src);

	//move_deal(src);
	un_light(src, 256);

	return 0;
}