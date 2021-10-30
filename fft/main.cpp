#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace std;
using namespace cv;

//中心化：四个部分交换
Mat To_center(Mat &src)
{
	//将频谱图像裁剪成偶数并将低频部分放到图像中心
	int width = (src.cols / 2) * 2;
	int height = (src.rows / 2) * 2;
	src = src(Rect(0, 0, width, height));

	int colToCut = src.cols / 2;
	int rowToCut = src.rows / 2;

	//获取图像，分别为左上右上左下右下
	//注意这种方式得到的是magImg的ROI的引用
	//对下面四幅图像进行修改就是直接对magImg进行了修改
	Mat topLeftImg(src, Rect(0, 0, colToCut, rowToCut));
	Mat topRightImg(src, Rect(colToCut, 0, colToCut, rowToCut));
	Mat bottomLeftImg(src, Rect(0, rowToCut, colToCut, rowToCut));
	Mat bottomRightImg(src, Rect(colToCut, rowToCut, colToCut, rowToCut));

	//第二象限和第四象限进行交换
	Mat tmpImg;
	topLeftImg.copyTo(tmpImg);
	bottomRightImg.copyTo(topLeftImg);
	tmpImg.copyTo(bottomRightImg);

	//第一象限和第三象限进行交换
	bottomLeftImg.copyTo(tmpImg);
	topRightImg.copyTo(bottomLeftImg);
	tmpImg.copyTo(topRightImg);

	return src;
}

//中心化：计算dft前 将数据（图像）阵列移位；移位后数据 包含完整的数据 周期
void Data_TO_center(Mat &src)
{
	int w = src.rows;
	int h = src.cols;

	float *src_data = (float*)src.data;

	for (int j = 0; j < w; j++)
	{
		for (int i = 0; i < h; i++)
		{
			src_data[j*src.step / sizeof(float) + i] *= pow(-1, i + j);
		}
	}
}

int main()
{
	string path = "1.bmp";
	Mat src = imread(path, 0);
	imshow("输入图像", src);

	//【1】创建一个复数矩阵，储存傅里叶变换后的矩阵
	int r = getOptimalDFTSize(src.rows);
	int c = getOptimalDFTSize(src.cols);//得到最优尺寸
	Mat padded;
	//扩充src的边缘，将图像变大( 0, r - src.rows, 0, c - src.cols)分别表示上下左右；
	copyMakeBorder(src, padded, 0, r - src.rows, 0, c - src.cols, BORDER_CONSTANT, Scalar::all(0));
	padded.convertTo(padded, CV_32FC1);

	//数据中心化
	Data_TO_center(padded);

	//创建一个复数矩阵，实部为plane[0]，虚部plane[1]填充0
	Mat plane[] = { padded,Mat::zeros(padded.size(),CV_32F) };

	//【2】傅里叶变换
	Mat complexImg;
	merge(plane, 2, complexImg); //可以理解为组合成2通道（实部+虚部）图像
	dft(complexImg, complexImg); //DFT变换后的数据复制到原处，没有另外开辟内存， complexImg是个复数矩阵

 	/***********************频率域滤波部分*************************/
	//高斯低通滤波函数（中间高两边低）
	Mat gaussianBlur(complexImg.size(), CV_32FC2);
	//高斯高通滤波函数（中间低两边高）
	Mat gaussianSharpen(complexImg.size(), CV_32FC2);

	float * G_b_data = (float *)gaussianBlur.data;
	float * G_s_data = (float *)gaussianSharpen.data;

	double delta = 2 * 10 * 10;
	for (int j = 0; j < complexImg.rows; j++)
	{
		for (int i = 0; i < complexImg.cols; i++)
		{
			//截至频率距离圆点的距离，delta表示高斯曲线的扩散程度
			double d = pow(j - complexImg.rows / 2, 2) + pow(i - complexImg.cols / 2, 2);
			G_b_data[j*complexImg.step / sizeof(float) + 2 * i] = expf(-d / delta);
			G_b_data[j*complexImg.step / sizeof(float) + 2 * i + 1] = expf(-d / delta);

			G_s_data[j*complexImg.step / sizeof(float) + 2 * i] = 1 - expf(-d / delta);
			G_s_data[j*complexImg.step / sizeof(float) + 2 * i + 1] =1 - expf(-d / delta);
		}
	}

	/***********************滤波器构建结束***********************/

	//滤波器和DFT结果相乘（矩阵内积）
	multiply(complexImg, gaussianBlur, gaussianBlur);
	multiply(complexImg, gaussianSharpen, gaussianSharpen);

	//计算幅度谱（傅里叶谱）
	split(complexImg, plane);
	Mat magImg;
	magnitude(plane[0], plane[1], magImg);//计算二维矢量的幅值									 
	magImg += Scalar::all(1);
	log(magImg, magImg);  //转换到对数坐标
	//归一化图像
	normalize(magImg, magImg, 0, 1, CV_MINMAX);
	imshow("频谱图像", magImg);

	//计算频谱
	Mat Gauss_Blur;
	split(gaussianBlur, plane);
	magnitude(plane[0], plane[1], Gauss_Blur);
	Gauss_Blur += Scalar::all(1);
	log(Gauss_Blur, Gauss_Blur);
	normalize(Gauss_Blur, Gauss_Blur, 1, 0, CV_MINMAX);
	imshow("高斯低通滤波频谱", Gauss_Blur);

	split(gaussianSharpen, plane);
	magnitude(plane[0], plane[1], plane[0]);
	plane[0] += Scalar::all(1);
	log(plane[0], plane[0]);
	normalize(plane[0], plane[0], 1, 0, CV_MINMAX);
	imshow("高斯高通滤波频谱", plane[0]);

	//将滤波器 实部和虚部 中心化
	split(complexImg, plane);
	plane[0] = To_center(plane[0]);
	plane[1] = To_center(plane[1]);
	merge(plane, 2, complexImg);
	//傅里叶反变换
	Mat complexIDFT, IDFTImg;
	idft(complexImg, complexIDFT);
	split(complexIDFT, plane);
	magnitude(plane[0], plane[1], IDFTImg);
	normalize(IDFTImg, IDFTImg, 0, 1, CV_MINMAX);

	imshow("反变换图像", IDFTImg);

	//IDFT得到滤波结果
	Mat gaussianBlurImg;
	idft(gaussianBlur, complexIDFT);
	split(complexIDFT, plane);
	magnitude(plane[0], plane[1], gaussianBlurImg);
	normalize(gaussianBlurImg, gaussianBlurImg, 0, 1, CV_MINMAX);

	Mat gaussianSharpenImg;
	idft(gaussianSharpen, complexIDFT);
	split(complexIDFT, plane);
	magnitude(plane[0], plane[1], gaussianSharpenImg);
	normalize(gaussianSharpenImg, gaussianSharpenImg, 0, 1, CV_MINMAX);

	imshow("高斯低通滤波", gaussianBlurImg);
	imshow("高斯高通滤波", gaussianSharpenImg);

	waitKey(0);
}