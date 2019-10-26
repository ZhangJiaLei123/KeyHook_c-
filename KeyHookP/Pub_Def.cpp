
#include "Pub_Def.h"
#include "KeyHook.h"

int StartRun(CallBackFun pFun)
{
	//KbHook* pAlarmSer =  new KbHook;
	//pAlarmSer->run(pFun);
	return KbHookThreadStat(pFun);
}

int SendKey(int keyCode) {
	return KbHookSendKey(keyCode);
}

int MakeKeyHookUdp() {

	return KbHookThreadStat(NULL);
}

int StopKeyHookUdp() {

	return 1;
}

int KeyHookTest() {
	printf("KeyHookDll make sucess\n");
	return 1;
}