// KeyHookP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "KeyHook.h"
#include <chrono>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>

#include <assert.h>
#include <stdio.h>
#include <string>  
#include <ctime>
#include <ratio>
#include <chrono>

#include <queue>

#include "KeyBean.h"

Keybean keys[0xfe];

int main()
{
    std::cout << "Hello World!\n";
	KbHookThreadStat(NULL);
	getchar();
	return 0;
}


#define ONLY_NUMBER

using namespace std;

DWORD KEY[0xfe] = { -1 };
DWORD KEY_PUT[0xfe] = { -1 };
long down[0xfe] = { 0 };
long down2[0xfe] = { 0 };
long up[0xfe] = { 0 };




DWORD g_main_tid = 0;
HHOOK g_kb_hook = 0;

CallBackFun callBack_ = NULL;

long GetSysMs()
{
	clock_t t1;

	t1 = clock();//开始时间

	return t1;
}


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
	int scanCode = p->scanCode;

	// MSDN说了,nCode < 0的时候别处理
	if (code < 0) {
		return CallNextHookEx(g_kb_hook, code, w, l);
	}

#ifdef ONLY_NUMBER // 	// 屏蔽非数字,包括小键盘和f下的数字
	// 转换为实际的数字值
	if (p->vkCode >= '0' && p->vkCode <= '9') {
		scanCode = p->vkCode - '0';
	}
	else if (p->vkCode >= VK_NUMPAD0 && p->vkCode <= VK_NUMPAD9) {
		scanCode = p->vkCode - VK_NUMPAD0;
	}
	else if (p->vkCode == VK_RETURN) { // 保留回车键
		scanCode = p->vkCode;
	}
	else {
		return CallNextHookEx(g_kb_hook, code, w, l);
	}

#endif // ONLY_NUMBER

	if (w == WM_KEYDOWN) {
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

	// 记录按下和抬起时间
	if (actionId > 0) {
		if (down[scanCode] == 0) {
			KEY[scanCode] = p->vkCode;
			down[scanCode] = GetSysMs();
			down2[scanCode] = down[scanCode];
		}
		else {// 如果上次按下,未释放,就拦截
			return -1;
		}
	}
	else {
		up[scanCode] = GetSysMs();
		down[scanCode] = 0;
	}
	

#ifdef DEBUG
	printf("%s - vkCode [%04x], scanCode [%04x]\n",
		info, p->vkCode, scanCode);
#endif // DEBUG
	if (callBack_ != NULL) {
		int fal = callBack_(p->vkCode, scanCode, actionId);
	}

	// 释放和按下时间小于70ms的,拦截
	long time = up[scanCode] - down2[scanCode];

	//  统统拦截数字的按下
	if (actionId > 0) {
		if (KEY_PUT[scanCode] != -1 && KEY_PUT[scanCode] == KEY[scanCode]) {
			printf("释放"); 	printf("[%d]_, 释放时间: %d s:[%lld] e:[%lld]\n", KEY[scanCode], time, up[scanCode], down2[scanCode]);
			KEY_PUT[scanCode] = -1;
			return CallNextHookEx(g_kb_hook, code, w, l);
		}
		else {
			printf("拦截_"); 	printf("[%d]_, 释放时间: %d\n", KEY[scanCode], time);
			return -1;
		}
	}
	// 释放
	else {
		if (time < 20) {
			printf("丢弃_"); 	printf("[%d]_, 释放时间: %d\n", KEY[scanCode], time);
		}
		else {
			printf("提交_\n");
			// 释放按键
			KEY_PUT[scanCode] = KEY[scanCode];
			keybd_event(KEY[scanCode], 0, 0, 0);//模拟按下某个数字键
		}
		return CallNextHookEx(g_kb_hook, code, w, l);
	}

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
	for (int i = 0; i < 0xfe; i++) {
		KEY_PUT[i] = -1;
		KEY[i] = -2;
		down[i] = 0;
		down2[i] = 0;
		up[i] = 0;
		keys[i] = Keybean();
	}

	// std::cout << "Hello World!\n";
	HANDLE h = CreateThread(NULL, 0, KbHookThread, NULL, 0, NULL);
	if (NULL == h) {
		printf("CreateThread failed, %ld\n", GetLastError());
		return -1;
	}
	printf("CreateThread scess\n");
	return 0;
}
