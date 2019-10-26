#pragma once
#include <thread>
#include "Pub_Def.h"
typedef struct T_EWM
{
	char n1;
	char n2;
} EWM;

int KbHook_SetHookLock(int value);
int KbHook_SendKey(int keyCode);
int KbHook_ThreadStat(CallBackFun fun);
int KbHook_ThreadStop();
