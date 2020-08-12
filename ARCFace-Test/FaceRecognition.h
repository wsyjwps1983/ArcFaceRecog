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

	string m_configPath;//�����ļ�·��
	string m_registerPath;//ע��ͼ���ַ

	bool init();//faceEngine��ʼ��
	bool faceCompare(IplImage* image, vector<faceResult>& recogResults);
	bool release();//�ͷ���Դ
	bool insertRegisterImage(string filename, IplImage* image);//����һ��ע��ͼ��
//	bool reloadAllRegisterFeatures();//���¼�������ע��ͼ������
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
	mutex g_mutex;//����
	
	std::vector<ASF_FaceFeature> m_featuresVec;//ע������������
	vector<std::string> jpg_files; //ע�������ļ���
	vector<string> m_featuresName;//ע�������Ӧ����
	void ReadSettings();
	bool extractImagesFeature();//��ȡ����ע��ͼ������
	MRESULT extractFeature(IplImage* image,ASF_SingleFaceInfo& faceInfo, ASF_FaceFeature& faceFeature);//��ȡ����ͼ������
	bool compare(faceResult& faceRs, ASF_SingleFaceInfo& single_face, ASF_FaceFeature faceFeature);//�������Աȼ��������
};

int get_files(std::string fileFolderPath, std::string fileExtension, std::vector<std::string>&file);