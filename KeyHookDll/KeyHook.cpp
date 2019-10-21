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
* �����¼���Ӧ�ص�
*/
bool CALLBACK con_handler(DWORD CtrlType)
{
	//printf("ע��Ctrl + C��");
	return TRUE;
};

/***
* �����¼���Ӧ
*/
LRESULT CALLBACK kb_proc(int code, WPARAM w, LPARAM l)
{
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)l;
	const char* info = NULL;
	int actionId = 0;
	if (w == WM_KEYDOWN){
		info = "key dn"; // ��������
		actionId = 1;
	}
	else if (w == WM_KEYUP) {
		info = "key up"; // �����ͷ�
		actionId = -1;
	}
	else if (w == WM_SYSKEYDOWN) // ϵͳ��������,Alt����ʱ��Ӧ
	{
		info = "sys key dn";
		actionId = 2;
	}
	else if (w == WM_SYSKEYUP) // ϵͳ�����ͷ�
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
*  ���̹��� �����߳�
*/
DWORD WINAPI KbHookThread(LPVOID lpParam)
{
	g_main_tid = GetCurrentThreadId();

	//ע��Ctrl + C��
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)&con_handler, TRUE);
	g_kb_hook = SetWindowsHookEx(WH_KEYBOARD_LL, // ��װһ�������ӳ������ӵͼ����������¼���
		(HOOKPROC)&kb_proc, //���ӵͼ����������¼��Ĺ����ӳ̡�
		NULL,//DLL�ľ��������lpfn����ָ��Ĺ����ӳ̡����dwThreadId����ָ����ǰ���̴������̣߳����ҹ����ӳ�λ���뵱ǰ���̹����Ĵ����У�����뽫hMod��������ΪNULL��
		0);//�빳���ӳ���������̵߳ı�ʶ������������Ӧ�ó�������ò���Ϊ�㣬�����ӳ���������߳���ͬһ���������е����������߳���������й�Windows�̵�Ӧ�ó�������ı�ע���֡�
	if (g_kb_hook == NULL) {
		printf("SetWindowsHookEx failed, %ld\n", GetLastError());
		return 0;
	}

	// Message ѭ��
	MSG msg;

	// GetMessage����WM_QUIT��Ϣ��ֵΪ��ʱ���ء�
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg); // ��Ϣת��
		DispatchMessage(&msg);  // �ͷ���Ϣ
	}

	// �˳��߳�
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

