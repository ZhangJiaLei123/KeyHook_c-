#include "stdafx.h"
#include "Pub_Def.h"
#include "CPlusThread.h"

int StartRun(CallBackFun pFun)
{
	//KbHook* pAlarmSer =  new KbHook;
	//pAlarmSer->run(pFun);
	return KbHookThreadStat(pFun);
}