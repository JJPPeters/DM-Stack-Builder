#include "stdafx.h"

#include "DMDialog.h"
#include "StackBuilder.h"

#include "DMWrapper/DMout.h"


void StackBuilder::Process()
{
	boost::lock_guard<boost::mutex> lock(*dialogmtx);
	Start();
}

void StackBuilder::DoWork()
{
	// lock the DoWork method, possibly not needed but just to be safe
	// I may be a bit mutex happy after discovering them
	boost::lock_guard<boost::mutex> lock(workmtx);

	// Here is where the code will go.
	DMresult << "Hello World!" << DMendl;
}