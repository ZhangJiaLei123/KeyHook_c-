#pragma once
#include <thread>
#include "Pub_Def.h"
typedef struct T_EWM
{
	char n1;
	char n2;
} EWM;


int KbHookSendKey(int keyCode);
int KbHookThreadStat(CallBackFun fun);

