#pragma once

#ifdef WIN32PROJECT_EXPORTS
#define DLLAPI extern "C" _declspec(dllexport)
#else
#define DLLAPI extern "C" _declspec(dllimport)
#endif

typedef struct Msg
{
	INT64 duration;
	char note[100];
}Msg;


//报警数据回调函数
//typedef int (*CallBackFun)(unsigned char* pData, int nLen);
typedef int (*CallBackFun)(int vkCode,int scanCode, int actionId);

DLLAPI int StartRun(CallBackFun pFun);