#include "stdafx.h"
#include "DMWorker.h"


void DMWorker::Start()
{
	AfxBeginThread(ThreadProcess, this);
}

UINT DMWorker::ThreadProcess(LPVOID p)
{
	DMWorker* me = (DMWorker *)p;
	boost::lock_guard<boost::mutex> lock(me->thread_mtx);
	me->DoWork();
	return 0;
}