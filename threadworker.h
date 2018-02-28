#pragma once
#include "stdafx.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
// #include <boost/thread/win32/mfc_thread_init.hpp>
//	Copyright (C) Dave Kerr, 2009
//	dopedcoder@googlemail.com
//  http://www.codeproject.com/Articles/32601/Worker-Class

//	This class makes implementing a Worker Class (a class with the 'DoWork' function
//	that is threaded) trivial.

// Heavily modified to work better (using mutex abd afxbeginthread rather than while loop) Usage is largely the same

//	Usage:
//		Start a WorkerClass by calling 'Start'.
//		Make Worker Classes by deriving, and override 'DoWork'.

//  These no longer apply:
//		Stop a WorkerClass by calling 'Stop'.
//		Pause/Unpause a WorkerClass by calling 'Pause'.

//  for an idea on how to stop the thread, chec kout http://stackoverflow.com/questions/590792/how-to-kill-a-mfc-thread

// pause stuff was with help from: https://stackoverflow.com/questions/16907072/how-do-i-use-a-boost-condition-variable-to-wait-for-a-thread-to-complete-process
// and https://msdn.microsoft.com/en-us/library/windows/desktop/ms686903(v=vs.85).aspx

class ThreadWorker
{
private:
	virtual void DoWork() = 0;

	static UINT ThreadProcess(LPVOID p);

	CWinThread* m_hThread;

	boost::mutex thread_mtx;

	HANDLE m_hKillEvent;
	HANDLE m_hWorkEvent;

	CONDITION_VARIABLE pause_condition;

	CRITICAL_SECTION pause_lock;

	bool paused;

	void Run()
	{
		while (ContinueWorking())
		{
			// wait for the signal to actuall do something
			WaitForSingleObject(m_hWorkEvent, INFINITE);

			DoWork();
		}
	}

public:
	ThreadWorker();

	void Start();

	void RunWork();

	//  Stop thread parts are basically taken from
	//  http://stackoverflow.com/questions/29251723/how-to-safely-close-a-thread-which-has-a-infinite-loop-in-it
	//  http://stackoverflow.com/questions/21116675/terminating-a-worker-thread-from-a-parent-thread-mfc

	void Stop();

	bool IsStopping();

	bool ContinueWorking();

	void Pause();
	void Pause(bool set_pause);

	void WaitIfPaused();

	bool IsPaused()
	{
		return paused;
	} // Is this thread safe?
};

