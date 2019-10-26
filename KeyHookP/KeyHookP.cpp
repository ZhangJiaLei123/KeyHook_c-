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
#include <iostream>   
#include <list>   
#include <numeric>   
#include <algorithm>   

#include "KeyBean.h"

bool isKbLock = false;
void starSendKey();

#define EWM_SIZE_DEF 12
EWM erm[EWM_SIZE_DEF] = { EWM{1,0}, EWM{1,1}, EWM{1,2}, EWM{1,3}, EWM{1,4}, EWM{1,5},
						 EWM{2,5}, EWM{2,6}, EWM{2,7}, EWM{2,8}, EWM{2,9}, EWM{3,0} };

int main()
{
    std::cout << "Hello World!\n";
	KbHookThreadStat(NULL);
	getchar(); 
	getchar();
	return 0;
}


#define ONLY_NUMBER

using namespace std;

DWORD KEY[0xfe] = { -1 };
DWORD KEY_PUT[0xfe] = { -1 };
time_t down[0xfe] = { 0 };
time_t down2[0xfe] = { 0 };
time_t up[0xfe] = { 0 };

time_t lastTime = 0;
time_t nowTime = 0;

DWORD g_main_tid = 0;
HHOOK g_kb_hook = 0;


queue<Keybean> keys;
typedef list<int> KEYQEUE;// 按键
typedef list<Keybean> KEYQEUE_IN;// 按键
KEYQEUE keyQeue;
KEYQEUE_IN keyIn;

CallBackFun callBack_ = NULL;

std::time_t getTimeStamp()
{
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	std::time_t timestamp = tmp.count();
	return timestamp;
}

time_t GetSysMs()
{
	clock_t t1;

	t1 = clock();//开始时间


	return getTimeStamp();
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

	if (isKbLock) {
		return CallNextHookEx(g_kb_hook, code, w, l);
	}

	// 回车键就提交
	if (p->vkCode == VK_RETURN) {
		printf("回车\n");
		if (!keyQeue.empty()) {
			starSendKey();
			return -1;
		}
		return CallNextHookEx(g_kb_hook, code, w, l);
	}


#ifdef ONLY_NUMBER // 	// 屏蔽非数字,屏蔽小键盘
	// 转换为实际的数字值
	if (p->vkCode >= '0' && p->vkCode <= '9') {
		scanCode = p->vkCode - '0';
	}
	//else if (p->vkCode >= VK_NUMPAD0 && p->vkCode <= VK_NUMPAD9) {
	//	scanCode = p->vkCode - VK_NUMPAD0;
	//}
	else if (p->vkCode == VK_RETURN) { // 保留回车键
		scanCode = p->vkCode;
	}
	else {
		if (!keyQeue.empty()) {
			keyQeue.clear();
		}
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
		// 记录按键
		//keys.push(Keybean(p));
		if (down[scanCode] == 0) {
			down[scanCode] = GetSysMs();
			down2[scanCode] = down[scanCode];
		}
		else {
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

	// 释放和按下时间小于70ms的,拦截
	time_t time = up[scanCode] - down2[scanCode];

	//  统统拦截数字的按下
	if (actionId > 0) {
		return -1;
	}
	// 释放
	else {
		if (time < 20) {
			// 将要丢弃的按键,推送到缓存中,交给丢弃线程处理
			keyQeue.push_back(p->vkCode);
		}
		else {
			//printf("提交_\n");
			isKbLock = true;
			keybd_event(p->vkCode, 0, 0, 0);//模拟按下某个数字键
			isKbLock = false;
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

int get(KEYQEUE _list, int _i) {
	if ( _list.empty() || _list.size() == 0) {
		return 0;
	}
	KEYQEUE::iterator it = _list.begin();
	for (int i = 0; i < _i; i++) {
		++it;
	}

	try
	{
		return *it;
	}
	catch (const std::exception&)
	{
		return 0;
	}

	return *it;
}

int toNumber(int n0) {
	int scanCode = 0;
	if (n0 >= '0' && n0 <= '9') {
		scanCode = n0 - '0';
	}
	else if (n0 >= VK_NUMPAD0 && n0 <= VK_NUMPAD9) {
		scanCode = n0 - VK_NUMPAD0;
	}
	return scanCode;
}


/* 键盘丢弃处理线程 */
DWORD WINAPI KbSendThread(LPVOID lpParam) {

	KEYQEUE keyQeue2 = keyQeue;

	//keyQeue2.push_back(keyQeue);

	// 关闭键盘拦截
	char n0 = get(keyQeue2, 0) - '0';
	char n1 = get(keyQeue2, 1) - '0';
	isKbLock = true;

	// 拦截支付码
	bool fal = false;
	for (int i = 0; i < EWM_SIZE_DEF; i++) {
		if (erm[i].n1 == n0 && erm[i].n2 == n1) {
			fal = true;
			//printf("丢弃_支付码\n");
			break;
		}
	}

	if (!fal) {
		//printf("继续提交其他条形码\n");
		// 继续提交其他条形码
		for (KEYQEUE::iterator i = keyQeue2.begin(); i != keyQeue2.end(); ++i) {
			if(*i != VK_RETURN)
				keybd_event( *i, 0, 0, 0);//模拟按下某个数字键
			else {
				//printf("意外的回车键");
			}
		}
	}

	// 向上回调
	for (KEYQEUE::iterator i = keyQeue2.begin(); i != keyQeue2.end(); ++i) {
		if (*i != VK_RETURN) {
			if (callBack_ != NULL) {
				int fal = callBack_((*i) , (*i) - '0', -1);
			}
		}
		else {
			//printf("意外的回车键");
		}
	}

	// 补加回车键通知
	if (callBack_ != NULL) {
		int fal = callBack_(28, 28, -1);
	}

	// 补加回车
	if (!fal) {
		Sleep(20);
		keybd_event(VK_RETURN, 0, 0, 0);
	}

	keyQeue2.clear();
	keyQeue.clear();
	isKbLock = false;
	return 0;
}

// 开始键盘模拟
void starSendKey() {
	isKbLock = true;
	HANDLE h = CreateThread(NULL, 0, KbSendThread, NULL, 0, NULL);
}

int KbHookSendKey(int keyCode) {
	keybd_event(keyCode, 0, 0, 0);
	return 0;
}

int KbHookThreadStat(CallBackFun callBack)
{
	callBack_ = callBack;
	for (int i = 0; i < 0xfe; i++) {
		KEY_PUT[i] = -1;
		KEY[i] = -2;
		down[i] = 0;
		down2[i] = 0;
		up[i] = 0;
	}


	// std::cout << "Hello World!\n";
	HANDLE h = CreateThread(NULL, 0, KbHookThread, NULL, 0, NULL);
	if (NULL == h) {
		printf("CreateThread failed, %ld\n", GetLastError());
		return -1;
	}
	
	printf("CreateThread scess v1.1");
	KbHookSendKey(VK_RETURN);
	return 0;
}
