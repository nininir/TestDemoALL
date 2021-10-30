#include<iostream>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

// AreaLimit����ͨ����� ��ֵ CheckMode��0����ȥ��������1����ȥ�������� NeihborMode��0����4����1����8����
void RemoveSmallRegion(Mat &src, Mat &dst,int thre,int AreaLimit, int Checkmode, int NeihborMode)
{
	int RemoveCount = 0;//��¼ȥ������
//��¼ÿ�����ص����״̬�ı�ǩ��0����δ��飬1�������ڼ��,2�����鲻�ϸ���Ҫ��ת��ɫ����3������ϸ������
	Mat PointLabel = Mat::zeros(src.size(), CV_8UC1);
	uchar *iData = src.data;
	uchar *iLabel = PointLabel.data;
	if (Checkmode == 1)
	{
		cout << "ȥ��С����";		
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
		cout << "ȥ���׶�";
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

	vector<Point2i> NeihborPos;//��¼�����λ��
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		cout << "Neighbor mode: 8����." << endl;
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, 1));
		NeihborPos.push_back(Point2i(1, -1));
	}
	else cout << "Neighbor mode: 4����." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;

	//��ʼ���
	for (int j = 0; j < src.rows; j++)
	{
		for (int i = 0; i < src.cols; i++)
		{
			//iLabel[27 * PointLabel.step + 360] = 255;
			
			if (iLabel[j*PointLabel.step + i] == 0)
			{
				//********��ʼ�õ㴦�ļ��********** 	
				vector<Point2i> GrowBuffer;
				int CheckResult = 0;
				GrowBuffer.push_back(Point2i(i, j));
				iLabel[j*PointLabel.step + i] = 1;
				
				//����������
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
				if (GrowBuffer.size() > AreaLimit) CheckResult = 2;  //�жϽ�����Ƿ񳬳��޶��Ĵ�С����1Ϊδ������2Ϊ���� 
				else { CheckResult = 1; RemoveCount++; }
				
				for (int i = 0; i < GrowBuffer.size(); i++)
				{
					CurrX = GrowBuffer.at(i).x;
					CurrY = GrowBuffer.at(i).y;
					iLabel[CurrY*PointLabel.step + CurrX] += CheckResult;
				}
				//********�����õ㴦�ļ��**********
			}
		}
	}
	Checkmode = 255 * (1 - Checkmode);

	uchar * iDstData = dst.data;
	//��ʼ��ת�����С������
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