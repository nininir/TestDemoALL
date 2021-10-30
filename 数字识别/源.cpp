#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


//��λ������������
void f3(cv::Mat &image, cv::Mat &dst) {
	cv::Mat result;
	image.copyTo(result);
	cv::medianBlur(image, image, 3);//ȥ��
	cv::Mat erodeImage;
	cv::erode(image, erodeImage, cv::Mat(), cv::Point(-1, -1), 2);//��ʴ����
	cv::threshold(erodeImage, erodeImage, 0, 255, cv::THRESH_OTSU);//��ֵ��
	erodeImage = 255 - erodeImage;
	std::vector<std::vector<cv::Point>>contours;
	cv::findContours(erodeImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);//��������
	std::vector<cv::Point>number1;//��¼������������������ź�����Y���������
	cv::Mat drawImage0(image.size(), CV_8UC1, cv::Scalar(0));//���ڱ�ǣ��������������������ģ���ͼ��
	uchar*data_drawImage0 = drawImage0.data;
	for (int i = 0; i < contours.size(); i++) {//ɸѡ����
		cv::Rect r0 = cv::boundingRect(contours[i]);
		int d = cv::contourArea(contours[i]);
		int a = r0.width*r0.height;
		double b = r0.width / (double)r0.height;
		if (a >= 2000 && a <= 5000 && abs(b - 1) < 0.5) {

			cv::Moments mom = cv::moments(cv::Mat(contours[i]));
			int x = mom.m10 / mom.m00;
			int y = mom.m01 / mom.m00;
			data_drawImage0[y*drawImage0.step + x] = 1;//������������������λ�ã����ڶ�ӦdrawImage0ͼ��λ��������Ϊ1
			cv::Point k(i, y);
			number1.push_back(k);
		}

	}
	//����ÿ5��ͳ������Ϊ1�ĸ������������3���Ĵ洢��pp������
	std::vector<cv::Point>pp;
	for (int m = 4; m < drawImage0.rows; m++) {
		int sum = 0;
		cv::Mat mask(drawImage0, cv::Rect(0, m - 4, drawImage0.cols, 5));
		uchar*data_mask = mask.data;
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < mask.cols; j++) {
				sum += data_mask[i*mask.step + j];

			}
		}
		if (sum == 3) {
			cv::Point p(sum, m);
			pp.push_back(p);
		}
	}
	//��pp�����м�¼�����������ƽ��ֵ�����ں�number1�м�¼����������������Ƚϴ�С
	//��������֮��Ϊ3�ı�����contours0��
	int sumpp = 0;
	int len = pp.size();
	for (int i = 0; i < pp.size(); i++) {
		sumpp += pp[i].y;
	}
	sumpp = sumpp / len;
	std::vector<int>cc;
	std::vector<std::vector<cv::Point>>contours0;
	for (int i = 0; i < number1.size(); i++) {
		int d = abs(number1[i].y - sumpp);
		if (d <= 3) {//��������֮��Ϊ3
			contours0.push_back(contours[number1[i].x]);
		}
	}
	int size = contours0.size();
	cv::Rect r0 = cv::boundingRect(contours0[0]);
	cv::Rect r1 = cv::boundingRect(contours0[size - 1]);
	cv::Point p1(r0.x - 2, r0.y - 2);//��һ���������Ͻǣ�����ֱ���������ƫ����������
	cv::Point p2(r1.x + r1.width + 2, r1.y + r1.height + 2);// ���һ���������½ǣ�����ֱ���������ƫ����������
	cv::Mat imageROI(result, cv::Rect(p1, p2));
	imageROI.copyTo(dst);
}
//ϸ������
cv::Mat thinImage(const cv::Mat & src, const int maxIterations = -1)
{
	assert(src.type() == CV_8UC1);
	cv::Mat dst;
	int width = src.cols;
	int height = src.rows;
	src.copyTo(dst);
	int count = 0;  //��¼��������  
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) //���ƴ������ҵ�����������  
			break;
		std::vector<uchar *> mFlag; //���ڱ����Ҫɾ���ĵ�  
									//�Ե���  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//��������ĸ����������б��  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 255) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 * 255 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6 * 255)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 255) ++ap;
					if (p3 == 0 && p4 == 255) ++ap;
					if (p4 == 0 && p5 == 255) ++ap;
					if (p5 == 0 && p6 == 255) ++ap;
					if (p6 == 0 && p7 == 255) ++ap;
					if (p7 == 0 && p8 == 255) ++ap;
					if (p8 == 0 && p9 == 255) ++ap;
					if (p9 == 0 && p2 == 255) ++ap;

					if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0)
					{
						//���  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//����ǵĵ�ɾ��  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//ֱ��û�е����㣬�㷨����  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//��mFlag���  
		}

		//�Ե���  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//��������ĸ����������б��  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 255) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 * 255 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6 * 255)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 255) ++ap;
					if (p3 == 0 && p4 == 255) ++ap;
					if (p4 == 0 && p5 == 255) ++ap;
					if (p5 == 0 && p6 == 255) ++ap;
					if (p6 == 0 && p7 == 255) ++ap;
					if (p7 == 0 && p8 == 255) ++ap;
					if (p8 == 0 && p9 == 255) ++ap;
					if (p9 == 0 && p2 == 255) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{
						//���  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//����ǵĵ�ɾ��  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//ֱ��û�е����㣬�㷨����  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//��mFlag���  
		}
	}
	return dst;
}
//�ָ�ÿ������,��ԭͼ�Ϲ۲쵽01��08��94��ÿ������������01��08��94�����ָ�ٶ�01ϸ����ָͬ��08��94
std::vector<cv::Mat>dstsegmentNumber(cv::Mat&image) {
	cv::Mat Image;
	image.copyTo(Image);
	cv::medianBlur(Image, Image, 3);
	cv::threshold(Image, Image, 0, 255, cv::THRESH_OTSU);
	Image = 255 - Image;
	cv::Mat qq;
	Image.copyTo(qq);
	Image = 255 - Image;
	cv::Mat result;
	Image.copyTo(result);
	result = 255 - result;
	cv::Mat resultROI(result.rows, result.cols + 1, CV_8UC1, cv::Scalar(255));
	cv::Mat resultROI1(resultROI, cv::Rect(0, 0, result.cols, result.rows));
	qq.copyTo(resultROI1);
	uchar *data_resultROI = resultROI.data;
	std::vector<int>segmentPoint;
	for (int j = 0; j < resultROI.cols - 1; j++) {
		int sum1 = 0;
		int sum2 = 0;
		for (int i = 0; i < result.rows; i++) {
			sum1 += data_resultROI[i*resultROI.step + j];
			sum2 += data_resultROI[i*resultROI.step + (j + 1)];

		}
		if ((sum1 == 0) && (sum2 != 0)) {//���㵱ǰ�������ܺ�Ϊ�㣬��һ�����غͲ�Ϊ�㣬�������е�����

			segmentPoint.push_back(j);

		}

	}
	//ϸ����ָ�
	std::vector<cv::Mat>dst;//�������洢�ָ���ͼƬ
	for (int i = 0; i < segmentPoint.size() - 1; i++) {
		cv::Mat mask(image, cv::Rect(cv::Point(segmentPoint[i], 0), cv::Point(segmentPoint[i + 1], image.rows - 1)));
		cv::medianBlur(mask, mask, 3);
		cv::threshold(mask, mask, 0, 255, cv::THRESH_OTSU);
		mask = 255 - mask;
		mask = thinImage(mask);//ϸ��
		mask = 255 - mask;
		dst.push_back(mask);
	}
	std::vector<cv::Mat>dst2;
	for (int i = 0; i < dst.size(); i++) {
		cv::Mat mask1 = dst[i];
		mask1 = 255 - mask1;
		uchar*data_mask1 = mask1.data;
		std::vector<int>segmentPoint1;
		for (int j = 0; j < mask1.cols - 1; j++) {
			int sum3 = 0;
			int sum4 = 0;
			for (int i = 0; i < mask1.rows; i++) {
				sum3 += data_mask1[i*mask1.step + j];
				sum4 += data_mask1[i*mask1.step + (j + 1)];

			}
			if ((sum3>255) && (sum4 == 255)) {//���㵱ǰ�����غ�Ϊ����255����һ�����غ�Ϊ255��
											  //��ǰ�����غ�Ϊ255����һ�����غ�Ϊ����255
				segmentPoint1.push_back(j + 1);//���µ�ǰ�е���һ��

			}
			else if ((sum3 == 255) && (sum4 > 255)) {//���µ�ǰ��
				segmentPoint1.push_back(j);

			}

		}
		int p0 = (segmentPoint1[0] + segmentPoint1[1]) / 2;//�ָ���ޣ�������е�ƽ���У�
														   //����ϸ�����01֮������һ��ϸ�ߣ�ȥ�����ߵ��е�������Ϊ�ָ���ޣ�
		cv::Mat mask2(mask1, cv::Rect(cv::Point(0, 0), cv::Point(p0, mask1.rows - 1)));
		dst2.push_back(mask2);
		cv::Mat mask3(mask1, cv::Rect(cv::Point(p0, 0), cv::Point(mask1.cols - 1, mask1.rows - 1)));
		dst2.push_back(mask3);
		segmentPoint1.clear();
	}

	dst.clear();
	return dst2;//���ص��Ǵ洢ϸ��ͼ�������

}
//�ұ�Ե��1/2X��
//6����Ե����� 2,3,5,6,8,9
//4����Ե����� 0,4
//2����Ե����� 7
//1Ϊ������������ú���������֮�ȼ��ɱ��
std::vector<cv::Point> searchHalfX(cv::Mat &image) {
	std::vector<cv::Point>keypoint;
	cv::Mat Image(image.rows + 2, image.cols + 2, CV_8UC1, cv::Scalar(255));
	cv::Mat roi(Image, cv::Rect(1, 1, image.cols, image.rows));
	image.copyTo(roi);
	uchar*data_Image = Image.data;
	int j = Image.cols / 2 - 1;//1/2X��
	int a1;
	int a2;
	for (int i = 0; i < Image.rows - 1; i++) {
		a1 = data_Image[i*Image.step + j];
		a2 = data_Image[(i + 1)*Image.step + j];
		if ((a1 - a2) != 0 && a1 == 255) {//�ж����������������ֵ�仯�������ȷ���Ƿ�Ϊ���� 

			cv::Point p(j, i + 1);
			keypoint.push_back(p);
		}
		else if ((a1 - a2) != 0 && a1 == 0)
		{
			cv::Point p(j, i);
			keypoint.push_back(p);
		}
	}

	return keypoint;
}
//����2/3Y���ı�Ե��
std::vector<cv::Point> searchTwoThreeY(cv::Mat &image) {
	std::vector<cv::Point>keypoint;
	cv::Mat Image(image.rows + 2, image.cols + 2, CV_8UC1, cv::Scalar(255));
	cv::Mat roi(Image, cv::Rect(1, 1, image.cols, image.rows));
	image.copyTo(roi);
	uchar*data_Image = Image.data;
	int i = 5 * Image.rows / 6 - 1;
	int a1;
	int a2;
	for (int j = 0; j < Image.cols - 1; j++) {
		a1 = data_Image[i*Image.step + j];
		a2 = data_Image[i*Image.step + (j + 1)];
		if ((a1 - a2) != 0 && a1 == 255) {//
			cv::Point p(j + 1, i);
			keypoint.push_back(p);
		}
		else if ((a1 - a2) != 0 && a1 == 0)
		{
			cv::Point p(j, i);
			keypoint.push_back(p);
		}

	}

	return keypoint;

}
int X1(cv::Mat& image, std::vector<cv::Point>keypoint1) {//������������������ƫ��10�����ص�λ��������
	uchar*data_image = image.data;
	int i1 = keypoint1[2].y - 10;
	int b1;
	int c;
	for (int j = 0; j < image.cols; j++) {
		b1 = data_image[i1*image.step + j];
		if (b1 == 0) {//��������һ������Ϊ��ĵ��¼������X����
			c = j;
			break;
		}
	}
	return c;
}
int X2(cv::Mat& image, std::vector<cv::Point>keypoint1) {//������������������ƫ��10�����ص�λ��������

	uchar*data_image = image.data;
	int i1 = keypoint1[3].y + 10;
	int b1;
	int c;
	for (int j = 0; j < image.cols; j++) {
		b1 = data_image[i1*image.step + j];
		if (b1 == 0) {//��������һ������Ϊ��ĵ��¼������X����
			c = j;
			break;
		}
	}
	return c;
}
int contourSize(cv::Mat &image) {//�������ֵ�������
	cv::Mat Image(image.rows + 2, image.cols + 2, CV_8UC1, cv::Scalar(255));
	cv::Mat roi(Image, cv::Rect(1, 1, image.cols, image.rows));
	image.copyTo(roi);

	Image = 255 - Image;
	int size;
	std::vector<std::vector<cv::Point>>contours;
	cv::findContours(Image, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
	size = contours.size();
	return size;

}
bool condition(cv::Mat &image) {//�ж��������������ϰ벿�ֻ����°벿�֣������ж�6��9�����������·���true
	bool a(false);
	cv::Mat Image(image.rows + 2, image.cols + 2, CV_8UC1, cv::Scalar(255));
	cv::Mat roi(Image, cv::Rect(1, 1, image.cols, image.rows));
	image.copyTo(roi);
	Image = 255 - Image;
	std::vector<std::vector<cv::Point>>contours;
	cv::findContours(Image, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
	int y1 = image.rows / 2 - 1;
	//int x1 = image.cols / 2 - 1;
	cv::Moments mom = cv::moments(cv::Mat(contours[1]));
	int a1 = mom.m01 / mom.m00;
	if (a1 > y1) {
		a = true;
	}
	return a;

}



//����ʶ����
void numberDistinguish(cv::Mat &image) {
	//��һ������ȷ��λҪʶ������ֵ�λ��
	//�ڶ������ָ�����
	//�����������ʶ��

	//��һ��
	cv::Mat result2;
	f3(image, result2);
	//�ڶ����ָ�
	std::vector<cv::Mat>result3;//�洢�ָ���ϸ��ͼ�������
	result3 = dstsegmentNumber(result2);
	std::vector<int>number;
	for (int i = 0; i < result3.size(); i++) {
		cv::Mat numberImage1;
		cv::Mat numberImage2;
		numberImage1 = result3[i];
		numberImage1.copyTo(numberImage2);
		numberImage2 = 255 - numberImage2;
		std::vector<std::vector<cv::Point>>contours;
		cv::findContours(numberImage1, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);//ֻ��������
		cv::Rect r0 = cv::boundingRect(contours[0]);//�߿��Χ�������������ж����ֿ�߱�
													//�������Ϊ255���к��У��Ա�����������
		cv::Mat resultRoi(numberImage2, cv::Rect(r0));
		cv::Mat image1(resultRoi.rows + 2, resultRoi.cols + 2, CV_8UC1, cv::Scalar(255));
		cv::Mat roi(image1, cv::Rect(1, 1, resultRoi.cols, resultRoi.rows));
		resultRoi.copyTo(roi);
		image1 = 255 - image1;
		std::vector<std::vector<cv::Point>>contours0;
		cv::findContours(image1, contours0, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);//���������������ж����ֵ�������
																				  //��ʼʶ��
																				  //���жϸ������Ƿ�Ϊ1

		if (r0.width <= r0.height / 2 && contours0.size() == 1) {
			//std::cout << "����Ϊ1" << std::endl;
			number.push_back(1);
		}
		else {


			std::vector<cv::Point>keypoint1;//keypoint1Ϊ1/2X���ı�Ե�㼯��
			keypoint1 = searchHalfX(resultRoi);
			int len0 = keypoint1.size();
			int Q1;
			if (len0 == 6) {//235689
				Q1 = contourSize(resultRoi);//������Ϊ1��235����������Ϊ2��6��9����������Ϊ3��8��

				if (Q1 == 3) {
					//std::cout << "����Ϊ8" << std::endl;
					number.push_back(8);
				}
				else if (Q1 == 2) {

					bool t;
					t = condition(resultRoi);
					if (t == true) {
						//std::cout << "����Ϊ6" << std::endl;
						number.push_back(6);
					}

					else if (t == false) {
						//std::cout << "����Ϊ9" << std::endl;
						number.push_back(9);
					}
				}
				else if (Q1 == 1) {
					cv::Mat Image(resultRoi.rows + 2, resultRoi.cols + 2, CV_8UC1, cv::Scalar(255));
					cv::Mat roi(Image, cv::Rect(1, 1, resultRoi.cols, resultRoi.rows));
					resultRoi.copyTo(roi);
					int k1, k2;
					int n1 = Image.cols / 2 - 1;
					k1 = X1(Image, keypoint1);
					k2 = X2(Image, keypoint1);
					if (k1<n1&&k2>n1) {//ƫ���������������
									   //std::cout << "����Ϊ5" << std::endl;
						number.push_back(5);
					}

					else if (k1 > n1 && k2 > n1) {
						//std::cout << "����Ϊ3" << std::endl;
						number.push_back(3);
					}
					else {
						//std::cout << "����Ϊ2" << std::endl;
						number.push_back(2);
					}
				}
			}
			else if (len0 == 4) {
				//���2/3Y����Ե������Ϊ2������Ϊ4
				//���2/3Y����Ե������Ϊ4������Ϊ0
				std::vector<cv::Point>keypoint2;//keypoint2Ϊ2/3Y���ı�Ե�㼯��
				keypoint2 = searchTwoThreeY(resultRoi);
				int len1 = keypoint2.size();
				if (len1 == 2) {
					//std::cout << "����Ϊ4" << std::endl;
					number.push_back(4);
				}
				else if (len1 == 4) {
					//std::cout << "����Ϊ0" << std::endl;
					number.push_back(0);
				}

			}
			else if (len0 == 2) {
				//std::cout << "����Ϊ7" << std::endl;
				number.push_back(7);
			}

		}

	}
	std::cout << "��������������Ϊ";
	for (int i = 0; i < number.size(); i++) {

		std::cout << number[i] << " ";

	}
}

int main() {

	cv::Mat image = cv::imread("E:/win10/123.PNG", 0);
	numberDistinguish(image);
	system("pause");
	cv::waitKey(0);
}