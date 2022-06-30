/*************************************************
Copyright (C), 2019-2029, GuideIR Tech. Co., Ltd.
File name	: common_type.h
Author		: Guide Sensmart RD
Version		: 1.0
Date		: 2019/7/6
Description	:
*************************************************/

#ifndef COMMON_TYPE_H
#define COMMON_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/* generic type redefinitions */
typedef int                 INT32_T;
typedef unsigned int        UINT32_T;
typedef short               INT16_T;
typedef unsigned short      UINT16_T;
typedef char                CHAR_T;
typedef unsigned char       BYTE_T;

typedef long                LONG_T;
typedef unsigned long       ULONG_T;
typedef unsigned long long  UINT64_T;
typedef long long           INT64_T;
typedef float               FLOAT_T;
typedef double              DOUBLE_T;
#ifdef WINDOWS_PLATFORM
typedef void *				HANDLE_T;	//兼容32bit和64bit操作系统
#else
typedef UINT64_T			HANDLE_T;
#endif // WINDOWS_PLATFORM
#define CONST				const
#define VOID				void

typedef enum
{
	FALSE_T = 0,
	TRUE_T = 1,
} BOOL_T;

/* general return values */
#define GUIDEIR_OK			(0)
#define GUIDEIR_ERR			(-1)
#define GUIDEIR_NULL		(0)//((VOID *)0)

#ifdef __cplusplus
}
#endif

#endif // !COMMON_TYPE_H

