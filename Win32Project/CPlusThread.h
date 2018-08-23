#pragma once
#include <thread>
#include "Pub_Def.h"

class CPlusThread
{
public:
	void run(CallBackFun fun);
private:
	void WorkFun();
private:
	CallBackFun callBack_;
	std::thread thread_;
	unsigned char data_[150];
	std::chrono::steady_clock::duration  startTime_;
};

