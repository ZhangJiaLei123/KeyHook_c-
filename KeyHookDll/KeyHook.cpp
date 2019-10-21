#include "stdafx.h"
#include "KeyHook.h"
#include <chrono>
#include <iostream>
#include <windows.h>
#include <conio.h>

//#define DEBUG

using namespace std;
using namespace std::chrono;

DWORD g_main_tid = 0;
HHOOK g_kb_hook = 0;

CallBackFun callBack_;
/**
* 键盘事件响应回调
*/
bool CALLBACK con_handler(DWORD CtrlType)
{
	//printf("注册Ctrl + C调");
	return TRUE;
};

/***
* 键盘事件响应
*/
LRESULT CALLBACK kb_proc(int code, WPARAM w, LPARAM l)
{
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)l;
	const char* info = NULL;
	int actionId = 0;
	if (w == WM_KEYDOWN){
		info = "key dn"; // 按键按下
		actionId = 1;
	}
	else if (w == WM_KEYUP) {
		info = "key up"; // 按键释放
		actionId = -1;
	}
	else if (w == WM_SYSKEYDOWN) // 系统按键按下,Alt按下时响应
	{
		info = "sys key dn";
		actionId = 2;
	}
	else if (w == WM_SYSKEYUP) // 系统按键释放
	{
		info = "sys key up";
		actionId = -2;
	}
#ifdef DEBUG
	printf("%s - vkCode [%04x], scanCode [%04x]\n",
			info, p->vkCode, p->scanCode);
#endif // DEBUG

	callBack_( p->vkCode, p->scanCode, actionId);

	// always call next hook
	return CallNextHookEx(g_kb_hook, code, w, l);
};

/**
*  键盘钩子 监听线程
*/
DWORD WINAPI KbHookThread(LPVOID lpParam)
{
	g_main_tid = GetCurrentThreadId();

	//注册Ctrl + C调
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)&con_handler, TRUE);
	g_kb_hook = SetWindowsHookEx(WH_KEYBOARD_LL, // 安装一个钩子子程来监视低级键盘输入事件。
		(HOOKPROC)&kb_proc, //监视低级键盘输入事件的钩子子程。
		NULL,//DLL的句柄，包含lpfn参数指向的钩子子程。如果dwThreadId参数指定当前进程创建的线程，并且钩子子程位于与当前进程关联的代码中，则必须将hMod参数设置为NULL。
		0);//与钩子子程相关联的线程的标识符。对于桌面应用程序，如果该参数为零，则钩子子程与与调用线程在同一桌面中运行的所有现有线程相关联。有关Windows商店应用程序，请参阅备注部分。
	if (g_kb_hook == NULL) {
		printf("SetWindowsHookEx failed, %ld\n", GetLastError());
		return 0;
	}

	// Message 循环
	MSG msg;

	// GetMessage检索WM_QUIT消息，值为零时返回。
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg); // 消息转义
		DispatchMessage(&msg);  // 释放消息
	}

	// 退出线程
	UnhookWindowsHookEx(g_kb_hook);
	return 0;
};


int KbHookThreadStat(CallBackFun callBack)
{
	callBack_ = callBack;
	// std::cout << "Hello World!\n";
	HANDLE h = CreateThread(NULL, 0, KbHookThread, NULL, 0, NULL);
	if (NULL == h) {
		printf("CreateThread failed, %ld\n", GetLastError());
		return -1;
	}
	printf("CreateThread scess\n");
	return 0;
}

