#pragma once
#include <thread>
#include "Pub_Def.h"

int KbHookThreadStat(CallBackFun fun);

class KbHook
{
public:
	void run(CallBackFun fun);
	DWORD WINAPI KbHookThread(LPVOID lpParam);
	
private:
	LRESULT CALLBACK kb_proc(int code, WPARAM w, LPARAM l);
	void WorkFun();
private:
	CallBackFun callBack_;
	std::thread thread_;
	unsigned char data_[150];
	std::chrono::steady_clock::duration  startTime_;
};

