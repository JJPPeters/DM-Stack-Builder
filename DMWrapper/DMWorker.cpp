#include "stdafx.h"
#include "DMWorker.h"


void DMWorker::Start()
{
	// another thread cannot be launched
	if (thread_mtx.try_lock())
	{
		// handles to be used later
		m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hThread = AfxBeginThread(ThreadProcess, this);
		paused = false;
	}
}

UINT DMWorker::ThreadProcess(LPVOID p)
{
	DMWorker* me = (DMWorker *)p;
	// As the mutex is locked in the creating thread
	// we will unlock then lock it in this thread.
	// Could be very bad
	//if (!me->thread_mtx.try_lock())
	//{
	//	//me->thread_mtx.unlock();
	//	//me->thread_mtx.lock();
	//}

	boost::lock_guard<boost::mutex> lock(me->thread_mtx, boost::adopt_lock_t());

	me->DoWork();
	//me->thread_mtx.unlock();
	return 0;
}

void DMWorker::Stop()
{
	// set event so the Stopping() method will return true
	SetEvent(m_hKillEvent);
	paused = false; // is paused, we want to unpause it so we can stop it
	// wait for thread to finish
	WaitForSingleObject(m_hThread, INFINITE);
}

bool DMWorker::ContinueWorking()
{
	return WaitForUnPaused() && !IsStopping();
}

bool DMWorker::IsStopping()
{
	return WaitForSingleObject(m_hKillEvent, 0) == WAIT_OBJECT_0;
}

// I don't like my while loop method for doing this, but it works for now.
// should use an event, basically use the waitforsingleobject as the 'while loop'
bool DMWorker::WaitForUnPaused()
{
	while (IsPaused() && !IsStopping()) { Sleep(1); }
	if (IsStopping())
		return false;
	else
		return true;
}