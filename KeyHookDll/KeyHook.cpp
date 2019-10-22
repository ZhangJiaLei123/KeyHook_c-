#include "stdafx.h"
#include "KeyHook.h"
#include <chrono>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include "UdpTool.h"


#include "easywsclient.hpp"
#include <assert.h>
#include <stdio.h>
#include <string>

using easywsclient::WebSocket;
WebSocket::pointer ws = NULL;

//void handle_message(const std::string& message)
//{
//	printf(">>> %s\n", message.c_str());
//	if (message == "world") { ws->close(); }
//}


//#define DEBUG

#define ONLY_NUMBER

using namespace std;
using namespace std::chrono;

DWORD g_main_tid = 0;
HHOOK g_kb_hook = 0;

CallBackFun callBack_ = NULL;
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
	int scanCode = p->scanCode;

#ifdef ONLY_NUMBER // 	// ���η�����,����С���̺�f�µ�����
	// ת��Ϊʵ�ʵ�����ֵ
	if (p->vkCode >= '0' && p->vkCode <= '9') {
		scanCode = p->vkCode - '0';
	}
	else if (p->vkCode >= VK_NUMPAD0 && p->vkCode <= VK_NUMPAD9 ) {
		scanCode = p->vkCode - VK_NUMPAD0;
	}
	else {
		return CallNextHookEx(g_kb_hook, code, w, l);
	}

#endif // ONLY_NUMBER

	if (w == WM_KEYDOWN) {
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
			info, p->vkCode, scanCode);
#endif // DEBUG
	if (callBack_ != NULL) {
			callBack_( p->vkCode, scanCode, actionId);
	}
	else {
		char buf[64];
		sprintf_s(buf, 32, "%s - vkCode [%04x], scanCode [%04x]", info, p->vkCode, scanCode);
		SendUdp(buf, NULL);
	}
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

