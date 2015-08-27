#pragma once

#include <boost/shared_ptr.hpp>
#include "boost/thread/mutex.hpp"
#include "boost/thread.hpp"
//	Copyright (C) Dave Kerr, 2009
//	dopedcoder@googlemail.com

//	This class makes implementing a Worker Class (a class with the 'DoWork' function
//	that is threaded) trivial.

// Heavily modified to work better (using mutex abd afxbeginthread rather than while loop) Usage is largely the same

//	Usage:
//		Start a WorkerClass by calling 'Start'.
//		Make Worker Classes by deriving, and override 'DoWork'.

//  These no longer apply:
//		Stop a WorkerClass by calling 'Stop'.
//		Pause/Unpause a WorkerClass by calling 'Pause'.

class DMWorker
{
private:
	virtual void DoWork() = 0;
	static UINT ThreadProcess(LPVOID p);

	boost::mutex thread_mtx; // not sure this is needed
public:
	void Start();
};