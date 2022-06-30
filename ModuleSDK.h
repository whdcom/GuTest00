/*************************************************
Copyright (C), 2020--2030, GuideIR Tech. Co., Ltd.
File name	: ModuleSDK.h
Author		: Guide Sensmart RD
Version		: 1.0
Date		: 2020/9/6
Description	:
*************************************************/
#ifndef MODULE_SDK_H
#define MODULE_SDK_H

#include "common_type.h"

#ifdef __cplusplus
#    define MODULE_SDK_START  extern "C" {
#    define MODULE_SDK_END    };
#else
#    define MODULE_SDK_START
#    define MODULE_SDK_END
#endif

MODULE_SDK_START

#ifdef WINDOWS_PLATFORM
#    define MODULEAPI __declspec(dllexport)
#else
#    define MODULEAPI
#endif // WINDOWS_PLATFORM

typedef enum
{
	SET_DISTANCE = 1,		//距离0.5~2.5米。可以不设置，默认1.5米。参数类型：浮点型。
	GET_DISTANCE,
	SET_AMBIENT_TEMP,	    //环境温度在用户不设置的情况下，内部自动计算环温。如果用户设置，则使用设置的值。参数类型：浮点型。
	GET_AMBIENT_TEMP,       //获取环温。
	GET_FPA_TEMP,			//获取焦平面温度。参数类型：浮点型。
	RESET_NOW,				//设备重启。
	HEART_BEATING_ENABLE,	//开启心跳功能。心跳间隔时间通常5~15秒，如果参数设置为0则默认5秒。参数类型：整形。
	HEART_BEATING_DISABLE,	//关闭心跳功能。
	SET_DEBUGGING,		    //设置是否开启调试。参数类型：整型。0，关闭调试。1，开启调试。信息输出到日志文件debug_out.log。日志文件默认保存在程序同级目录下。
	DEBUGGING_PATH,	        //设置日志保存的完整路径。注意要在RegisterImgCallBack之前调用。必须是已存在的目录，例如/home/app/debug.log。Windows例如"D:/Tools/debug.log"。长度不要超过128。参数类型：字符串，以 \0 结束。
	GRAB_FRAME,				//开始录X16，Y16数据。每秒录一帧。录满8MB后停止。
	SNAPSHOT,				//拍照保存当前这帧图像，存储在程序的同级目录下。
}PARAMETER_TYPE;

typedef enum
{
	SET_FORMAT = 1,		//设置成像格式。0,YUV422_YUYV. 1,YUV422_UYVY. 2,YUV422_Plane. 3,YVU422_Plane. 20,RGB888，默认格式。参数类型：整型。
	SHUTTER_NOW,		//打快门
	SET_AUTO_SHUTTER,	//0，关闭自动快门。1，打开自动快门。默认打开自动快门。
	SET_SHUTTER_TIME,	//设置自动快门时间，单位秒。默认30秒。
	SET_PALETTE,		//设置伪彩，范围0-10. 默认0.
	GET_PALETTE,
	GET_SCALE,			//获取缩放倍数。RegisterImgCallBack时设置缩放倍数。
	SET_CONTRAST,		//设置对比度。对比度0-255。默认255. 参数类型：整形。
	GET_CONTRAST,
	SET_BRIGHTNESS,		//设置亮度。亮度0-255。默认70. 参数类型：整形。
	GET_BRIGHTNESS,
	SET_ROTATION,		//设置旋转方式，默认0。0，不旋转。1，旋转90度。2，旋转180度。3，旋转270度。
	GET_ROTATION,		//设置旋转后RGB和Y16数据均旋转。
	SET_FLIP,			//设置是否左右翻转，默认0。0，不翻转。1，左右翻转。
	GET_FLIP,			//设置翻转后RGB和Y16数据均翻转。
	SET_FRAME_RATE,		//设置帧率
	GET_FRAME_RATE,		//获取实际帧率
}IMG_PARAM_TYPE;

typedef struct OUT_PUT_IR_DATA {
	int width;
	int height;
	unsigned char *yuvData; //由SET_FORMAT指定输出格式。
	int yuvLength;
	short *y16Data;
	int y16Length;
}CallBackData;

typedef int (*ImageCallBack)(CallBackData callBackData, void *param);

typedef struct
{
	char serialNum[32];
	char armVersion[32];
	char fpgaVersion[32];
	char sdkVersion[32];
}FACTORY_VERSION;

/**
* @brief:	创建模组实例。
* @param:	const char *name			 模组名称
* @return:	返回实例句柄。
* @note:
**/
MODULEAPI HANDLE_T CreateModuleInstance(const char *name);

/**
* @brief:	销毁模组实例。
* @param:	HANDLE_T handle     实例句柄。
* @return:	无。
* @note:
**/
MODULEAPI void DestroyModuleInstance(HANDLE_T handle);

/**
* @brief:	注册成像回调函数，并设置缩放倍数。软件实现缩放会占用一定CPU，建议取到原始图像后利用硬件缩放。
* @param:	HANDLE_T handle				 实例句柄。
* @param:	ImageCallBack imageCallBack  图像回调函数。
* @param:	void *param			         回调函数预留参数。
* @param:	float scaleTime 	         缩放倍数。设置1，不缩放。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int RegisterImgCallBack(HANDLE_T handle, ImageCallBack imageCallBack, void *param, float scaleTime);

/**
* @brief:	根据Y16值计算体表温度。
* @param:	HANDLE_T handle     实例句柄。
* @param:	short y16           Y16值。
* @param:	float *surfaceTemp  输出体表温度。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureTempByY16(HANDLE_T handle, short y16, float *surfaceTemp);

/**
* @brief:	计算人体温度值。
* @param:	HANDLE_T handle     实例句柄。
* @param:	float surfaceTemp   体表温度。
* @param:	float envirTemp		环境温度。可以通过GET_AMBIENT_TEMP获取SDK计算的环温。注意受整机散热方面的影响，内部计算的环温与实际值有偏差，一般需要减去偏移量得到实际环温。
* @param:	float *bodyTemp     输出人体温度。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureBodyTemp(HANDLE_T handle, float surfaceTemp, float envirTemp, float *bodyTemp);

/**
* @brief:	计算温度矩阵。
* @param:	HANDLE_T handle     实例句柄。
* @param:	short* y16Array     Y16矩阵。
* @param:	x,y,w,h				目标矩形区域
* @param:	float *tempMatrix   输出温度矩阵。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureTempMatrix(HANDLE_T handle, short* y16Array, int x, int y, int w, int h, float *tempMatrix);

/**
* @brief:	计算温度矩阵，带距离参数。
* @param:	HANDLE_T handle     实例句柄。
* @param:	short* y16Array     Y16矩阵。
* @param:	float distance		距离参数
* @param:	x,y,w,h				目标矩形区域
* @param:	float *tempMatrix   输出温度矩阵。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureTempMatrixEx(HANDLE_T handle, short* y16Array, float distance, int x, int y, int w, int h, float *tempMatrix);

/**
* @brief:	设置测温参数。
* @param:	HANDLE_T handle				实例句柄。
* @param:	CMD_PARAMETER_TYPE type		参数类型。
* @param:	void *param					参数值，传地址。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureParamsControl(HANDLE_T handle, PARAMETER_TYPE type, void *param);

/**
* @brief:	设置成像参数。
* @param:	HANDLE_T handle				实例句柄。
* @param:	IMG_PARAM_TYPE type		    参数类型。
* @param:	void *param					参数值，传地址。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int ImageParamsControl(HANDLE_T handle, IMG_PARAM_TYPE type, void *param);

/**
* @brief:	固件升级。升级过程中，如果成功则设备会重启。用户在升级接口返回后，需要关闭设备，等待5秒，然后再重新打开设备。
* @param:	HANDLE_T handle				实例句柄。
* @param:	unsigned char *updateData	升级数据。
* @param:	int length              	参数值，传地址。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int FirmwareUpdate(HANDLE_T handle, unsigned char *updateData, int length);

/**
* @brief:  查询出厂信息，SN序列号、固件版本号和SDK版本号等。
* @param:  HANDLE_T handle				实例句柄。
* @param:  FACTORY_VERSION * info
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int FactoryVersion(HANDLE_T handle, FACTORY_VERSION *info);

//以下是扩展接口，无相关需求时可以不用关注。
typedef enum
{
	GET_SHUTTER_TEMP = 1,			//获取快门温度。参数类型：浮点型。
	GET_LENS_TEMP,					//获取镜筒温度。参数类型：浮点型。
	DO_NUC,
}EXPERT_PARAM_TYPE;

MODULEAPI int ExpertParamsControl(HANDLE_T handle, EXPERT_PARAM_TYPE type, void *param);

MODULE_SDK_END

#endif // !MODULE_SDK_H

