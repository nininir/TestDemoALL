#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using namespace std;
using namespace cv;

//���Ļ����ĸ����ֽ���
Mat To_center(Mat &src)
{
	//��Ƶ��ͼ��ü���ż��������Ƶ���ַŵ�ͼ������
	int width = (src.cols / 2) * 2;
	int height = (src.rows / 2) * 2;
	src = src(Rect(0, 0, width, height));

	int colToCut = src.cols / 2;
	int rowToCut = src.rows / 2;

	//��ȡͼ�񣬷ֱ�Ϊ����������������
	//ע�����ַ�ʽ�õ�����magImg��ROI������
	//�������ķ�ͼ������޸ľ���ֱ�Ӷ�magImg�������޸�
	Mat topLeftImg(src, Rect(0, 0, colToCut, rowToCut));
	Mat topRightImg(src, Rect(colToCut, 0, colToCut, rowToCut));
	Mat bottomLeftImg(src, Rect(0, rowToCut, colToCut, rowToCut));
	Mat bottomRightImg(src, Rect(colToCut, rowToCut, colToCut, rowToCut));

	//�ڶ����޺͵������޽��н���
	Mat tmpImg;
	topLeftImg.copyTo(tmpImg);
	bottomRightImg.copyTo(topLeftImg);
	tmpImg.copyTo(bottomRightImg);

	//��һ���޺͵������޽��н���
	bottomLeftImg.copyTo(tmpImg);
	topRightImg.copyTo(bottomLeftImg);
	tmpImg.copyTo(topRightImg);

	return src;
}

//���Ļ�������dftǰ �����ݣ�ͼ��������λ����λ������ �������������� ����
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
	imshow("����ͼ��", src);

	//��1������һ���������󣬴��渵��Ҷ�任��ľ���
	int r = getOptimalDFTSize(src.rows);
	int c = getOptimalDFTSize(src.cols);//�õ����ųߴ�
	Mat padded;
	//����src�ı�Ե����ͼ����( 0, r - src.rows, 0, c - src.cols)�ֱ��ʾ�������ң�
	copyMakeBorder(src, padded, 0, r - src.rows, 0, c - src.cols, BORDER_CONSTANT, Scalar::all(0));
	padded.convertTo(padded, CV_32FC1);

	//�������Ļ�
	Data_TO_center(padded);

	//����һ����������ʵ��Ϊplane[0]���鲿plane[1]���0
	Mat plane[] = { padded,Mat::zeros(padded.size(),CV_32F) };

	//��2������Ҷ�任
	Mat complexImg;
	merge(plane, 2, complexImg); //�������Ϊ��ϳ�2ͨ����ʵ��+�鲿��ͼ��
	dft(complexImg, complexImg); //DFT�任������ݸ��Ƶ�ԭ����û�����⿪���ڴ棬 complexImg�Ǹ���������

 	/***********************Ƶ�����˲�����*************************/
	//��˹��ͨ�˲��������м�����ߵͣ�
	Mat gaussianBlur(complexImg.size(), CV_32FC2);
	//��˹��ͨ�˲��������м�����߸ߣ�
	Mat gaussianSharpen(complexImg.size(), CV_32FC2);

	float * G_b_data = (float *)gaussianBlur.data;
	float * G_s_data = (float *)gaussianSharpen.data;

	double delta = 2 * 10 * 10;
	for (int j = 0; j < complexImg.rows; j++)
	{
		for (int i = 0; i < complexImg.cols; i++)
		{
			//����Ƶ�ʾ���Բ��ľ��룬delta��ʾ��˹���ߵ���ɢ�̶�
			double d = pow(j - complexImg.rows / 2, 2) + pow(i - complexImg.cols / 2, 2);
			G_b_data[j*complexImg.step / sizeof(float) + 2 * i] = expf(-d / delta);
			G_b_data[j*complexImg.step / sizeof(float) + 2 * i + 1] = expf(-d / delta);

			G_s_data[j*complexImg.step / sizeof(float) + 2 * i] = 1 - expf(-d / delta);
			G_s_data[j*complexImg.step / sizeof(float) + 2 * i + 1] =1 - expf(-d / delta);
		}
	}

	/***********************�˲�����������***********************/

	//�˲�����DFT�����ˣ������ڻ���
	multiply(complexImg, gaussianBlur, gaussianBlur);
	multiply(complexImg, gaussianSharpen, gaussianSharpen);

	//��������ף�����Ҷ�ף�
	split(complexImg, plane);
	Mat magImg;
	magnitude(plane[0], plane[1], magImg);//�����άʸ���ķ�ֵ									 
	magImg += Scalar::all(1);
	log(magImg, magImg);  //ת������������
	//��һ��ͼ��
	normalize(magImg, magImg, 0, 1, CV_MINMAX);
	imshow("Ƶ��ͼ��", magImg);

	//����Ƶ��
	Mat Gauss_Blur;
	split(gaussianBlur, plane);
	magnitude(plane[0], plane[1], Gauss_Blur);
	Gauss_Blur += Scalar::all(1);
	log(Gauss_Blur, Gauss_Blur);
	normalize(Gauss_Blur, Gauss_Blur, 1, 0, CV_MINMAX);
	imshow("��˹��ͨ�˲�Ƶ��", Gauss_Blur);

	split(gaussianSharpen, plane);
	magnitude(plane[0], plane[1], plane[0]);
	plane[0] += Scalar::all(1);
	log(plane[0], plane[0]);
	normalize(plane[0], plane[0], 1, 0, CV_MINMAX);
	imshow("��˹��ͨ�˲�Ƶ��", plane[0]);

	//���˲��� ʵ�����鲿 ���Ļ�
	split(complexImg, plane);
	plane[0] = To_center(plane[0]);
	plane[1] = To_center(plane[1]);
	merge(plane, 2, complexImg);
	//����Ҷ���任
	Mat complexIDFT, IDFTImg;
	idft(complexImg, complexIDFT);
	split(complexIDFT, plane);
	magnitude(plane[0], plane[1], IDFTImg);
	normalize(IDFTImg, IDFTImg, 0, 1, CV_MINMAX);

	imshow("���任ͼ��", IDFTImg);

	//IDFT�õ��˲����
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

	imshow("��˹��ͨ�˲�", gaussianBlurImg);
	imshow("��˹��ͨ�˲�", gaussianSharpenImg);

	waitKey(0);
}