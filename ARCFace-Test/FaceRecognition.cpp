#include "stdafx.h"
#include "FaceRecognition.h"


CFaceRecognition::CFaceRecognition()
{
	m_configPath = ".\\setting.ini";
	m_registerPath = ".\\registerFaces";
}
CFaceRecognition::CFaceRecognition(string configPath,string registerPath)
{
	m_configPath = configPath;
	m_registerPath = registerPath;
}

CFaceRecognition::~CFaceRecognition()
{
}

void CFaceRecognition::ReadSettings()
{

	//const char ConfigFile[] = m_configPath.c_str();//".\\setting.ini";
	Config configSettings(m_configPath);
	m_appID = configSettings.Read("APPID", m_appID);
	m_tag = configSettings.Read("tag", m_tag);
	m_sdkKey = configSettings.Read("SDKKEY", m_sdkKey);
	m_activeKey = configSettings.Read("ACTIVE_KEY", m_activeKey);
	m_rgbLiveThreshold = configSettings.Read("rgbLiveThreshold", 0.0);
	m_irLiveThreshold = configSettings.Read("irLiveThreshold", 0.0);
	m_rgbCameraId = configSettings.Read("rgbCameraId", 0);
	m_irCameraId = configSettings.Read("irCameraId", 0);
}

bool CFaceRecognition::init()
{
	ReadSettings();
	MRESULT faceRes = m_imageFaceEngine.ActiveSDK(const_cast<char*>(m_appID.data()), const_cast<char*>(m_sdkKey.data()), const_cast<char*>(m_activeKey.data()));
	cout << "激活结果: " << faceRes << endl;

	//获取激活文件信息
	ASF_ActiveFileInfo activeFileInfo = { 0 };
	m_imageFaceEngine.GetActiveFileInfo(activeFileInfo);

	if (faceRes == MOK)
	{
		faceRes = m_imageFaceEngine.InitEngine(ASF_DETECT_MODE_IMAGE);//Image
		cout << "IMAGE模式下初始化结果: " << faceRes << endl;
		
		faceRes = m_videoFaceEngine.InitEngine(ASF_DETECT_MODE_VIDEO);//Video
		cout << "Video模式下初始化结果: " << faceRes << endl;

		faceRes = extractImagesFeature();
	}
	return faceRes == MOK ? true : false;
}


bool CFaceRecognition::release()
{
	m_featuresVec.clear();
	MRESULT faceRs = m_imageFaceEngine.UnInitEngine();
	faceRs = m_videoFaceEngine.UnInitEngine();
	return faceRs == MOK ? true : false;
}
/*
提取单幅图像特征
*/
MRESULT CFaceRecognition::extractFeature(IplImage* image,ASF_SingleFaceInfo& faceInfo, ASF_FaceFeature& faceFeature)
{
	
	//FD
	
	MRESULT detectRes = m_imageFaceEngine.PreDetectFace(image, faceInfo, true);
	if (MOK != detectRes)
	{
		cvReleaseImage(&image);
		return detectRes;
	}
	//FaceFeature extract
	
	detectRes = m_imageFaceEngine.PreExtractFeature(image, faceFeature, faceInfo);

	if (MOK != detectRes)
	{
		free(faceFeature.feature);
		cvReleaseImage(&image);
		return detectRes;
	}
	return detectRes;
}
/*
提取所有注册图像特征
*/
bool CFaceRecognition::extractImagesFeature()
{
	MRESULT faceRes = MOK;
	//	vector<std::string> jpg_files;
	get_files(m_registerPath, ".jpg", jpg_files);
	for (int i = 0; i < jpg_files.size(); i++)
	{
		cout << jpg_files[i] << endl;
		IplImage* image = cvLoadImage(jpg_files[i].c_str());
		if (!image)
		{
			cvReleaseImage(&image);
			continue;
		}
		ASF_FaceFeature faceFeature = { 0 };
		faceFeature.featureSize = FACE_FEATURE_SIZE;
		faceFeature.feature = (MByte *)malloc(faceFeature.featureSize * sizeof(MByte));

		ASF_SingleFaceInfo faceInfo = { 0 };
		faceRes = extractFeature(image,faceInfo, faceFeature);

		if (MOK != faceRes)
		{
			cvReleaseImage(&image);
			continue;
		}
		string nameStr = jpg_files[i];
		nameStr = nameStr.erase(0, nameStr.rfind("\\") + 1);

		m_featuresName.push_back(nameStr.erase(nameStr.rfind("."), nameStr.size()));
		m_featuresVec.push_back(faceFeature);
		cvReleaseImage(&image);
	}
	return faceRes == MOK ? true : false;
}
bool CFaceRecognition::insertRegisterImage(string filename,IplImage* image)
{
	ASF_FaceFeature faceFeature = { 0 };
	faceFeature.featureSize = FACE_FEATURE_SIZE;
	faceFeature.feature = (MByte *)malloc(faceFeature.featureSize * sizeof(MByte));

	ASF_SingleFaceInfo faceInfo = { 0 };
	MRESULT faceRes = extractFeature(image, faceInfo, faceFeature);

	if (MOK != faceRes)
	{
		return false;
	}
	m_featuresName.push_back(filename.erase(filename.rfind("."), filename.size()));
	m_featuresVec.push_back(faceFeature);
}
//特征匹配进行识别，返回注册人员信息及未注册提示
bool CFaceRecognition::compare(faceResult& faceRs, ASF_SingleFaceInfo& single_face, ASF_FaceFeature faceFeature)
{
	faceRs = { 0 };
	int curIndex = 0;
	int maxIndex = 0;
	MFloat maxThreshold = 0.0;
	for each (auto regisFeature in m_featuresVec)
	{
		MFloat confidenceLevel = 0;
		MRESULT pairRes = m_videoFaceEngine.FacePairMatching(confidenceLevel, faceFeature, regisFeature);

		if (MOK == pairRes && confidenceLevel > maxThreshold)
		{
			maxThreshold = confidenceLevel;
			maxIndex = curIndex;
		}
		curIndex++;
	}
	if (maxThreshold >= m_rgbLiveThreshold)
	{
		cout << jpg_files[maxIndex] << "号 :" << maxIndex << "阈值" << maxThreshold << endl;
		faceRs.faceID = maxIndex;
		faceRs.faceName = m_featuresName[maxIndex];
		faceRs.faceRect.x = single_face.faceRect.left;
		faceRs.faceRect.y = single_face.faceRect.top;
		faceRs.faceRect.width = single_face.faceRect.right - single_face.faceRect.left;
		faceRs.faceRect.height = single_face.faceRect.bottom - single_face.faceRect.top;
		return true;
	}
	else
	{
		faceRs.faceID = maxIndex;
		faceRs.faceName = "NoRegist";
		faceRs.faceRect.x = single_face.faceRect.left;
		faceRs.faceRect.y = single_face.faceRect.top;
		faceRs.faceRect.width = single_face.faceRect.right - single_face.faceRect.left;
		faceRs.faceRect.height = single_face.faceRect.bottom - single_face.faceRect.top;
		return false;
	}
		
}
bool CFaceRecognition::faceCompare(IplImage* img,vector<faceResult>& recogResults)
{
	bool rs = false;
	IplImage* tempImage = NULL;
	lock_guard<mutex> lock(g_mutex);
	{		
		tempImage = cvCloneImage(img);
	}
	//检测人脸
	ASF_MultiFaceInfo faceInfo = { 0 };
	MRESULT detectRes = m_videoFaceEngine.PreDetectFaces(tempImage, faceInfo, true);
	if (MOK != detectRes)
	{
		return false;
	}
	//初始化特征
	ASF_FaceFeature faceFeature = { 0 };
	faceFeature.featureSize = FACE_FEATURE_SIZE;
	faceFeature.feature = (MByte *)malloc(faceFeature.featureSize * sizeof(MByte));
	for (int i = 0; i < faceInfo.faceNum; i++)
	{
		ASF_SingleFaceInfo single_face = {0};
		single_face.faceOrient = faceInfo.faceOrient[i];
		single_face.faceRect.top = faceInfo.faceRect[i].top;
		single_face.faceRect.left = faceInfo.faceRect[i].left;
		single_face.faceRect.bottom = faceInfo.faceRect[i].bottom;
		single_face.faceRect.right = faceInfo.faceRect[i].right;

		detectRes = m_videoFaceEngine.PreExtractFeature(tempImage,
			faceFeature, single_face);
		if (MOK != detectRes)
		{
			cvReleaseImage(&tempImage);
			return false;
		}
		faceResult faceRs = { 0 };
		compare(faceRs, single_face, faceFeature);
		recogResults.push_back(faceRs);	//不管是否匹配成功都需要返回，失败返回未注册	
		
	}
	
	cvReleaseImage(&tempImage);

	

	
	return true;
}





///////////////////////////////////////////////////////////////////////////////////////////////////
int get_files(std::string fileFolderPath, std::string fileExtension, std::vector<std::string>&file)
{
	std::string fileFolder = fileFolderPath + "\\*" + fileExtension;
	std::string fileName;
	struct _finddata_t fileInfo;
	long long findResult = _findfirst(fileFolder.c_str(), &fileInfo);
	if (findResult == -1)
	{
		_findclose(findResult);
		return 0;
	}
	bool flag = 0;

	do
	{
		fileName = fileFolderPath + "\\" + fileInfo.name;
		if (fileInfo.attrib == _A_ARCH)
		{
			file.push_back(fileName);
		}
	} while (_findnext(findResult, &fileInfo) == 0);

	_findclose(findResult);
}
