#pragma once

#include "DMWrapper/DMWorker.h"
#include "DMWrapper/DMImage.h"
#include "DMWrapper/DMColmap.h"
#include "DMWrapper/DMROI.h"

#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread.hpp"

class CDMDialog;

class StackBuilder : public DMWorker//, public DMListenable
{
private:
	// Mutex shared with the dialog class, used for situations where settings in the dialog need to be updated
	boost::shared_ptr<boost::mutex> dialogmtx;
	// Mutex purely for the Alignment class, stops the dowork method being called multiple times
	boost::mutex workmtx;

	// Reference to parent dialog. Used to update things like a progress bar
	CDMDialog* parent; // can this be made to a shared_ptr?

	// Main body of the alignment code. Runs in new thread to avoid locking anything else.
	void DoWork();

public:
	StackBuilder(CDMDialog* myparent, boost::shared_ptr<boost::mutex> mydialogmtx) : parent(myparent), dialogmtx(mydialogmtx) {}

	StackBuilder(const StackBuilder& other) : parent(other.parent), dialogmtx(other.dialogmtx) {}

	void Process();
};