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

	virtual void Process() = 0;

	virtual bool IsImageOpen() = 0;
	virtual int GetImageID() = 0;

	virtual void StartBuildingFixed(int slices) = 0;
};

// actual implementation of the builder
template <class T>
class Builder : public BuilderGeneric
{
private:

	boost::shared_ptr<boost::mutex> dialogmtx;

	CDMDialog* parent;

	DMImage<T> watchedImage;
	DMImage<T> buildingImage;

	T oldPixel;

	int slicenumber, sliceindex;

	// to really protect these, can we put them in a class that is defined in this class with the getters and setters
	bool dofixed, doexpanded, dobuild;;

	void SetFixed() { dofixed = true; doexpanded = false; }
	void SetExpandable() { dofixed = false; doexpanded = true; }

public:

	// TODO: check that these are constructing the images properly (not just trying to copy, could cast them)
	Builder(DMImageGeneric towatch, CDMDialog* myparent, boost::shared_ptr<boost::mutex> mydialogmtx) : watchedImage(towatch), parent(myparent), dialogmtx(mydialogmtx)
	{
		dofixed = false;
		doexpanded = false;
		dobuild = false;
		int sy = watchedImage.getHeight();
		int sx = watchedImage.getWidth();

		oldPixel = watchedImage.getElement(sy - 1, sx - 1);

		watchedImage.CreateDataListener(); // TODO: this shouldn't need to be called beforehand??
		watchedImage.GetDataListener()->addListenable(this);
	}

	~Builder()
	{
		// TODO: spend more time making sure everything that needs to be destructed is.
		Stop();
		watchedImage.RemoveDataListener();
	}

	void Process()
	{
		boost::lock_guard<boost::mutex> lock(*dialogmtx);
		Start();
	}

	void DoWork()
	{
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

		// only for fixed method
		if (sliceindex >= slicenumber)
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

		buildingImage.DataChanged(); // might want to not do this until very end

		// done?
		++sliceindex;

		// just for testing so i don't have to worry about resetting anything
		if (sliceindex >= slicenumber)
		{
			sliceindex = 0;
			dobuild = false;
		}



		int expand_number = 3;

		// for expandable only, get current image depth, if sliceindex >= then reshape image
		if (sliceindex >= buildingImage.getDepth())
		{
			std::vector<T> imagecopy(sx*sy*buildingImage.getDepth());
			// get all data
			buildingImage.GetData(imagecopy);
			buildingImage.AddSlices(expand_number);
			buildingImage.getDepth(imagecopy);
			//set all data
		}
	}

	bool IsImageOpen() { return watchedImage.IsOpen(); }

	int GetImageID() { return watchedImage.getID(); }

	// here the number of slices is fixed and we just fill them up
	void StartBuildingFixed(int slices)
	{
		sliceindex = 0;
		DataType::DataTypes dtype = static_cast<DataType::DataTypes>(watchedImage.getDataType());

		int sy = watchedImage.getHeight();
		int sx = watchedImage.getWidth();

		// TODO: get name of watched image
		buildingImage.fromType("building", sx, sy, slices, dtype);
		buildingImage.Show();

		// now need to set a flag (with mutex protection?) that will indicate that the DoWork function can start copying the data
		// set a variable that indicates how many frames (e.g. slices) and a variable to indicate we are doing the fixed slices version.
		slicenumber = slices;

		dobuild = true;
		SetFixed();
	}
};

// this class is actually pretty pointless now, could just move it to the Dialog class
class StackBuilder
{
private:

	boost::shared_ptr<BuilderGeneric> build;

public:

		StackBuilder(CDMDialog* myparent, boost::shared_ptr<boost::mutex> mydialogmtx, DMImageGeneric tobewatched)
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
		bool GetImageID() { return build->GetImageID(); }
		void StartBuildingFixed(int slices) { build->StartBuildingFixed(slices); }

};
//	// Mutex shared with the dialog class, used for situations where settings in the dialog need to be updated
//	boost::shared_ptr<boost::mutex> dialogmtx;
//	// Mutex purely for the Alignment class, stops the dowork method being called multiple times
//	boost::mutex workmtx;
//
//	boost::mutex exposuremtx;
//
//	// Reference to parent dialog. Used to update things like a progress bar
//	CDMDialog* parent; // can this be made to a shared_ptr?
//
//	// Main body of the alignment code. Runs in new thread to avoid locking anything else.
//	void DoWork();
//
//	numbertype lastPixel;
//
//	DataType::DataTypes watchedType;
//
//	template <typename T>
//	numbertype getLastPixel();
//
//public:
//	StackBuilder(CDMDialog* myparent, boost::shared_ptr<boost::mutex> mydialogmtx, DMImage tobewatched) : parent(myparent), dialogmtx(mydialogmtx), watchedImage(tobewatched)
//	{
//		// need a better way to set this to the initial value
//
//		watchedType = static_cast<DataType::DataTypes>(watchedImage.getDataType());
//
//
//		watchedImage.CreateDataListener();
//		watchedImage.DataListener->addListenable(this);
//	}
//
//	StackBuilder(const StackBuilder& other) : parent(other.parent), dialogmtx(other.dialogmtx), watchedImage(other.watchedImage), lastPixel(other.lastPixel), watchedType(other.watchedType)
//	{
//		lastPixel = other.lastPixel;
//		watchedImage.CreateDataListener();
//		watchedImage.DataListener->addListenable(this);
//	}
//
//	~StackBuilder()
//	{
//		Stop();
//		watchedImage.RemoveDataListener();
//	}
//
//	void startBuilding(int slices);
//
//	unsigned long getImageID()
//	{
//		return watchedImage.getID();
//	}
//
//	void Process();
//
//	template <typename T>
//	void BuildStack();
//
//	DMImage watchedImage;
//	DMImage buildImage;
//};