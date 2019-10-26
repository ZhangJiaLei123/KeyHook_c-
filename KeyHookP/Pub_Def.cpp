// 接口实现
#include "Pub_Def.h"
#include "KeyHook.h"

int StartpKeyHook(CallBackFun pFun)
{
	return KbHook_ThreadStat(pFun);
}

int StopKeyHook() {

	return KbHook_ThreadStop();
}

int SendKey(int keyCode) {
	return KbHook_SendKey(keyCode);
}

int SetKbHookLock(int value) {
	return KbHook_SetHookLock(value);
}

