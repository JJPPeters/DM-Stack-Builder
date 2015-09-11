#pragma once

#include "DMWrapper/DMWorker.h"
#include "DMWrapper/DMImage.h"

#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread.hpp"

#include <cstdint>


class CDMDialog;

class BuilderGeneric;

template <class T>
class Builder;

// pure virtual class that has the same methods as the inherited classes
// We store a pointer to this class though ths object pointed to is the template class Builder (this allows us to handle the different potential types)
class BuilderGeneric : public DMWorker, public DMListenable
{
private:

	virtual void DoWork() = 0;

public:

	virtual ~BuilderGeneric() {};

	virtual void StopBuild() = 0;

	virtual void Process() = 0;

	virtual bool IsImageOpen() = 0;
	virtual int GetImageID() = 0;

	virtual void StartBuildingFixed(int slices) = 0;
	virtual void StartBuildingExpanding(int slices) = 0;
};

// actual implementation of the builder
template <class T>
class Builder : public BuilderGeneric
{
private:

	boost::shared_ptr<boost::mutex> dialogmtx;
	boost::mutex destroymtx;

	CDMDialog* parent;

	DMImage<T> watchedImage;
	DMImage<T> buildingImage;

	T oldPixel;

	int fixednumber, sliceindex, expandnumber;

	// to really protect these, can we put them in a class that is defined in this class with the getters and setters
	bool dofixed, doexpanded, dobuild;;

	void SetFixed() { dofixed = true; doexpanded = false; }
	void SetExpanding() { dofixed = false; doexpanded = true; }

public:

	// TODO: check that these are constructing the images properly (not just trying to copy, could cast them)
	Builder(DMImageGeneric& towatch, CDMDialog* myparent, boost::shared_ptr<boost::mutex> mydialogmtx) : watchedImage(towatch), parent(myparent), dialogmtx(mydialogmtx)
	{
		dofixed = false;
		doexpanded = false;
		dobuild = false;
		int sy = watchedImage.getHeight();
		int sx = watchedImage.getWidth();

		oldPixel = watchedImage.getElement(sy - 1, sx - 1);

		watchedImage.GetDataListener()->AddListenable(this);
	}

	~Builder()
	{
		//i think the remove data listener thing is not working and the listener is still trying to call process after the object has been destroyed
		// the active variable in the DMlistener is not being maintained properly, possible due to some copy thing (in the image?)

		// could also clear listenables? to stop it calling this

		// it appears something here will crash the program if the dowork thing is still going on
		DMresult << "dconstruct" << DMendl;
		// TODO: spend more time making sure everything that needs to be destructed is.
		boost::lock_guard<boost::mutex> lock(destroymtx, boost::adopt_lock_t()); // not sure if this waits to get the lock
		DMresult << "remove listener" << DMendl;
		watchedImage.RemoveDataListener(); // this might be the part that is crashing
		DMresult << "stio build" << DMendl;
		StopBuild(); // this just stops the doWork function doing anything (it is still entered), should be fine
		DMresult << "stop thread" << DMendl;
		Stop(); //this stops the worker
		DMresult << "end" << DMendl;
	}

	void Process()
	{
		//boost::lock_guard<boost::mutex> lock(destroymtx);
		Start();
	}

	// not the same as Stop as we want the listening to keep happening
	void StopBuild()
	{
		// need to mutex changing sliceindex as the loop could be half way complete.
		//sliceindex = 0; // not realy needed, but nice anyway
		dobuild = false; // this should possibly be protected


		// check for blank frames and remove them
	}

	void DoWork()
	{
		// stop the item being deleted whilst it is working, could maybe have something in the destructor
		// i.e. a mutex for destroying, possible a flag to return at the beginning of this method

		//try to get the lock, if we don't then just return

		if (!destroymtx.try_lock() || !dialogmtx->try_lock())
			return;
		boost::lock_guard<boost::mutex> dlock(destroymtx, boost::adopt_lock_t());
		boost::lock_guard<boost::mutex> lock(*dialogmtx, boost::adopt_lock_t());
		
		//DMresult << "Doing my listen!" << DMendl;

		int sy = watchedImage.getHeight();
		int sx = watchedImage.getWidth();

		T newPixel = watchedImage.getElement(sy - 1, sx - 1);

		if (newPixel == oldPixel)
			return;

		//DMresult << "Last pixel changed!!!" << DMendl;
		// reset values for the next datachange call
		oldPixel = newPixel;

		// check if we are doing the build and have a method to build by
		if (!(dofixed || doexpanded) || !dobuild)
			return;

		// this is just a safety net, the previoud iteration of this loop should make it so this statement is never hit
		if (sliceindex >= buildingImage.getDepth())
			return;

		// get data from watched image
		std::vector<T> newSlice(sx*sy);
		try
		{
			watchedImage.GetSliceData(newSlice);
		}
		catch (const std::exception& ex)
		{
			DMresult << ex.what() << DMendl;
			return;
		}

		// copy over to correct slice of building image
		try
		{
			buildingImage.SetSliceData(newSlice, sliceindex);
		}
		catch (const std::exception& ex)
		{
			DMresult << ex.what() << DMendl;
			return;
		}

		//set current slice to be new one
		buildingImage.SetDisplayedSlice(sliceindex);

		buildingImage.DataChanged(); // might want to not do this until very end of stack

		// increment iterator, then test if we are to continue or need to expand the stack
		++sliceindex;

		// for expandable only, get current image depth, if sliceindex >= then reshape image
		if (sliceindex >= buildingImage.getDepth() && doexpanded)
		{
			std::vector<T> imagecopy(sx*sy*buildingImage.getDepth());
			// Get data from building image
			try
			{
				buildingImage.GetData(imagecopy);
			}
			catch (const std::exception& ex)
			{
				DMresult << ex.what() << DMendl;
				return;
			}
			
			// expand the build image
			try
			{
				buildingImage.AddSlices(expandnumber);
			}
			catch (const std::exception& ex)
			{
				DMresult << ex.what() << DMendl;
				return;
			}

			// copy data back into building image
			try
			{
				buildingImage.SetData(imagecopy);
				buildingImage.DataChanged();
			}
			catch (const std::exception& ex)
			{
				DMresult << ex.what() << DMendl;
				return;
			}
		}

		// only for fixed method
		if (sliceindex >= fixednumber && dofixed)
		{
			sliceindex = 0;
			dobuild = false;
			return;
		}
	}

	bool IsImageOpen() { return watchedImage.IsOpen(); }

	int GetImageID() { return watchedImage.getID(); }

	// here the number of slices is fixed and we just fill them up
	void StartBuildingFixed(int slices)
	{
		// Need to account for if a build has already started
		// mutex in the dowork loop?

		// call stop() that sets dobuild to false, waits for the dowork mutex to be freed, removes blank frames and...

		sliceindex = 0;
		DataType::DataTypes dtype = static_cast<DataType::DataTypes>(watchedImage.getDataType());

		int sy = watchedImage.getHeight();
		int sx = watchedImage.getWidth();

		// TODO: get name of watched image
		buildingImage.fromType("building", sx, sy, slices, dtype);
		buildingImage.Show();

		// now need to set a flag (with mutex protection?) that will indicate that the DoWork function can start copying the data
		// set a variable that indicates how many frames (e.g. slices) and a variable to indicate we are doing the fixed slices version.
		fixednumber = slices;

		dobuild = true;
		SetFixed();
	}

	// here the number of slices will expand as we go on
	void StartBuildingExpanding(int slices)
	{
		sliceindex = 0;

		if (doexpanded)
		{
			dobuild = false;
			return;
		}

		DataType::DataTypes dtype = static_cast<DataType::DataTypes>(watchedImage.getDataType());

		int sy = watchedImage.getHeight();
		int sx = watchedImage.getWidth();

		// TODO: get name of watched image
		buildingImage.fromType("building", sx, sy, slices, dtype);
		buildingImage.Show();

		// now need to set a flag (with mutex protection?) that will indicate that the DoWork function can start copying the data
		// set a variable that indicates how many frames (e.g. slices) and a variable to indicate we are doing the fixed slices version.
		expandnumber = slices;

		dobuild = true;
		SetExpanding();
	}
};

// this class is actually pretty pointless now, could just move it to the Dialog class.
// At least it hides the massive switch statement from view
class StackBuilder
{
private:

	boost::shared_ptr<BuilderGeneric> build;

public:

	StackBuilder(CDMDialog* myparent, boost::shared_ptr<boost::mutex> mydialogmtx, DMImageGeneric& tobewatched)
	{

		DataType::DataTypes dtype = static_cast<DataType::DataTypes>(tobewatched.getDataType());

		switch (dtype)
		{
		case DataType::BOOL:
			build.reset(new Builder<bool>(tobewatched, myparent, mydialogmtx)); break;
		case DataType::INT_1:
			build.reset(new Builder<int8_t>(tobewatched, myparent, mydialogmtx)); break;
		case DataType::INT_2:
			build.reset(new Builder<int16_t>(tobewatched, myparent, mydialogmtx)); break;
		case DataType::INT_4:
			build.reset(new Builder<int32_t>(tobewatched, myparent, mydialogmtx)); break;
		case DataType::UINT_1:
			build.reset(new Builder<uint8_t>(tobewatched, myparent, mydialogmtx)); break;
		case DataType::UINT_2:
			build.reset(new Builder<uint16_t>(tobewatched, myparent, mydialogmtx)); break;
		case DataType::UINT_4:
			build.reset(new Builder<uint32_t>(tobewatched, myparent, mydialogmtx));break;
		case DataType::FLOAT:
			build.reset(new Builder<float>(tobewatched, myparent, mydialogmtx)); break;
		case DataType::DOUBLE:
			build.reset(new Builder<double>(tobewatched, myparent, mydialogmtx));break;
		case DataType::COMPLEX_FLOAT:
			build.reset(new Builder<std::complex<float>>(tobewatched, myparent, mydialogmtx)); break;
		case DataType::COMPLEX_DOUBLE:
			build.reset(new Builder<std::complex<double>>(tobewatched, myparent, mydialogmtx)); break;
		case DataType::RGB:
			DMresult << "RGB images are not supported yet." << DMendl; return; break;
		default:
			DMresult << "I don't know this image type." << DMendl; return; break;
		}

	}

	bool IsImageOpen() { return build->IsImageOpen(); }
	int GetImageID() { return build->GetImageID(); }
	void StartBuildingFixed(int slices) { build->StartBuildingFixed(slices); }
	void StartBuildingExpanding(int slices) { build->StartBuildingExpanding(slices); }

	void StopBuild(){ build->StopBuild(); }
};