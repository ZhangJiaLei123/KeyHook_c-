#pragma once


#ifndef _CMYQUE_H_
#define	_CMYQUE_H_

#include <iostream>
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

using namespace std;

class Keybean
{
	DWORD   vkCode;
	DWORD   scanCode;
	DWORD   flags;
	DWORD   time;
	ULONG_PTR dwExtraInfo;

	queue<int> down; // °´¼ü°´ÏÂ


public :

	Keybean(PKBDLLHOOKSTRUCT p) {
		vkCode = p->vkCode;
		scanCode = p->scanCode;
		flags = p->flags;
		time = p->time;
		dwExtraInfo = p->dwExtraInfo;
	}
};

#endif