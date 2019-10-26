// 接口声明
#pragma once

#ifdef WIN32PROJECT_EXPORTS
#define DLLAPI extern "C" _declspec(dllexport)
#else
#define DLLAPI extern "C" _declspec(dllimport)
#endif

//报警数据回调函数
//typedef int (*CallBackFun)(unsigned char* pData, int nLen);
typedef int (*CallBackFun)(int vkCode,int scanCode, int actionId);

// 开启键盘监听
DLLAPI int StartpKeyHook(CallBackFun pFun);
// 关闭键盘监听
DLLAPI int StopKeyHook();
// 设置键盘拦截使能
DLLAPI int SetKbHookLock(int value);
// 发送模拟按键
DLLAPI int SendKey(int keyCode);