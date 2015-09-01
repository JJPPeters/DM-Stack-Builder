#pragma once

#include "stdafx.h"

#include <boost/shared_ptr.hpp>
#include "boost/thread/mutex.hpp"
#include "boost/thread.hpp"
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

class DMWorker
{
private:
	virtual void DoWork() = 0;

	static UINT ThreadProcess(LPVOID p);

	CWinThread* m_hThread;

	boost::mutex thread_mtx;

	HANDLE m_hKillEvent;

	bool paused;

public:
	void Start();

	//  Stop thread parts are basically taken from
	//  http://stackoverflow.com/questions/29251723/how-to-safely-close-a-thread-which-has-a-infinite-loop-in-it
	//  http://stackoverflow.com/questions/21116675/terminating-a-worker-thread-from-a-parent-thread-mfc

	void Stop();

	bool IsStopping();

	bool ContinueWorking();

	// this is a really bad way of doing it, but oh well for now
	void Pause() { paused = !paused; }

	bool IsPaused() { return paused; }

	bool WaitForUnPaused();
};