#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


//定位数字所在区域
void f3(cv::Mat &image, cv::Mat &dst) {
	cv::Mat result;
	image.copyTo(result);
	cv::medianBlur(image, image, 3);//去噪
	cv::Mat erodeImage;
	cv::erode(image, erodeImage, cv::Mat(), cv::Point(-1, -1), 2);//腐蚀两次
	cv::threshold(erodeImage, erodeImage, 0, 255, cv::THRESH_OTSU);//二值化
	erodeImage = 255 - erodeImage;
	std::vector<std::vector<cv::Point>>contours;
	cv::findContours(erodeImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);//找外轮廓
	std::vector<cv::Point>number1;//记录满足条件的轮廓的序号和质心Y坐标的容器
	cv::Mat drawImage0(image.size(), CV_8UC1, cv::Scalar(0));//用于标记（符合条件的轮廓的质心）的图像
	uchar*data_drawImage0 = drawImage0.data;
	for (int i = 0; i < contours.size(); i++) {//筛选轮廓
		cv::Rect r0 = cv::boundingRect(contours[i]);
		int d = cv::contourArea(contours[i]);
		int a = r0.width*r0.height;
		double b = r0.width / (double)r0.height;
		if (a >= 2000 && a <= 5000 && abs(b - 1) < 0.5) {

			cv::Moments mom = cv::moments(cv::Mat(contours[i]));
			int x = mom.m10 / mom.m00;
			int y = mom.m01 / mom.m00;
			data_drawImage0[y*drawImage0.step + x] = 1;//满足条件的轮廓质心位置，将在对应drawImage0图像位置像素设为1
			cv::Point k(i, y);
			number1.push_back(k);
		}

	}
	//采用每5行统计像素为1的个数，满足等于3个的存储在pp容器中
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
	//求pp容器中记录的行数坐标的平均值，用于和number1中记录的轮廓质心纵坐标比较大小
	//满足坐标之差为3的保存在contours0中
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
		if (d <= 3) {//满足坐标之差为3
			contours0.push_back(contours[number1[i].x]);
		}
	}
	int size = contours0.size();
	cv::Rect r0 = cv::boundingRect(contours0[0]);
	cv::Rect r1 = cv::boundingRect(contours0[size - 1]);
	cv::Point p1(r0.x - 2, r0.y - 2);//第一个轮廓左上角，这里分别向左向上偏移两个像素
	cv::Point p2(r1.x + r1.width + 2, r1.y + r1.height + 2);// 最后一个轮廓右下角，这里分别向右向下偏移两个像素
	cv::Mat imageROI(result, cv::Rect(p1, p2));
	imageROI.copyTo(dst);
}
//细化函数
cv::Mat thinImage(const cv::Mat & src, const int maxIterations = -1)
{
	assert(src.type() == CV_8UC1);
	cv::Mat dst;
	int width = src.cols;
	int height = src.rows;
	src.copyTo(dst);
	int count = 0;  //记录迭代次数  
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) //限制次数并且迭代次数到达  
			break;
		std::vector<uchar *> mFlag; //用于标记需要删除的点  
									//对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
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
						//标记  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空  
		}

		//对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
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
						//标记  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空  
		}
	}
	return dst;
}
//分割每个数字,从原图上观察到01、08、94、每两个相连、先01、08、94两两分割、再对01细化后分割、同理08、94
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
		if ((sum1 == 0) && (sum2 != 0)) {//满足当前列像素总和为零，下一列像素和不为零，记下这列的列数

			segmentPoint.push_back(j);

		}

	}
	//细化后分割
	std::vector<cv::Mat>dst;//容器，存储分割后的图片
	for (int i = 0; i < segmentPoint.size() - 1; i++) {
		cv::Mat mask(image, cv::Rect(cv::Point(segmentPoint[i], 0), cv::Point(segmentPoint[i + 1], image.rows - 1)));
		cv::medianBlur(mask, mask, 3);
		cv::threshold(mask, mask, 0, 255, cv::THRESH_OTSU);
		mask = 255 - mask;
		mask = thinImage(mask);//细化
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
			if ((sum3>255) && (sum4 == 255)) {//满足当前列像素和为大于255、下一列像素和为255或
											  //当前列像素和为255，下一列像素和为大于255
				segmentPoint1.push_back(j + 1);//记下当前列的下一列

			}
			else if ((sum3 == 255) && (sum4 > 255)) {//记下当前列
				segmentPoint1.push_back(j);

			}

		}
		int p0 = (segmentPoint1[0] + segmentPoint1[1]) / 2;//分割界限：满足的列的平均列，
														   //（如细化后的01之间相连一条细线，去这条线的中点所在列为分割界限）
		cv::Mat mask2(mask1, cv::Rect(cv::Point(0, 0), cv::Point(p0, mask1.rows - 1)));
		dst2.push_back(mask2);
		cv::Mat mask3(mask1, cv::Rect(cv::Point(p0, 0), cv::Point(mask1.cols - 1, mask1.rows - 1)));
		dst2.push_back(mask3);
		segmentPoint1.clear();
	}

	dst.clear();
	return dst2;//返回的是存储细化图像的容器

}
//找边缘点1/2X处
//6个边缘点比如 2,3,5,6,8,9
//4个边缘点比如 0,4
//2个边缘点比如 7
//1为特殊情况，采用横轴与纵轴之比即可辨别
std::vector<cv::Point> searchHalfX(cv::Mat &image) {
	std::vector<cv::Point>keypoint;
	cv::Mat Image(image.rows + 2, image.cols + 2, CV_8UC1, cv::Scalar(255));
	cv::Mat roi(Image, cv::Rect(1, 1, image.cols, image.rows));
	image.copyTo(roi);
	uchar*data_Image = Image.data;
	int j = Image.cols / 2 - 1;//1/2X处
	int a1;
	int a2;
	for (int i = 0; i < Image.rows - 1; i++) {
		a1 = data_Image[i*Image.step + j];
		a2 = data_Image[(i + 1)*Image.step + j];
		if ((a1 - a2) != 0 && a1 == 255) {//判断上下两个点的像素值变化情况、来确定是否为交点 

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
//搜索2/3Y处的边缘点
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
int X1(cv::Mat& image, std::vector<cv::Point>keypoint1) {//遍历第三个交点向上偏移10个像素的位置所在行
	uchar*data_image = image.data;
	int i1 = keypoint1[2].y - 10;
	int b1;
	int c;
	for (int j = 0; j < image.cols; j++) {
		b1 = data_image[i1*image.step + j];
		if (b1 == 0) {//搜索到第一个像素为零的点记录下他的X坐标
			c = j;
			break;
		}
	}
	return c;
}
int X2(cv::Mat& image, std::vector<cv::Point>keypoint1) {//遍历第三个交点向下偏移10个像素的位置所在行

	uchar*data_image = image.data;
	int i1 = keypoint1[3].y + 10;
	int b1;
	int c;
	for (int j = 0; j < image.cols; j++) {
		b1 = data_image[i1*image.step + j];
		if (b1 == 0) {//搜索到第一个像素为零的点记录下他的X坐标
			c = j;
			break;
		}
	}
	return c;
}
int contourSize(cv::Mat &image) {//返回数字的轮廓数
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
bool condition(cv::Mat &image) {//判断内轮廓质心在上半部分还是下半部分（用于判断6和9）、质心在下返回true
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



//数字识别函数
void numberDistinguish(cv::Mat &image) {
	//第一步：精确定位要识别的数字的位置
	//第二步：分割数字
	//第三步：逐个识别

	//第一步
	cv::Mat result2;
	f3(image, result2);
	//第二步分割
	std::vector<cv::Mat>result3;//存储分割后的细化图像的容器
	result3 = dstsegmentNumber(result2);
	std::vector<int>number;
	for (int i = 0; i < result3.size(); i++) {
		cv::Mat numberImage1;
		cv::Mat numberImage2;
		numberImage1 = result3[i];
		numberImage1.copyTo(numberImage2);
		numberImage2 = 255 - numberImage2;
		std::vector<std::vector<cv::Point>>contours;
		cv::findContours(numberImage1, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);//只找外轮廓
		cv::Rect r0 = cv::boundingRect(contours[0]);//边框包围外轮廓、用于判断数字宽高比
													//添加像素为255的行和列，以便找内外轮廓
		cv::Mat resultRoi(numberImage2, cv::Rect(r0));
		cv::Mat image1(resultRoi.rows + 2, resultRoi.cols + 2, CV_8UC1, cv::Scalar(255));
		cv::Mat roi(image1, cv::Rect(1, 1, resultRoi.cols, resultRoi.rows));
		resultRoi.copyTo(roi);
		image1 = 255 - image1;
		std::vector<std::vector<cv::Point>>contours0;
		cv::findContours(image1, contours0, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);//找所有轮廓，以判断数字的轮廓数
																				  //开始识别
																				  //先判断该数字是否为1

		if (r0.width <= r0.height / 2 && contours0.size() == 1) {
			//std::cout << "数字为1" << std::endl;
			number.push_back(1);
		}
		else {


			std::vector<cv::Point>keypoint1;//keypoint1为1/2X处的边缘点集合
			keypoint1 = searchHalfX(resultRoi);
			int len0 = keypoint1.size();
			int Q1;
			if (len0 == 6) {//235689
				Q1 = contourSize(resultRoi);//轮廓数为1（235），轮廓数为2（6，9），轮廓数为3（8）

				if (Q1 == 3) {
					//std::cout << "数字为8" << std::endl;
					number.push_back(8);
				}
				else if (Q1 == 2) {

					bool t;
					t = condition(resultRoi);
					if (t == true) {
						//std::cout << "数字为6" << std::endl;
						number.push_back(6);
					}

					else if (t == false) {
						//std::cout << "数字为9" << std::endl;
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
					if (k1<n1&&k2>n1) {//偏移搜索满足的条件
									   //std::cout << "数字为5" << std::endl;
						number.push_back(5);
					}

					else if (k1 > n1 && k2 > n1) {
						//std::cout << "数字为3" << std::endl;
						number.push_back(3);
					}
					else {
						//std::cout << "数字为2" << std::endl;
						number.push_back(2);
					}
				}
			}
			else if (len0 == 4) {
				//如果2/3Y处边缘交点数为2，数字为4
				//如果2/3Y处边缘交点数为4，数字为0
				std::vector<cv::Point>keypoint2;//keypoint2为2/3Y处的边缘点集合
				keypoint2 = searchTwoThreeY(resultRoi);
				int len1 = keypoint2.size();
				if (len1 == 2) {
					//std::cout << "数字为4" << std::endl;
					number.push_back(4);
				}
				else if (len1 == 4) {
					//std::cout << "数字为0" << std::endl;
					number.push_back(0);
				}

			}
			else if (len0 == 2) {
				//std::cout << "数字为7" << std::endl;
				number.push_back(7);
			}

		}

	}
	std::cout << "从左到右数字依次为";
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