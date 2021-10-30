#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;


void un_light(Mat &input, int blockSize)
{
	if (input.channels() == 3) cvtColor(input, input, CV_BGR2GRAY);

	double average = mean(input)[0];//��ʾ��һ��ͨ���ľ�ֵ

									//ÿһ��ĳߴ��С
	int row_new = ceil(double(input.rows) / double(blockSize)); // ceil()����ȡ������,ceil()��������������룬ceil(-10.5) == -10  ceil(10.5) == 11
	int col_new = ceil(double(input.cols) / double(blockSize));

	Mat blockImage;
	blockImage = Mat::zeros(row_new, col_new, CV_32FC1);

	//�ֿ���� 
	for (int i = 0; i < row_new; i++)
	{
		for (int j = 0; j < col_new; j++)
		{
			//ÿһ������ԭͼ���С��ƶ�����������Ԫ�ؾ�ֵ�����Ӧ��ê�㴦
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
	resize(blockImage, blockImage2, input.size(), (0, 0), (0, 0), INTER_CUBIC);

	Mat input2;
	input.convertTo(input2, CV_32FC1);
	Mat dst = input2 - blockImage2;
	dst.convertTo(input, CV_8UC1);
	namedWindow("dst", CV_WINDOW_AUTOSIZE);
	imshow("dst", dst);

}

//��ñ����
void Top_hut(Mat &input)
{
	if (input.channels() == 3) cvtColor(input, input, CV_BGR2GRAY);

	Mat bHat;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(55, 55));
	morphologyEx(input, bHat, MORPH_BLACKHAT, element);
	imshow("black_hat", bHat);
	imwrite("bHat.jpg", bHat);

	Mat bDst;
	threshold(bHat, bDst, 80, 255, THRESH_OTSU | THRESH_BINARY);
	imshow("result", bDst);
}

//��ʴ 
void method_2(Mat &input)
{
	if (input.channels() == 3) cvtColor(input, input, CV_BGR2GRAY);

	Mat element = getStructuringElement(MORPH_RECT, Size(55, 55));
	Mat blackimage;
	morphologyEx(input, blackimage, MORPH_ERODE, element);
	imshow("blackImage", blackimage);

	Mat diss_black;
	diss_black = input - blackimage;
	imshow("diss", diss_black);

	Mat dst;
	threshold(diss_black, dst, 80, 255, THRESH_OTSU | THRESH_BINARY);
	imshow("result", dst);
	imwrite("mat.bmp", dst);

}

void RotateSmooth4Line(Mat &input)
{
	if (input.channels() == 3) cvtColor(input, input, CV_BGR2GRAY);
	Mat dst = Mat(input.size(), CV_8UC1);
	uchar *data_input = input.data;
	uchar *data_dst = dst.data;

	int Mask[8][7][2] =
	{
		{ { -1, 2 },{ 0, 2 },{ 1, 2 },{ 0, 1 },{ 0, 0 },{ 0, -1 },{ 0, -2 } },
		{ { 1, 2 },{ 2, 2 },{ 2, 1 },{ 1, 1 },{ 0, 0 },{ -1, -1 },{ -2, -2 } },
		{ { 2, 1 },{ 2, 0 },{ 2, -1 },{ 1, 0 },{ 0, 0 },{ -1, 0 },{ -2, 0 } },
		{ { 2, -1 },{ 2, -2 },{ 1, -2 },{ 1, -1 },{ 0, 0 },{ -1, 1 },{ -2, 2 } },
		{ { 1, -2 },{ 0, -2 },{ -1, -2 },{ 0, -1 },{ 0, 0 },{ 0, 1 },{ 0, 2 } },
		{ { -1, -2 },{ -2, -2 },{ -2, -1 },{ -1, -1 },{ 0, 0 },{ 1, 1 },{ 2, 2 } },
		{ { -2, -1 },{ -2, 0 },{ -2, 1 },{ -1, 0 },{ 0, 0 },{ 1, 0 },{ 2, 0 } },
		{ { -2, 1 },{ -2, 2 },{ -1, 2 },{ -1, 1 },{ 0, 0 },{ 1, -1 },{ 2, -2 } }
	};


	for (int i = 0; i < input.rows; i++)
	{
		for (int j = 0; j < input.cols; j++)
		{
			int Mask_index_final, count_final;
			int Min_Delta_Square = 1000000000;
			// m , n ����ģ����ת
			for (int m = 0; m < 9; m++)
			{
				int MaskPiontCount = 0;// ģ������Ч���ص����� n
				int MaskSum = 0;
				int Mask_Square_Sum = 0;
				int Delta_Square = 0;
				if (m != 8)
				{
					for (int n = 0; n < 7; n++)
					{
						int x = j + Mask[m][n][0];
						int y = i + Mask[m][n][1];
						//����������ĵ���ͼ���ڲ���MaskPointCount��һ
						if (x >= 0 && x < input.cols && y >= 0 && y < input.rows)
						{ 
							//��Ч������ֵ �ܺ�
							MaskSum += data_input[y*input.step + x];
							//��Ч������ֵ ƽ����
							Mask_Square_Sum += data_input[y*input.step + x] * data_input[y*input.step + x];

							//��Ч������
							MaskPiontCount++;
						}
					}
					//������������������ɢ��ֵ
					Delta_Square = (Mask_Square_Sum - MaskSum*MaskSum / MaskPiontCount) / MaskPiontCount;
				}
				else
				{
					for (int b = -1; b <= 1; b++)
					{
						for (int a = -1; a <= 1; a++)
						{
							int y = i + b;
							int x = j + a;
							//����������ĵ���ͼ���ڲ���MaskPointCount��һ
							if (x >= 0 && x < input.cols && y < input.rows)
							{
								//��Ч������ֵ �ܺ�
								MaskSum += data_input[y*input.step + x];
								//��Ч������ֵ ƽ����
								Mask_Square_Sum += data_input[y*input.step + x] * data_input[y*input.step + x];

								//��Ч������
								MaskPiontCount++;
							}
						}
					}
					//������������������ɢ��ֵ
					Delta_Square = (Mask_Square_Sum - MaskSum*MaskSum / MaskPiontCount) / MaskPiontCount;
				}
				if (Min_Delta_Square > Delta_Square)
				{
					Min_Delta_Square = Delta_Square;
					Mask_index_final = m;
					count_final = MaskPiontCount;
				}
			}
			//��ת����

			//��������ص�����ƽ��ֵ
			int dst_value = 0;
			if (Mask_index_final != 8)
			{
				for (int n = 0; n < 7; n++)
				{
					int y = i + Mask[Mask_index_final][n][1];
					int x = j + Mask[Mask_index_final][n][0];

					if (x >= 0 && x < input.cols && y >= 0 && y < input.rows)
					{

					}

				}
			}
		}
	}

}

int main()
{
	Mat src, dst;
	src = imread("2.png",0);

	if (!src.data)
	{
		printf("could not load image..\n");
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
 	imshow("input", src);


	/*un_light(src, 32);*/
	//double t1 = getTickCount();

	 Top_hut(src);
	 //method_2(src);

	//RotateSmooth4Line(src);

	//double t2 = cv::getTickCount();
	//double t3 = cv::getTickCount();
	//t3 = (t2 - t1) / cv::getTickFrequency() * 1000;
	//cout << t3 << endl;
	waitKey(0);
	return 0;
}