#include "stdafx.h"
#include "Pub_Def.h"
#include "CPlusThread.h"

void StartRun(CallBackFun pFun)
{
	CPlusThread* pAlarmSer =  new CPlusThread;
	pAlarmSer->run(pFun);
}