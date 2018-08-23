#include "stdafx.h"
#include "CPlusThread.h"
#include <chrono>

using namespace std;
using namespace std::chrono;


void CPlusThread::run(CallBackFun fun)
{
	callBack_ = fun;
	thread_ = std::thread(&CPlusThread::WorkFun,this);
}

void CPlusThread::WorkFun()
{
	startTime_ = steady_clock::now().time_since_epoch();
	while (TRUE)
	{
		if (callBack_)
		{
			steady_clock::duration currentTime = steady_clock::now().time_since_epoch();
			long long time_span = duration_cast<milliseconds>(currentTime - startTime_).count();
			Msg msg;
			msg.duration = time_span;
			strcpy_s(msg.note,"C++线程持续运行的毫秒数");
			memcpy_s(data_,8,(void*)&msg.duration,8);
			memcpy_s(data_+8,100,(void*)msg.note,100);
			callBack_(data_, 108);
		}
		Sleep(1000);
	}
}

