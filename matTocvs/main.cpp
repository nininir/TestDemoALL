#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <sstream>
#include <string>
#include<opencv2/opencv.hpp>
#include <fstream>

using namespace std;
using namespace cv;



void writeToCSV(std::string filename, cv::Mat m)
{
	std::ofstream myfile;
	myfile.open(filename.c_str());
	myfile << cv::format(m, cv::Formatter::FMT_CSV) << std::endl;
	myfile.close();
}

void saveMat(const char* filename, cv::Mat inputMat)//将mat矩阵保存到CSV文件中
{
	try
	{
		FILE* fpt = fopen(filename, "w");
		int rows = inputMat.rows;
		int clos = inputMat.cols;
		float* s_data = (float*)inputMat.data;

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < clos; j++)
			{
				if (j < clos - 1)
					fprintf(fpt, "%f,", s_data[i * inputMat.step / sizeof(float) + j]);
				else
					fprintf(fpt, "%f\n", s_data[i * inputMat.step / sizeof(float) + j]);
			}
		}
		fclose(fpt);
	}
	
	catch (...)
	{
		cout << "filename is null" << endl;
	}
}

void SaveMatToCsv(const char* filename, cv::Mat inputMat)
{
	ofstream outFile; //输出文件流(输出到文件)
	outFile.open(filename, ios::out);
	float* s_data = (float*)inputMat.data;

	for (int i = 0; i < inputMat.rows; i++)
	{
		for (int j = 0; j < inputMat.cols; j++)
		{
			if (j < inputMat.cols - 1)
				outFile << to_string(s_data[i * inputMat.step / sizeof(float) + j]);
			else
				outFile << to_string(s_data[i * inputMat.step / sizeof(float) + j]) << "\n";

		}
	}
	outFile.close();
}


int main()
{
	Mat src = imread("C:\\cvProject\\maTocvs\\maTocvs\\1.tif", 0);
	if (!src.data)
	{
		cout << "imread is error!" << endl;
		return -1;
	}

	//string filename = "C:\\Users\\vm\\Desktop\\mat.csv";
	//writeToCSV(filename, src);


	src.convertTo(src, CV_32FC1);

	double time0 = static_cast<double>(getTickCount());

	saveMat("C:\\Users\\vm\\Desktop\\mat.csv", src);
	//writeToCSV(filename, src);
	//SaveMatToCsv("C:\\Users\\mxl\\Desktop\\1009_2\\mat.csv", src);

	time0 = ((double)getTickCount() - time0) / getTickFrequency();
	cout << time0 << endl;

	cv::waitKey(0);
	return 0;
}