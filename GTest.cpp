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
//生成的温度矩阵保存的固定路径
std::string tempDir = "D:\\vs\\GuTest00\\tempData";

//处理红外原始数据，通过原始数据针对任意点，通过y16数据进行测温，
//同时也可以通过原始y16得到最高和最低的y16值，算出最高温和最低温，避免全帧测温计算最高最低温
//此处理可以单独放在独立线程中执行，防止在cbFuncHandle中执行，影响回调函数返回
void handleY16Data(short* pY16Data, const int& iLen, const int& iWidth, const int& iHeight) {
	if (iLen != iWidth * iHeight) {
		std::cout << "invalid data size ,drop it!" << std::endl;
		return;
	}
	//模拟点测温
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
			//针对人体测温，需要将体表温度映射到人体温度
			if (MeasureTempByY16(g_Module, iPosY16, &fSurfaceT) != GUIDEIR_OK) {
				std::cout << "measure point error , posx : " << iPosX << " posy: " << iPosY << std::endl;
				return;
			}
			else {
				//针对人体测温，需要将体表温度映射到人体温度
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
//生成实时温度矩阵，保存在文件-当前系统日期.txt中
void saveTempMatrix(std::string dir, short* pY16Data, const int& iWidth, const int& iHeight) {
	if (dir.empty()){
		return;
	}
	char pStrPath1[256];
	//char *m = new char[147457];
	//std::string s = "27.891";
	//获取系统当前的时间
	std::time_t currTime = time(NULL);
	struct tm mt;
	localtime_s(&mt ,&currTime);
	//获取毫秒时间段
	struct timeb tb; 
	ftime(&tb);
	sprintf_s(pStrPath1,256, "\\%d%02d%02d%02d%02d%02d%04d.txt", mt.tm_year + 1900, mt.tm_mon + 1, mt.tm_mday, mt.tm_hour, mt.tm_min, mt.tm_sec, tb.millitm);
	std::string filename = dir + pStrPath1;
	std::ofstream file;
	/*std::ofstream file;
	file.open("D:\\vs\\tmepMaxtrix-6.10-01.txt");
	*/
	file.open(filename);
	float* tempMatrix = nullptr;
	tempMatrix = (FLOAT_T *) malloc ((sizeof(FLOAT_T)) *iHeight * iWidth );
	if (tempMatrix) {
		//memset(tempMatrix, 0.000, sizeof(float) * iHeight * iWidth);
		//float* tempMatrixColumn = (float*)malloc( iWidth * sizeof(float));
		/*printf("tempMatrix is:\n");
		for (int i = 0; i < W * H; i++) {
			printf("%f ", tempMatrix[i]);
			if ((i + 1) % W == 0) {
				printf("\n");
			}
		}*/
		//计算温度矩阵
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


			//new完对象之后要释放
			/*delete[] m;
			m = NULL;
			delete[] tempMatrix;
			tempMatrix = NULL;
			delete[] tempMatrixColumn;
			tempMatrixColumn = NULL;*/
			//free(tempMatrixColumn);
			//sprintf_s(m, 25, "%d", tempMatrix[0]);


			if (tempMatrix) {
				for (int i = 0, k = 0; i < iHeight * iWidth; i++) {
					file << std::setprecision(2) << std::fixed << tempMatrix[i] << ",";
					k++;
					if ((i + 1) % iWidth == 0) {
						file << std::endl;
						k = 0;
					}
				}
			}
		}
		free(tempMatrix);
	}
	}





//处理红外实时视频数据，数据格式为yuv422，用于调用端显示使用，
//同时也可以根据实际情况，在实时画面中绘制自定义信息
//此处理可以单独放在独立线程中执行，避免在在cbFuncHandle中执行，影响回调函数返回
void handleYuvData(unsigned char* pYuvData, const int& iLen, const int& iWidth, const int& iHeight) {
	//用户可以使用yuv422进行实时画面绘制操作
	return;
}

//注册回调红外数据回调函数
int cbFuncHandle(CallBackData cbData, void* param) {
	//实时画面更新操作
	if (cbData.yuvData) {
		handleYuvData(cbData.yuvData, cbData.yuvLength, cbData.width, cbData.height);
		printf("yuvLength=%d\n", cbData.yuvLength);
	}
	//实时y16测温数据更新操作
	if (cbData.y16Data) {
		handleY16Data(cbData.y16Data, cbData.y16Length, cbData.width, cbData.height);
	}
	//每3秒测一次
	// 
	//std::this_thread::sleep_for(std::chrono::milliseconds(40));
	//while (flat) {
	//	//用户其他操作
		//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	//}
	return 0;
}

int main()
{
	int iWidth = 120;
	int iHight = 90;
	int isDebugging = 0;
	/******************************初始化sdk模块******************************/
	g_Module = CreateModuleInstance(whd);
	MeasureParamsControl(g_Module, DEBUGGING_PATH, (void*)"D:/vs/GuTest/x64/Debug/debug.log");
	MeasureParamsControl(g_Module, SET_DEBUGGING, &isDebugging);
	RegisterImgCallBack(g_Module, cbFuncHandle, nullptr, (int)1);
	if (g_Module) {
		/******************************设置测温参数******************************/
		//设置测温参数，距离参数，设置为0.8米
		float fDistance = 0.8f;
		if (MeasureParamsControl(g_Module, SET_DISTANCE, (void*)&fDistance) != GUIDEIR_OK) {
			std::cout << "set distance error!" << std::endl;
		}
		else {
			std::cout << "set distance is"<<fDistance << std::endl;
		}
		//设置测温参数，环境温度参数，设置为25摄氏度
		float fEnvironment = 25.0;
		if (MeasureParamsControl(g_Module, SET_AMBIENT_TEMP, (void*)&fEnvironment) != GUIDEIR_OK) {
			std::cout << "set environment error!" << std::endl;
		}
		else {
			std::cout << "set environment is"<< fEnvironment << std::endl;
		}
		//设置成像格式
		int format = 1;
		if (ImageParamsControl(g_Module, SET_FORMAT, &format)!= GUIDEIR_OK) {
			std::cout << "set format error!" << std::endl;
		}
		else {
			std::cout << "set format is" << format << std::endl;
		}
		////设置快门
		//int shutter = 0;
		//if (ImageParamsControl(g_Module, SET_FORMAT, &shutter) != GUIDEIR_OK) {
		//	std::cout << "set shutter error!" << std::endl;
		//}
		//else {
		//	std::cout << "set format is" << shutter << std::endl;
		//}
	}

	/******************************用户线程其他操作******************************/
	while (true) {
		//用户其他操作
		std::this_thread::sleep_for(std::chrono::seconds(1));

	}
	/******************************释放sdk模块******************************/
	//程序退出时调用
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
