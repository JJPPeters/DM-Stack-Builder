#include "stdafx.h"

#include "DMDialog.h"
#include "StackBuilder.h"

#include "DMWrapper/DMout.h"

#include <dos.h> // for sleep
#include <stdlib.h> // for rand

void StackBuilder::Process()
{
	boost::lock_guard<boost::mutex> lock(*dialogmtx);
	Start();
}

void StackBuilder::DoWork()
{
	int sy = watchedImage.getHeight();
	int sx = watchedImage.getWidth();

	DMresult << "Doing my listen, " << sx << ", " << sy << DMendl;

	// get current lastPixel;
	// this template type depends on the type of the image
	// basically this DoWork function needs to just get the data type and then call a template function to do the actual maths etc... (or just convert to a double for processing
	double currentPixel = watchedImage.getElement<double>(sy-1, sx-1);

	DMresult << "last: " << lastPixel << ", current: " << currentPixel << DMendl;

	if (std::abs(std::abs(currentPixel) - std::abs(lastPixel)) > 1e-10)
	{
		DMresult << "Last Pixel Changed" << DMendl;
	}

	lastPixel = currentPixel;
}