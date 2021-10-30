#include<iostream>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

Mat Local_corrention(Mat &src)
{
	Mat temp = src.clone();
	vector<Mat> src_rgb;
	split(src, src_rgb);

	Mat I;
	I = (src_rgb[0] + src_rgb[1] + src_rgb[2]) / 3.0;

	Mat I0 = 255 - I;

	Mat M;
	GaussianBlur(I0, M, Size(77, 77), BORDER_DEFAULT);

	Mat output(src.size(), CV_8UC3);
	for(int c = 0; c < 3; c++)
	{
		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				uchar t = M.at<uchar>(j, i);
				uchar s = src_rgb[c].at<uchar>(j, i);
				float EXP = pow(2, ((128 - t) / 128.0));
				output.at<Vec3b>(j, i)[c] = 255 * pow((s / 255.0), EXP);
			}
		}
	}

	return output;
}
int main()
{
	Mat src = imread("2.png");
	if (!src.data)
	{
		cout << "read image error";
		return -1;
	}

	imshow("src", src);

	Mat result = Local_corrention(src);
	waitKey(0);
	return 0;
}