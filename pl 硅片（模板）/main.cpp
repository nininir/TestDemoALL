// 402.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <iostream>
#include <io.h>   

#include<opencv.hpp>
#include<core.hpp>
#include<world.hpp>
#include<opencv2/imgproc/types_c.h>
#include<opencv2/imgproc/imgproc_c.h>
#include<opencv2/highgui/highgui_c.h>
using namespace cv;
using namespace std;


bool change_mat_bright(Mat mat_src, Mat &mat_dst, int Min_Bright, int Max_Bright)
{
	if (mat_src.rows == 0 || mat_src.channels() != 1)
	{
		mat_dst = mat_src.clone();
		return false;
	}

	uchar*data_src = mat_src.data;
	if (Min_Bright < 0)
		Min_Bright = 0;
	if (Min_Bright > 50)
		Min_Bright = 50;

	if (Max_Bright < 40)
		Max_Bright = 40;
	if (Max_Bright > 255)
		Max_Bright = 255;

	float k_Bright = 255.f / (float)(Max_Bright - Min_Bright);

	Mat mat_temp = Mat(mat_src.cols, mat_src.rows, CV_8UC1);
	mat_temp = 0;
	uchar*data_temp = mat_temp.data;

	for (int j = 0; j < mat_src.rows; j++)
	{
		for (int i = 0; i < mat_src.cols; i++)
		{
			int src_val = data_src[j*mat_src.step + i];
			if (src_val < Min_Bright)
				src_val = Min_Bright;

			if (src_val > Max_Bright)
				src_val = Max_Bright;

			int temp_val = (src_val - Min_Bright)* k_Bright + Min_Bright;

			if (temp_val > 255)
				temp_val = 255;

			data_temp[j*mat_src.step + i] = temp_val;
		}
	}

	mat_dst = mat_temp.clone();
	return true;
}

//�������Ļ�
void fft2shift(IplImage *src, IplImage *dst)
{
	IplImage *image_Re = 0, *image_Im = 0;
	int nRow, nCol, i, j, cy, cx;
	double scale, shift, tmp13, tmp24;
	image_Re = cvCreateImage(CvSize(src->width, src->height), IPL_DEPTH_64F, 1);
	//Imaginary part  
	image_Im = cvCreateImage(CvSize(src->width, src->height), IPL_DEPTH_64F, 1);
	cvSplit(src, image_Re, image_Im, 0, 0);
	//����ԭ���������˹����ͼ����p123  
	// Compute the magnitude of the spectrum Mag = sqrt(Re^2 + Im^2)  
	//���㸵��Ҷ��  
	cvPow(image_Re, image_Re, 2.0);
	cvPow(image_Im, image_Im, 2.0);
	cvAdd(image_Re, image_Im, image_Re);
	cvPow(image_Re, image_Re, 0.5);
	//�����任����ǿ�Ҷȼ�ϸ��(���ֱ任ʹ��խ���ͻҶ�����ͼ��ֵӳ��  
	//һ������ֵ������ɼ�������˹����ͼ����p62)  
	// Compute log(1 + Mag);  
	cvAddS(image_Re, cvScalar(1.0), image_Re); // 1 + Mag  
	cvLog(image_Re, image_Re); // log(1 + Mag)  

							   //Rearrange the quadrants of Fourier image so that the origin is at the image center  
	nRow = src->height;
	nCol = src->width;
	cy = nRow / 2; // image center  
	cx = nCol / 2;
	//CV_IMAGE_ELEMΪOpenCV����ĺ꣬������ȡͼ�������ֵ����һ���־��ǽ������ı任  
	for (j = 0; j < cy; j++) {
		for (i = 0; i < cx; i++) {
			//���Ļ���������ݳ��Ŀ���жԽǽ���  
			tmp13 = CV_IMAGE_ELEM(image_Re, double, j, i);
			CV_IMAGE_ELEM(image_Re, double, j, i) = CV_IMAGE_ELEM(
				image_Re, double, j + cy, i + cx);
			CV_IMAGE_ELEM(image_Re, double, j + cy, i + cx) = tmp13;

			tmp24 = CV_IMAGE_ELEM(image_Re, double, j, i + cx);
			CV_IMAGE_ELEM(image_Re, double, j, i + cx) =
				CV_IMAGE_ELEM(image_Re, double, j + cy, i);
			CV_IMAGE_ELEM(image_Re, double, j + cy, i) = tmp24;
		}
	}
	//��һ�����������Ԫ��ֵ��һΪ[0,255]  
	//[(f(x,y)-minVal)/(maxVal-minVal)]*255  
	double minVal = 0, maxVal = 0;
	// Localize minimum and maximum values  
	cvMinMaxLoc(image_Re, &minVal, &maxVal);
	// Normalize image (0 - 255) to be observed as an u8 image  
	scale = 255 / (maxVal - minVal);
	shift = -minVal * scale;
	cvConvertScale(image_Re, dst, scale, shift);
	cvReleaseImage(&image_Re);
	cvReleaseImage(&image_Im);
}


//����Ҷ�任 ������դ��
Mat FourierSmooth(Mat mat_src, int half_gap_width, int half_mu_width)
{
	IplImage *src, *dst, *Invdst, *temp, *Im = 0, *Re, *FourierImage;

	temp = &IplImage(mat_src);
	//��ʾԭͼ��  
	//cvNamedWindow("���任ͼ��", 1);
	//cvShowImage("���任ͼ��", temp);
	dst = cvCreateImage(CvSize(temp->width, temp->height), temp->depth, 1);     //����Ҷ�任ʵ�� ͼ��
	FourierImage = cvCreateImage(CvSize(temp->width, temp->height), temp->depth, 1);   //����Ҷ�任���Ļ���ͼ��
	Invdst = cvCreateImage(CvSize(temp->width, temp->height), temp->depth, 1);         //����Ҷ���任ͼ��
	src = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 2);          //���任��2ͨ��ͼ��
	Im = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 1);
	Re = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 1);

	//����cvConcertScaleת����������  
	cvConvertScale(temp, Re);
	//����ͨ���ĻҶ�ֵ��Ϊ0  
	cvZero(Im);
	//����Ҷ�任�ڸ�������opencv�б�ʾΪ��ͨ��  

	cvMerge(Re, Im, NULL, NULL, src);
	IplImage *Fourier = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 2);
	IplImage *InvFourier = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 2);

	//����Ҷ�任  
	cvDFT(src, Fourier, CV_DXT_FORWARD);
	//cvDFT(Fourier, InvFourier, CV_DXT_INV_SCALE);
	cvSplit(Fourier, Re, Im, NULL, NULL);
	//����cvConcertScaleת����������  
	//cvConvertScale(Re, dst);
	cvNamedWindow("Im");
	cvShowImage("Im", Im);

	//��ʾ����Ҷ�任ͼ��  
	cvNamedWindow("Re", 1);
	cvShowImage("Re", Re);

	//���Ļ�  
	fft2shift(Fourier, FourierImage);
	cvNamedWindow("���Ļ�����Ҷ�任ͼ��", 1);
	cvShowImage("���Ļ�����Ҷ�任ͼ��", FourierImage);

	//cvSplit(InvFourier, Re, Im, NULL, NULL);
	//����cvConcertScaleת����������  
	//cvConvertScale(Re, Invdst);

	//��ʾ����Ҷ��任ͼ��  
	cvNamedWindow("����Ҷ��任ͼ��", 1);
	cvShowImage("����Ҷ��任ͼ��", Invdst);




	//�����˲���  ����ֵ��Ϊ1���м���������Ϊ0
	IplImage* mu = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 1);
	cvSet(mu, cvScalar(1.0f));
	double* data = (double*)mu->imageData;

	//�����ཨ����ɫ��״ģ�壬���Ļ�֮�󼴱任���м�
	/*for (int j = half_gap_width; j < temp->width - half_gap_width; j++)
	{
	data[0 * (temp->width) + j] = 0;
	data[1 * (temp->width) + j] = 0;
	data[(temp->height - 1)*(temp->width) + j] = 0;
	data[(temp->height - 2)*(temp->width) + j] = 0;
	}*/

	//�����ཨ����ɫ��״ģ�壬���Ļ�֮�󼴱任���м�
	for (int i = half_gap_width; i < temp->height - half_gap_width; i++)
	{
		for (int k = 0; k < half_mu_width; k++)
		{
			data[i * (temp->width) + k] = 0;
			data[i * (temp->width) + (temp->height - k - 1)] = 0;
		}
	}

	cvNamedWindow("mu");
	cvShowImage("mu", mu);

	IplImage *Im1 = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 1);
	IplImage *Re1 = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 1);
	IplImage *Fourier1 = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 2);
	IplImage *InvFourier1 = cvCreateImage(CvSize(temp->width, temp->height), IPL_DEPTH_64F, 2);
	IplImage *Invdst1 = cvCreateImage(CvSize(temp->width, temp->height), temp->depth, 1);

	cvSplit(Fourier, Re, Im, NULL, NULL);//Re��ʵ��    Im���鲿
	//cvConvertScale(Re, Re1_temp);
	//cvConvertScale(Im, Im1_temp);

	cvMul(mu, Re, Re1);           //����������
	cvMul(mu, Im, Im1);

	//cvConvertScale(Re1_temp, Re1,255*255);
	//cvConvertScale(Im1_temp, Im1,255*255);
	cvNamedWindow("Re1");
	cvShowImage("Re1", Re1);
	cvNamedWindow("Im1");
	cvShowImage("Im1", Im1);
	cvMerge(Re1, Im1, NULL, NULL, Fourier1);
	fft2shift(Fourier1, FourierImage);
	cvNamedWindow("���Ļ�����Ҷ�任ͼ��1", 1);
	cvShowImage("���Ļ�����Ҷ�任ͼ��1", FourierImage);


	cvDFT(Fourier1, InvFourier1, CV_DXT_INVERSE_SCALE);
	cvSplit(InvFourier1, Re1, Im1, NULL, NULL);
	cvConvertScale(Re1, Invdst1);

	//��ʾ����Ҷ��任ͼ��  
	cvNamedWindow("����Ҷ��任ͼ��1", 1);
	cvShowImage("����Ҷ��任ͼ��1", Invdst1);
	//cvSaveImage("D:\\fourier3.bmp", Invdst1);

	//opencv 2.4.11 IplImage->Mat ���
	//Mat mat_dst = Mat(Invdst1, 1);

	//opencv 3.4.3 IplImage->Mat ���
	Mat mat_dst;
	mat_dst = cvarrToMat(Invdst1, true);
	//Mat mat_dst1 = mat_dst.clone();

	//cvWaitKey(0);
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&Invdst);
	//cvReleaseImage(&temp);
	cvReleaseImage(&Re);
	cvReleaseImage(&Im);
	cvReleaseImage(&FourierImage);
	cvReleaseImage(&Fourier);
	cvReleaseImage(&InvFourier);
	cvReleaseImage(&mu);
	cvReleaseImage(&Re1);
	cvReleaseImage(&Im1);
	cvReleaseImage(&Fourier1);
	cvReleaseImage(&InvFourier1);
	cvReleaseImage(&Invdst1);
	return mat_dst;
}

void adaptiveThreshold2(InputArray _src, OutputArray _dst, double maxValue,
	int method, int type, int blockSize, double delta)
{
	Mat src = _src.getMat();
	CV_Assert(src.type() == CV_8UC1);
	CV_Assert(blockSize % 2 == 1 && blockSize > 1);
	Size size = src.size();

	_dst.create(size, src.type());
	Mat dst = _dst.getMat();

	if (maxValue < 0)
	{
		dst = Scalar(0);
		return;
	}

	Mat mean;

	if (src.data != dst.data)
		mean = dst;

	if (method == ADAPTIVE_THRESH_MEAN_C)
		boxFilter(src, mean, src.type(), Size(blockSize, blockSize),
			Point(-1, -1), true, BORDER_REPLICATE);
	else if (method == ADAPTIVE_THRESH_GAUSSIAN_C)
		GaussianBlur(src, mean, Size(blockSize, blockSize), 0, 0, BORDER_REPLICATE);
	else
		CV_Error(CV_StsBadFlag, "Unknown/unsupported adaptive threshold method");

	int i, j;
	uchar imaxval = saturate_cast<uchar>(maxValue);
	int idelta = type == THRESH_BINARY ? cvCeil(delta) : cvFloor(delta);
	uchar tab[768];

	if (type == CV_THRESH_BINARY)
		for (i = 0; i < 768; i++)
			tab[i] = (uchar)(i - 255 > -idelta ? imaxval : 0);
	else if (type == CV_THRESH_BINARY_INV)
		for (i = 0; i < 768; i++)
			tab[i] = (uchar)(i - 255 <= -idelta ? imaxval : 0);
	else
		CV_Error(CV_StsBadFlag, "Unknown/unsupported threshold type");

	if (src.isContinuous() && mean.isContinuous() && dst.isContinuous())
	{
		size.width *= size.height;
		size.height = 1;
	}

	for (i = 0; i < size.height; i++)
	{
		const uchar* sdata = src.data + src.step*i;
		const uchar* mdata = mean.data + mean.step*i;
		uchar* ddata = dst.data + dst.step*i;

		for (j = 0; j < size.width; j++)
			ddata[j] = tab[sdata[j] - mdata[j] + 255];
	}
}

int main()
{
	Mat mat0 = imread("1.bmp", CV_LOAD_IMAGE_UNCHANGED);
	if (mat0.data==0)
	{
		cout << "error readimage" << endl;
		return -1;
	}

	uchar * data_mat0 = mat0.data;

	//Mat mat1 = Mat(mat0.rows, mat0.cols, CV_8UC1);
	//uchar * data_mat1 = mat1.data;
	//mat1 = 0;
	//for (int j = 0; j < mat0.rows; j++)
	//{
	//	for (int i = 0; i < mat0.cols; i++)
	//	{

	//		int left8 = data_mat0[j*mat0.step + i * 2];
	//		int right8 = data_mat0[j*mat0.step + i * 2 + 1];
	//		data_mat1[j*mat1.step + i] = data_mat0[j*mat0.step + i * 2 + 1];
	//	}
	//}


	//imshow("mat0", mat0);
	//imshow("mat1", mat1);

	/*Mat mat2 = Mat(mat0.rows, mat0.cols, CV_8UC1);

	change_mat_bright(mat1, mat2, 0, 40);
	imshow("mat2", mat2);*/

	Mat mat3 = FourierSmooth(mat0, 40, 15);
	imshow("mat3", mat3);
	/*imwrite("mat1.bmp", mat1);
	imwrite("mat3.bmp", mat3);*/

	Mat mat4 = Mat(mat0.rows, mat0.cols, CV_8UC1);
	adaptiveThreshold2(mat3, mat4, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 38 * 2 + 3, double((143 - 128.f) / 10.f));
	imshow("mat4", mat4);
	//imwrite("mat4.bmp", mat4);

	cv::waitKey();
	std::cout << "Hello World!\n";
	return 0;
}

// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ������ʾ: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
