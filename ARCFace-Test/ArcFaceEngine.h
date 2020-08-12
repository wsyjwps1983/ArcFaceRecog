#pragma once

#include "stdafx.h"
#include "merror.h"
#include <opencv2\opencv.hpp>
#include <vector>
#include "Config.h"
//#define PRO

#ifdef PRO
#include "pro/arcsoft_face_sdk.h"
#else
#include "free/arcsoft_face_sdk.h"
#endif
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>
//#include <opencv2/nonfree/nonfree.hpp>
//#include<opencv2/legacy/legacy.hpp>
//#include <opencv2/video/background_segm.hpp>
#define CV_VERSION_ID CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
#ifdef _DEBUG
#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) "opencv_" name CV_VERSION_ID
#endif
#pragma comment( lib, cvLIB("calib3d"))
#pragma comment( lib, cvLIB("contrib"))
#pragma comment( lib, cvLIB("core"))
#pragma comment( lib, cvLIB("features2d"))
#pragma comment( lib, cvLIB("flann"))
#pragma comment( lib, cvLIB("highgui"))
#pragma comment( lib, cvLIB("imgproc"))
#pragma comment( lib, cvLIB("legacy"))
#pragma comment( lib, cvLIB("ml"))
#pragma comment( lib, cvLIB("nonfree"))
#pragma comment( lib, cvLIB("objdetect"))
#pragma comment( lib, cvLIB("ocl"))
#pragma comment( lib, cvLIB("photo"))
#pragma comment( lib, cvLIB("stitching"))
#pragma comment( lib, cvLIB("superres"))
#pragma comment( lib, cvLIB("ts"))
#pragma comment( lib, cvLIB("video"))
#pragma comment( lib, cvLIB("videostab"))
//#pragma comment( lib, "opencv_ffmpeg2411.lib")
using namespace cv;

#pragma comment( lib, "libarcsoft_face_engine.lib")
class ArcFaceEngine
{
public:
	ArcFaceEngine();
	ArcFaceEngine(string configPath);
	~ArcFaceEngine();
public:
	//����
	MRESULT ActiveSDK(char* appID, char* sdkKey, char* activeKey);
	//��ȡ�����ļ���Ϣ�����Ի�ȡ����Ч�ڣ�
	MRESULT GetActiveFileInfo(ASF_ActiveFileInfo& activeFileInfo);
	//��ʼ������
	MRESULT InitEngine(ASF_DetectMode mode);	
	//�ͷ�����
	MRESULT UnInitEngine();	
	//�������
	MRESULT PreDetectFace(IplImage* image, ASF_SingleFaceInfo& faceRect, bool isRGB);
	//�����������
	MRESULT PreDetectFaces(IplImage* image, ASF_MultiFaceInfo& faceRect, bool isRGB);
	//����������ȡ
	MRESULT PreExtractFeature(IplImage* image, ASF_FaceFeature& feature, ASF_SingleFaceInfo& faceRect);
	//�����ȶ�
	MRESULT FacePairMatching(MFloat &confidenceLevel, ASF_FaceFeature feature1, ASF_FaceFeature feature2, 
		ASF_CompareModel compareModel = ASF_LIFE_PHOTO);
	//���û�����ֵ
	MRESULT SetLivenessThreshold(MFloat	rgbLiveThreshold, MFloat irLiveThreshold);
	//RGBͼ���������Լ��
	MRESULT FaceASFProcess(ASF_MultiFaceInfo detectedFaces, IplImage *img, ASF_AgeInfo &ageInfo,
		ASF_GenderInfo &genderInfo, ASF_Face3DAngle &angleInfo, ASF_LivenessInfo& liveNessInfo);
	//IR������
	MRESULT FaceASFProcess_IR(ASF_MultiFaceInfo detectedFaces, IplImage *img, ASF_LivenessInfo& irLiveNessInfo);
	//��ȡ�汾��Ϣ
	const ASF_VERSION GetVersion();
	

public:
	string m_configPath;
	
private:
	MHandle m_hEngine;
	string m_appID;
	string m_sdkKey;
	string m_activeKey;
	string m_tag;
	float m_rgbLiveThreshold;
	float m_irLiveThreshold;
	int m_rgbCameraId;
	int m_irCameraId;

	string m_registerPath;//ע��ͼ���ַ
	std::vector<ASF_FaceFeature> m_featuresVec;//ע������������
};

//ͼƬ�ü�
void PicCutOut(IplImage* src, IplImage* dst, int x, int y);
//��ɫ�ռ�ת��
int ColorSpaceConversion(IplImage* image, MInt32 format, ASVLOFFSCREEN& offscreen);
