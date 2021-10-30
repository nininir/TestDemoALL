#include<iostream>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

// AreaLimit：连通域面积 阈值 CheckMode：0代表去除黑区域，1代表去除白区域 NeihborMode：0代表4邻域，1代表8邻域
void RemoveSmallRegion(Mat &src, Mat &dst,int thre,int AreaLimit, int Checkmode, int NeihborMode)
{
	int RemoveCount = 0;//记录去除个数
//记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查
	Mat PointLabel = Mat::zeros(src.size(), CV_8UC1);
	uchar *iData = src.data;
	uchar *iLabel = PointLabel.data;
	if (Checkmode == 1)
	{
		cout << "去除小区域";		
		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				if (iData[j*src.step + i] < thre)
				{
					iLabel[j*PointLabel.step + i] = 3;
				}
			}
		}
	}
	else
	{
		cout << "去除孔洞";
		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				if (iData[j*src.step + i] > thre)
				{
					iLabel[j*PointLabel.step + i] = 3;
				}
			}
		}
	}

	vector<Point2i> NeihborPos;//记录邻域点位置
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		cout << "Neighbor mode: 8邻域." << endl;
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, 1));
		NeihborPos.push_back(Point2i(1, -1));
	}
	else cout << "Neighbor mode: 4邻域." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;

	//开始检测
	for (int j = 0; j < src.rows; j++)
	{
		for (int i = 0; i < src.cols; i++)
		{
			//iLabel[27 * PointLabel.step + 360] = 255;
			
			if (iLabel[j*PointLabel.step + i] == 0)
			{
				//********开始该点处的检查********** 	
				vector<Point2i> GrowBuffer;
				int CheckResult = 0;
				GrowBuffer.push_back(Point2i(i, j));
				iLabel[j*PointLabel.step + i] = 1;
				
				//区域生长法
				for (int m = 0; m < GrowBuffer.size(); m++)
				{
					for (int n = 0; n < NeihborCount; n++)
					{
						CurrX = GrowBuffer.at(m).x + NeihborPos.at(n).x;
						CurrY = GrowBuffer.at(m).y + NeihborPos.at(n).y;
						if (CurrX >= 0 && CurrY >= 0 && CurrY < src.rows && CurrX < src.cols)
						{
							if (iLabel[CurrY*PointLabel.step + CurrX] == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));
								iLabel[CurrY*PointLabel.step + CurrX] = 1;
							}
						}
					}
				}
				if (GrowBuffer.size() > AreaLimit) CheckResult = 2;  //判断结果（是否超出限定的大小），1为未超出，2为超出 
				else { CheckResult = 1; RemoveCount++; }
				
				for (int i = 0; i < GrowBuffer.size(); i++)
				{
					CurrX = GrowBuffer.at(i).x;
					CurrY = GrowBuffer.at(i).y;
					iLabel[CurrY*PointLabel.step + CurrX] += CheckResult;
				}
				//********结束该点处的检查**********
			}
		}
	}
	Checkmode = 255 * (1 - Checkmode);

	uchar * iDstData = dst.data;
	//开始反转面积过小的区域
	for (int j = 0; j < src.rows; j++)
	{
		for (int i = 0; i < src.cols; i++)
		{
			if (iLabel[j*src.step + i] == 2)
				iDstData[j*src.step + i] = Checkmode;
			else if (iLabel[j*src.step + i] == 3)
				iDstData[j*src.step + i] = iData[j*src.step + i];
		}
	}
}

int main(){
	Mat src = imread("1.tif", 0);
	Mat dst = Mat::zeros(src.size(), CV_8UC1);
	
	if (!src.data)
	{
		cout << "imread is error!";
		return -1;
	}
	threshold(src, src, 77, 255, THRESH_BINARY | THRESH_OTSU);
	medianBlur(src, src, 5);
	RemoveSmallRegion(src, dst, 70, 1000, 0, 0);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}