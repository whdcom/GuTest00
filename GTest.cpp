#define whd "whd"
#define WINDOWS_PLATFORM
#include <common_type.h>
#include <ModuleSDK.h>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <format>
#include <time.h>
#include <sys/timeb.h>
#define BUF_SIZE 6



void saveTempMatrix(std::string dir, short* pY16Data, const int& iWidth, const int& iHeight);
HANDLE_T  g_Module = 0;
int flat = 1;
//���ɵ��¶Ⱦ��󱣴�Ĺ̶�·��
std::string tempDir = "D:\\vs\\GuTest00\\tempData";

//�������ԭʼ���ݣ�ͨ��ԭʼ�����������㣬ͨ��y16���ݽ��в��£�
//ͬʱҲ����ͨ��ԭʼy16�õ���ߺ���͵�y16ֵ���������º�����£�����ȫ֡���¼�����������
//�˴�����Ե������ڶ����߳���ִ�У���ֹ��cbFuncHandle��ִ�У�Ӱ��ص���������
void handleY16Data(short* pY16Data, const int& iLen, const int& iWidth, const int& iHeight) {
	if (iLen != iWidth * iHeight) {
		std::cout << "invalid data size ,drop it!" << std::endl;
		return;
	}
	//ģ������
	int iPosX = 100;
	int iPosY = 60;
	/*int W = iWidth;
	int H = iHeight;*/
	if (pY16Data && g_Module) {
		int iPosMeasure = iWidth * iPosY + iPosX;
		//for (int i = 0; i < iHeight; i++) {
		//	tempMatrix[i] = new float[iHeight];
		//}
		if (iPosMeasure < iWidth * iHeight) {
			short iPosY16 = pY16Data[iPosMeasure];
			//std::cout << "iPosY16 point error " << iPosY16 << std::endl;
			float fSurfaceT = 0;
			//���������£���Ҫ������¶�ӳ�䵽�����¶�
			if (MeasureTempByY16(g_Module, iPosY16, &fSurfaceT) != GUIDEIR_OK) {
				std::cout << "measure point error , posx : " << iPosX << " posy: " << iPosY << std::endl;
				return;
			}
			else {
				//���������£���Ҫ������¶�ӳ�䵽�����¶�
				float env;
				MeasureParamsControl(g_Module, GET_AMBIENT_TEMP, &env);
				float fBodyT = 0;
				if (MeasureBodyTemp(g_Module, fSurfaceT, env, &fBodyT) == GUIDEIR_OK) {
					printf("surface temperature:%f body temperature:%f env:%f \n", fSurfaceT, fBodyT, env);
					printf("\n\n\n\n\n\n");
				}
				int fps;
				if (ImageParamsControl(g_Module, GET_FRAME_RATE, &fps) != GUIDEIR_OK) {
					std::cout << "!!!GET FPS ERROR!!!" << std::endl;
					return;
				}
				else {
					std::cout << "FPS IS "<< fps<< std::endl;
				}
				saveTempMatrix(tempDir, pY16Data, iWidth, iHeight);
			}
		}
	}
}
//����ʵʱ�¶Ⱦ��󣬱������ļ�-��ǰϵͳ����.txt��
void saveTempMatrix(std::string dir, short* pY16Data, const int& iWidth, const int& iHeight) {
	if (dir.empty()){
		return;
	}
	char pStrPath1[256];
	//char *m = new char[147457];
	//std::string s = "27.891";
	//��ȡϵͳ��ǰ��ʱ��
	std::time_t currTime = time(NULL);
	struct tm mt;
	localtime_s(&mt ,&currTime);
	//��ȡ����ʱ���
	struct timeb tb; 
	ftime(&tb);
	sprintf_s(pStrPath1,256, "\\%d%02d%02d%02d%02d%02d%04d.txt", mt.tm_year + 1900, mt.tm_mon + 1, mt.tm_mday, mt.tm_hour, mt.tm_min, mt.tm_sec, tb.millitm);
	std::string filename = dir + pStrPath1;
	std::ofstream file;
	/*std::ofstream file;
	file.open("D:\\vs\\tmepMaxtrix-6.10-01.txt");
	*/
	file.open(filename);
	float* tempMatrix = (float*)malloc(iHeight * iWidth * sizeof(float));
	//memset(tempMatrix, 0.0, sizeof(int) * iHeight * iWidth);
	float* tempMatrixColumn = (float*)malloc( iWidth * sizeof(float));
	/*printf("tempMatrix is:\n");
	for (int i = 0; i < W * H; i++) {
		printf("%f ", tempMatrix[i]);
		if ((i + 1) % W == 0) {
			printf("\n");
		}
	}*/
	//�����¶Ⱦ���
	if (MeasureTempMatrix(g_Module, pY16Data, 0, 0, iWidth, iHeight, tempMatrix) != GUIDEIR_OK) {
		std::cout << "measure tempMatrix error  " << std::endl;
		return;
	}
	else {
		/*printf("tempMatrix is:\n");
		for (int i = 0; i < W * H; i++) {
			printf("%.2f ", tempMatrix[i]);
			if ((i + 1) %W == 0) {
				printf("\n");
			}
		}*/
		/*for (int i = 0, k = 0; i < iHeight * iWidth; i++) {
			char temp[30];
			sprintf_s(temp, 30, "%.3f", tempMatrix[i]);
			m[k] = temp[0];
			m[k + 1] = temp[1];
			m[k + 2] = temp[2];
			m[k + 3] = temp[3];
			if (i!= (iHeight * iWidth)-1) {
				m[k + 4] = ',';
			}
			k = k + 5;
		}*/
		//std::cout << "m is" << m << std::endl;


		//new�����֮��Ҫ�ͷ�
		/*delete[] m;
		m = NULL;
		delete[] tempMatrix;
		tempMatrix = NULL;
		delete[] tempMatrixColumn;
		tempMatrixColumn = NULL;*/
		free(tempMatrix);
		//free(tempMatrixColumn);
		//sprintf_s(m, 25, "%d", tempMatrix[0]);


		for (int i = 0, k = 0; i < iHeight * iWidth; i++) {
			tempMatrixColumn[k] = tempMatrix[i];
			file << std::setprecision(2) << std::fixed << tempMatrixColumn[k] << ",";
			k++;
			if ((i + 1) % iWidth == 0) {
				file << std::endl;
				k = 0;
			}
		}
		}
	}





//�������ʵʱ��Ƶ���ݣ����ݸ�ʽΪyuv422�����ڵ��ö���ʾʹ�ã�
//ͬʱҲ���Ը���ʵ���������ʵʱ�����л����Զ�����Ϣ
//�˴�����Ե������ڶ����߳���ִ�У���������cbFuncHandle��ִ�У�Ӱ��ص���������
void handleYuvData(unsigned char* pYuvData, const int& iLen, const int& iWidth, const int& iHeight) {
	//�û�����ʹ��yuv422����ʵʱ������Ʋ���
	return;
}

//ע��ص��������ݻص�����
int cbFuncHandle(CallBackData cbData, void* param) {
	//ʵʱ������²���
	if (cbData.yuvData) {
		handleYuvData(cbData.yuvData, cbData.yuvLength, cbData.width, cbData.height);
		printf("yuvLength=%d\n", cbData.yuvLength);
	}
	//ʵʱy16�������ݸ��²���
	if (cbData.y16Data) {
		handleY16Data(cbData.y16Data, cbData.y16Length, cbData.width, cbData.height);
	}
	//ÿ3���һ��
	// 
	//std::this_thread::sleep_for(std::chrono::milliseconds(40));
	//while (flat) {
	//	//�û���������
		//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	//}
	return 0;
}

int main()
{
	int iWidth = 120;
	int iHight = 90;
	int isDebugging = 0;
	/******************************��ʼ��sdkģ��******************************/
	g_Module = CreateModuleInstance(whd);
	MeasureParamsControl(g_Module, DEBUGGING_PATH, (void*)"D:/vs/GuTest/x64/Debug/debug.log");
	MeasureParamsControl(g_Module, SET_DEBUGGING, &isDebugging);
	RegisterImgCallBack(g_Module, cbFuncHandle, nullptr, (int)1);
	if (g_Module) {
		/******************************���ò��²���******************************/
		//���ò��²������������������Ϊ0.8��
		float fDistance = 0.8;
		if (MeasureParamsControl(g_Module, SET_DISTANCE, (void*)&fDistance) != GUIDEIR_OK) {
			std::cout << "set distance error!" << std::endl;
		}
		else {
			std::cout << "set distance is"<<fDistance << std::endl;
		}
		//���ò��²����������¶Ȳ���������Ϊ25���϶�
		float fEnvironment = 25.0;
		if (MeasureParamsControl(g_Module, SET_AMBIENT_TEMP, (void*)&fEnvironment) != GUIDEIR_OK) {
			std::cout << "set environment error!" << std::endl;
		}
		else {
			std::cout << "set environment is"<< fEnvironment << std::endl;
		}
		//���ó����ʽ
		int format = 1;
		if (ImageParamsControl(g_Module, SET_FORMAT, &format)!= GUIDEIR_OK) {
			std::cout << "set format error!" << std::endl;
		}
		else {
			std::cout << "set format is" << format << std::endl;
		}
		////���ÿ���
		//int shutter = 0;
		//if (ImageParamsControl(g_Module, SET_FORMAT, &shutter) != GUIDEIR_OK) {
		//	std::cout << "set shutter error!" << std::endl;
		//}
		//else {
		//	std::cout << "set format is" << shutter << std::endl;
		//}
	}

	/******************************�û��߳���������******************************/
	while (true) {
		//�û���������
		std::this_thread::sleep_for(std::chrono::seconds(1));

	}
	/******************************�ͷ�sdkģ��******************************/
	//�����˳�ʱ����
	if (g_Module) {
		DestroyModuleInstance(g_Module);
		g_Module = 0;
	}
	//DestroyModuleInstance(g_Module);
	return 0;
}

int Gtest1()
{
	return 0;
}
