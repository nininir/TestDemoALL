#include<iostream>
#include<opencv2\opencv.hpp>

using namespace cv;
using namespace std;

int main() {

	vector<String> filenames;
	String path = "C:\\Users\\那个技能\\Desktop\\jj";
	String SavePath = "C:\\Users\\那个技能\\Desktop\\LL\\";
	glob(path, filenames);//opencv里面用来读取指定路径下文件名的一个很好用的函数
	
	for (int i = 0; i < filenames.size(); i++)
	{
		Mat src = imread(filenames[i], 0);
		Mat dst, dst1;
		if (!src.data)
		{
			cout << "could not load image" << endl;
			return -1;
		}

		dst.create(src.rows, src.cols, src.type());
		dst.copyTo(dst1);

		String tt, ss;
		uchar *d_data = dst.data;
		uchar *s_data = src.data;
		uchar *d1_data = dst1.data;


		//水平翻转
		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				d_data[j*dst.step + i] = s_data[j*src.step + src.cols - 1 - i];
			}
		}

		tt = SavePath + "4" + filenames[i].substr(29);
		imwrite(tt, dst);

		////垂直翻转
		//for (int j = 0; j < src.rows; j++)
		//{
		//	for (int i = 0; i < src.cols; i++)
		//	{
		//		d1_data[j*dst.step + i] = s_data[(src.rows - j - 1)*src.step + i];
		//	}
		//}

		//ss = SavePath + "5" + filenames[i].substr(29);
		//imwrite(ss, dst1);
	}
	
	waitKey(0);
	return 0;

}
