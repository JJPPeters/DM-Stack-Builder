#pragma once

#include "DMWrapper/DMWorker.h"
#include "DMWrapper/DMImage.h"
#include "DMWrapper/DMColmap.h"
#include "DMWrapper/DMROI.h"

#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread.hpp"

class CDMDialog;

class StackBuilder : public DMWorker, public DMListenable
{
private:
	// Mutex shared with the dialog class, used for situations where settings in the dialog need to be updated
	boost::shared_ptr<boost::mutex> dialogmtx;
	// Mutex purely for the Alignment class, stops the dowork method being called multiple times
	boost::mutex workmtx;

	boost::mutex exposuremtx;

	// Reference to parent dialog. Used to update things like a progress bar
	CDMDialog* parent; // can this be made to a shared_ptr?

	// Main body of the alignment code. Runs in new thread to avoid locking anything else.
	void DoWork();

	double lastPixel;

public:
	StackBuilder(CDMDialog* myparent, boost::shared_ptr<boost::mutex> mydialogmtx, DMImage tobewatched) : parent(myparent), dialogmtx(mydialogmtx), watchedImage(tobewatched)
	{
		lastPixel = 0.0;
		watchedImage.CreateDataListener();
		watchedImage.DataListener->addListenable(this);
	}

	StackBuilder(const StackBuilder& other) : parent(other.parent), dialogmtx(other.dialogmtx), watchedImage(other.watchedImage), lastPixel(other.lastPixel)
	{
		lastPixel = 0.0;
		watchedImage.CreateDataListener();
		watchedImage.DataListener->addListenable(this);
	}

	~StackBuilder() { Stop(); }

	void Process();

	DMImage watchedImage;
};