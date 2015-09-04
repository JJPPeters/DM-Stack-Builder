#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include "DMROI.h"
#include "DMListener.h"
#include "DMout.h"

template <class U>
class DMImage;

namespace DataType
{
	enum DataTypes {
		BOOL = 14,
		INT_1 = 9,
		INT_2 = 1,
		INT_4 = 7,
		UINT_1 = 6,
		UINT_2 = 10,
		UINT_4 = 11,
		FLOAT = 2,
		DOUBLE = 12,
		COMPLEX_FLOAT = 3,
		COMPLEX_DOUBLE = 13,
		RGB = 23
	};
}

class DMImageGeneric
{
protected:
	unsigned long width;
	unsigned long height;
	unsigned long depth;

	DM::Image Image;
	DM::ImageDisplay Display;

	std::vector<boost::shared_ptr<DMROI>> ROIs;

	boost::shared_ptr<DMListener> ROIListener;
	boost::shared_ptr<DMListener> DataListener;

	void MakeFromType(std::string title, int x, int y, int z, DataType::DataTypes dtype);

public:

	DMImageGeneric() : ROIListener(new DMListener), DataListener(new DMListener) {}

	//check copy constructor of listenable
	DMImageGeneric(const DMImageGeneric& other) : Image(other.Image), Display(other.Display), ROIs(other.ROIs), ROIListener(other.ROIListener), DataListener(other.DataListener)
	{
		Image.GetDimensionSizes(width, height, depth);
		AddDataListener();// might need a call to AddDataListener here to re-add display?
		AddROIListener();
	}

	~DMImageGeneric() { RemoveDataListener(); RemoveROIListener(); }

	void fromFront();
	void fromType(std::string title, int x, int y, int z, DataType::DataTypes dtype);

	unsigned long getDataType() const { return Image.GetDataType(); }
	unsigned long getID() const { return Image.GetID(); }
	long GetDisplayID() { return Display.GetID(); }
	std::string GetTitle() const { return Image.GetName(); }

	boost::shared_ptr<DMListener> GetDataListener() { return DataListener; }
	boost::shared_ptr<DMListener> GetROIListener() { return ROIListener; }

	void DataChanged() { Image.DataChanged(); }

	void Show();
	void ShowAt(int t, int l, int b, int r);

	void Reshape(int x, int y, int z);
	void Reshape(int x, int y) { Reshape(x, y, getDepth()); }

	void AddSlices(int n) { Reshape(getWidth(), getHeight(), getDepth() + n); }

	bool IsSquarePow2() const { return ((width != 0) && !(width & (width - 1))) && ((height != 0) && !(height & (height - 1))) && (width == height); }

	unsigned long getWidth() { Image.GetDimensionSizes(width, height, depth); return width; };
	unsigned long getHeight() { Image.GetDimensionSizes(width, height, depth); return height; };
	unsigned long getDepth() { Image.GetDimensionSizes(width, height, depth); return depth; };

	// this does not need to be included as you only need it when you know the type
	// T &getElement(int j, int i);

	void DeleteImage() { DM::DeleteImage(Image); }

	void addROI(DMROI roi);

	// TODO: add functions to add listenables

	void NewROIListener();
	void AddROIListener();
	void RemoveROIListener();

	void NewDataListener();
	void AddDataListener();
	void RemoveDataListener();

	void ClearListenables() { (*ROIListener).ClearListeners(); (*ROIListener).ClearROIs(); }

	coord<long> GetWindowPosition() { long x, y; DM::GetWindowPosition(Image, &x, &y); return coord<long>(x, y); }
	coord<long> GetWindowSize() { long x, y; DM::GetWindowSize(Image, &x, &y); return coord<long>(x, y); }

	void SetWindowPosition(coord<long> pos) { DM::SetWindowPosition(Image, pos.x, pos.y); }
	void SetWindowSize(coord<long> size) { DM::SetWindowSize(Image, size.x, size.y); }

	// All GetSliceData methods require type to be known

	bool operator==(DMImageGeneric& RHS)
	{
		return getID() == RHS.getID();
	}

	bool IsOpen();
};