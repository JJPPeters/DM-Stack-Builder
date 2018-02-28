#include "threadworker.h"

ThreadWorker::ThreadWorker() : paused(false)
{
	InitializeConditionVariable(&pause_condition);
	InitializeCriticalSection(&pause_lock);
}

void ThreadWorker::Pause()
{
	Pause(!paused);
}

void ThreadWorker::Pause(bool set_pause)
{
	EnterCriticalSection(&pause_lock);
	DEBUG3("ThreadWorker::Pause - Setting paused, current = " << paused << ", new = " << set_pause);
	paused = set_pause;
	//pause_condition.notify_all(); // TODO: all or one?
	WakeConditionVariable(&pause_condition); // Could be WakeAllCondi...

	LeaveCriticalSection(&pause_lock);
}

void ThreadWorker::WaitIfPaused()
{
	EnterCriticalSection(&pause_lock);
	DEBUG3("ThreadWorker::WaitIfPaused - Paused = " << paused);
	while (paused)
		SleepConditionVariableCS(&pause_condition, &pause_lock, INFINITE);
	LeaveCriticalSection(&pause_lock);
}

void ThreadWorker::Start()
{
	DEBUG3("ThreadWorker::Start invoked");
	// another thread cannot be launched
	if (thread_mtx.try_lock())
	{
		// handles to be used later
		DEBUG3("ThreadWorker::Start - Starting thread");
		m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hWorkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hThread = AfxBeginThread(ThreadProcess, this);
	}
}

UINT ThreadWorker::ThreadProcess(LPVOID p)
{
	ThreadWorker* me = (ThreadWorker *)p;
	boost::lock_guard<boost::mutex> lock(me->thread_mtx, boost::adopt_lock_t());
	//me->DoWork();
	me->Run();
	return 0;
}

void ThreadWorker::Stop()
{
	DEBUG3("ThreadWorker::Stop invoked");
	// set event so the Stopping() method will return true
	SetEvent(m_hKillEvent);					
	if (paused)
		Pause(); // if paused, we want to unpause it so we can stop it
	WaitForSingleObject(m_hThread, INFINITE); // wait for thread to finish

	CloseHandle(m_hThread); m_hThread = NULL;
	CloseHandle(m_hWorkEvent); m_hWorkEvent = NULL;
	CloseHandle(m_hKillEvent); m_hKillEvent = NULL;
}

// purely for convenience
bool ThreadWorker::ContinueWorking()
{
	return !IsStopping();
}

bool ThreadWorker::IsStopping()
{
	return WaitForSingleObject(m_hKillEvent, 0) == WAIT_OBJECT_0;
}

void ThreadWorker::RunWork()
{
	SetEvent(m_hWorkEvent);
}