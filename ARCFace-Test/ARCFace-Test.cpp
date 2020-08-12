// ARCFace-Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ArcFaceEngine.h"
#include "Config.h"
#include <string>
#include <windows.h>
#include <time.h>
#include "FaceRecognition.h"
#define SafeFree(p) { if ((p)) free(p); (p) = NULL; }
#define SafeArrayDelete(p) { if ((p)) delete [] (p); (p) = NULL; } 
#define SafeDelete(p) { if ((p)) delete (p); (p) = NULL; } 

using namespace std;

#define APPID "YourAPPID"
#define SDKKEY "YourADKKEY"



int _tmain(int argc, _TCHAR* argv[])
{

	string ConfigFile("../ARCFace-Test/setting.ini");
	CFaceRecognition arcFace;
	
	arcFace.init();
	
	

	
	VideoCapture capture;
	capture.open(0);
	if (!capture.isOpened())
		cout << "fail to open!" << endl;
	
	IplImage image;
	bool stop = false;

	namedWindow("FaceRecognition");
	if (!capture.grab())
	{
		cout << "Can not grab images." << endl;
		return -1;
	}
	
	Mat frame;
	capture.retrieve(frame);
	int font_face = cv::FONT_HERSHEY_COMPLEX;
	
	while (!stop)
	{
		
		capture >> frame;
		if (frame.empty())
		{
			cout << "读取视频失败" << endl;
			break;
		}


		capture.retrieve(frame);
		image = IplImage(frame);
		vector<faceResult> recogResults;
		arcFace.faceCompare(&image,recogResults);
		for (int i = 0; i < recogResults.size(); i++)
		{
			rectangle(frame, recogResults[i].faceRect, Scalar(255, 0, 0));
			putText(frame, recogResults[i].faceName, Point(recogResults[i].faceRect.x, recogResults[i].faceRect.y), font_face,2.0, Scalar(255, 255, 0), 3 );
		}
		imshow("FaceRecognition", frame);
		recogResults.clear();
		int c = waitKey(27);
		//按下ESC或者到达指定的结束帧后退出读取视频
		if ((char)c == 27)
		{
			stop = true;
		}
	}

	return 0;
}

