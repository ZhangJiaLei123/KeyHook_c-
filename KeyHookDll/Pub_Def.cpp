
#include "stdafx.h"
#include "Pub_Def.h"
#include "KeyHook.h"
#include "UdpTool.h"

int StartRun(CallBackFun pFun)
{
	//KbHook* pAlarmSer =  new KbHook;
	//pAlarmSer->run(pFun);
	return KbHookThreadStat(pFun);
}

int MakeKeyHookUdp() {
	printf("MakeKeyHookUdpsucess\n");
	if (CreateUdp() != 0) {
		return -1;
	}
	return KbHookThreadStat(NULL);
}

int StopKeyHookUdp() {
	CloseUdp();
	return 1;
}

int KeyHookTest() {
	printf("KeyHookDll make sucess\n");
	return 1;
}