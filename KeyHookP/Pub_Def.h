#pragma once

#ifdef WIN32PROJECT_EXPORTS
#define DLLAPI extern "C" _declspec(dllexport)
#else
#define DLLAPI extern "C" _declspec(dllimport)
#endif

//�������ݻص�����
//typedef int (*CallBackFun)(unsigned char* pData, int nLen);
typedef int (*CallBackFun)(int vkCode,int scanCode, int actionId);

DLLAPI int StartRun(CallBackFun pFun);
DLLAPI int KeyHookTest();
DLLAPI int MakeKeyHookUdp();
DLLAPI int StopKeyHookUdp();
DLLAPI int SendKey(int keyCode);