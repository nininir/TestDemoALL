//���������˲�
//
//https ://blog.csdn.net/sinat_36412790/article/details/80414794
//
//����ϵ����ص�k��ȡֵԽ��Խƽ����Խ���ױ�����Ե��lambdaͬ��Ҳ��ȡֵԽ��Խƽ����

#include <opencv2/opencv.hpp>  
#include <iostream>  
using namespace cv;
using namespace std;


Mat Anisotropic(Mat &src, int k, float lambda)
{
	int iter = 20;
	float ei, si, wi, ni;//������������ �����ĵ� ����ֵ�� ��ֵ
	float ce, cs, cw, cn;//���������Ӧ����ɢϵ��

	Mat dst = src.clone();
	Mat tmp = src.clone();

	uchar *src_data = src.data;
	uchar *dst_data = dst.data;
	uchar *tmp_data = tmp.data;

	for (int n = 0; n < iter; n++)
	{
		for (int j = 1; j < src.rows - 1; j++)
		{
			for (int i = 1; i < src.cols - 1; i++)
			{
				float pix = tmp_data[j*src.step + i];
				si = tmp_data[j*src.step + i + 1] - pix;
				wi = tmp_data[(j + 1)*src.step + i] - pix;
				ni = tmp_data[j*src.step + i - 1] - pix;
				ei = tmp_data[(j - 1)*src.step + i] - pix;

				ce = exp(-ei*ei / (k*k));
				cs = exp(-si*si / (k*k));
				cw = exp(-wi*wi / (k*k));
				cn = exp(-ni*ni / (k*k));

				dst_data[j*dst.step + i] = pix + lambda*(ce*ei + cs*si + cw*wi + cn*ni);
			}
		}
		dst.copyTo(tmp);//�ؼ� һ������֤ ��һ�� �����ͼƬ �� ����һ�ε��� �����ɵ�  
	}
	
	return dst;
}


void Anisotropic_RGB(Mat &src, Mat &dst, int k, float lambda)
{
	vector<Mat> src_split;
	
	vector<Mat> dst_tmp;

	split(src, src_split);
	split(dst, dst_tmp);

	for (int c = 0; c < src_split.size(); c++)
	{
		dst_tmp[c] = Anisotropic(src_split[c], k, lambda);
		/*normalize(dst_tmp[c], dst_tmp[c], 0, 255, NORM_MINMAX);
		dst_tmp[c].convertTo(dst_tmp[c], CV_8UC1);*/
	}

	merge(dst_tmp, dst);
}

int main(int argc, char** argv)
{
	Mat src = imread("1.jpg", 1);//ע�� ��ͼ��ͨ��
	if (src.empty())
	{
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("input image", src);


	float k = 15;
	float lambda = 0.25;

	double t1 = getTickCount();
	//Mat result =Anisotropic(src, k, lambda);//��ͨ��

	Mat result = src.clone();
	Anisotropic_RGB(src, result, k, lambda);//ԭͼ 3ͨ��
	double t2 = getTickCount();

	cout << (t2 - t1) / getTickFrequency() * 1000 << endl;
	imshow("result", result);

	waitKey(0);
	return 0;
}