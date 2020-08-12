#pragma once
#include "stdafx.h"
#include "ArcFaceEngine.h"
#include <iostream>

#include <io.h>
#include <vector>
#include <string>
#include <mutex>
using namespace std;
#define FACE_FEATURE_SIZE 1032

struct faceResult
{
	int faceID;
	string faceName;
	Rect faceRect;
};
class CFaceRecognition
{
public:
	CFaceRecognition();
	CFaceRecognition(string configPath,string registerPath);
	~CFaceRecognition();

	ArcFaceEngine m_imageFaceEngine;
	ArcFaceEngine m_videoFaceEngine;

	string m_configPath;//配置文件路径
	string m_registerPath;//注册图像地址

	bool init();//faceEngine初始化
	bool faceCompare(IplImage* image, vector<faceResult>& recogResults);
	bool release();//释放资源
	bool insertRegisterImage(string filename, IplImage* image);//增加一个注册图像
//	bool reloadAllRegisterFeatures();//重新加载所有注册图像特征
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
	mutex g_mutex;//互斥
	
	std::vector<ASF_FaceFeature> m_featuresVec;//注册人像特征集
	vector<std::string> jpg_files; //注册人像文件名
	vector<string> m_featuresName;//注册人像对应名字
	void ReadSettings();
	bool extractImagesFeature();//提取所有注册图像特征
	MRESULT extractFeature(IplImage* image,ASF_SingleFaceInfo& faceInfo, ASF_FaceFeature& faceFeature);//提取单幅图像特征
	bool compare(faceResult& faceRs, ASF_SingleFaceInfo& single_face, ASF_FaceFeature faceFeature);//单人脸对比及结果处理
};

int get_files(std::string fileFolderPath, std::string fileExtension, std::vector<std::string>&file);