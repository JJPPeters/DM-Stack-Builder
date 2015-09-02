#pragma once

#include "DMListener.h"
#include "DMROI.h"
#include "DMout.h"

#include <boost/shared_ptr.hpp>

//enum ShowComplex{
//	SHOW_REAL,
//	SHOW_IMAG,
//	SHOW_AMP,
//	SHOW_PHASE,
//	SHOW_ABS
//};

/*
Binary type number: 14
int 1 type number: 9
int 2 type number: 1
int 4 type number: 7
uint 1 type number: 6
uint 2 type number: 10
uint 4 type number: 11
real 4 type number: 2
real 8 type number: 12
complex 8 type number: 3
complex 16 type number: 13
RGB 4 type number: 23
*/
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

class DMImage
{
private:
	unsigned long width;
	unsigned long height;
	unsigned long depth;
public:
	DM::Image Image;
	DM::ImageDisplay Display;

	unsigned long getDataType() const
	{
		return Image.GetDataType();
	}

	void DataChanged()
	{
		Image.DataChanged();
	}

	void Show()
	{
		Image.GetOrCreateImageDocument().Show();
		Image.GetOrCreateImageDocument().Clean(); // not sure if this is needed
	}

	bool isSquarePow2() const
	{
		return ((width != 0) && !(width & (width - 1))) && ((height != 0) && !(height & (height - 1))) && (width == height);
	}

	unsigned long getWidth(){ Image.GetDimensionSizes(width, height, depth); return width; };
	unsigned long getHeight(){ Image.GetDimensionSizes(width, height, depth); return height; };
	unsigned long getDepth(){ Image.GetDimensionSizes(width, height, depth); return depth; };

	std::vector<boost::shared_ptr<DMROI>> ROIs;

	boost::shared_ptr<DMListener> ROIListener;
	boost::shared_ptr<DMListener> DataListener;

	DMImage() : ROIListener(new DMListener), DataListener(new DMListener) {}

	template <typename T>
	T &getElement(int j, int i)
	{
		Gatan::PlugIn::ImageDataLocker iLocker = Gatan::PlugIn::ImageDataLocker(Image);
		T* idata = static_cast<T*>(iLocker.get());

		return idata[j * width + i];
	}

	void fromType(std::string title, int x, int y, int z, DataType::DataTypes dtype)
	{
		switch (dtype)
		{
		case DataType::BOOL:
			Image = DM::BinaryImage(title.c_str(), x, y, z);
			break;
		case DataType::INT_1:
			Image = DM::IntegerImage(title.c_str(), 1, false, x, y, z);
			break;
		case DataType::INT_2:
			Image = DM::IntegerImage(title.c_str(), 2, false, x, y, z);
			break;
		case DataType::INT_4:
			Image = DM::IntegerImage(title.c_str(), 4, false, x, y, z);
			break;
		case DataType::UINT_1:
			Image = DM::IntegerImage(title.c_str(), 1, true, x, y, z);
			break;
		case DataType::UINT_2:
			Image = DM::IntegerImage(title.c_str(), 2, true, x, y, z);
			break;
		case DataType::UINT_4:
			Image = DM::IntegerImage(title.c_str(), 4, true, x, y, z);
			break;
		case DataType::FLOAT:
			Image = DM::RealImage(title, 4, x, y, z);
			break;
		case DataType::DOUBLE:
			Image = DM::RealImage(title, 8, x, y, z);
			break;
		case DataType::COMPLEX_FLOAT:
			Image = DM::ComplexImage(title.c_str(), 8, x, y, z);
			break;
		case DataType::COMPLEX_DOUBLE:
			Image = DM::ComplexImage(title.c_str(), 16, x, y, z);
			break;
		case DataType::RGB:
			DMresult << "RGB images are not supported yet." << DMendl;
			return;
			break;
		default:
			DMresult << "I don't know this image type." << DMendl;
			return;
			break;
		}
	}

	//template <typename T>
	//DMImage(const std::vector<std::complex<T>> &data, std::string title, int bytes, int x, int y, int z) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y, z);
	//	SetComplexData(data, SHOW_REAL);
	//}

	//template <typename T>
	//DMImage(const std::vector<std::complex<T>> &data, std::string title, int bytes, int x, int y, int z, long t, long l, long b, long r) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y, z, t, l, b, r);
	//	SetComplexData(data, SHOW_REAL);
	//}

	//template <typename T>
	//DMImage(const std::vector<T> &data, std::string title, int bytes, int x, int y, int z) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y, z);
	//	SetRealData(data);
	//}

	//template <typename T>
	//DMImage(const std::vector<T> &data, std::string title, int bytes, int x, int y, int z, long t, long l, long b, long r) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y, z, t, l, b, r);
	//	SetRealData(data);
	//}

	//template <typename T>
	//DMImage(const std::vector<std::complex<T>> &data, std::string title, int bytes, int x, int y) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y);
	//	SetComplexData(data, SHOW_REAL);
	//}

	//template <typename T>
	//DMImage(const std::vector<std::complex<T>> &data, std::string title, int bytes, int x, int y, long t, long l, long b, long r) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y, t, l, b, r);
	//	SetComplexData(data, SHOW_REAL);
	//}

	//template <typename T>
	//DMImage(const std::vector<T> &data, std::string title, int bytes, int x, int y) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y);
	//	SetRealData(data);
	//}

	//template <typename T>
	//DMImage(const std::vector<T> &data, std::string title, int bytes, int x, int y, long t, long l, long b, long r) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y, t, l, b, r);
	//	SetRealData(data);
	//}

	//DMImage(std::string title, int bytes, int x, int y, int z) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y, z);
	//}

	//DMImage(std::string title, int bytes, int x, int y, int z, long t, long l, long b, long r) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y, z, t, l, b, r);
	//}

	//DMImage(std::string title, int bytes, int x, int y) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y);
	//}

	//DMImage(std::string title, int bytes, int x, int y, long t, long l, long b, long r) : ROIListener(new DMListener), DataListener(new DMListener)
	//{
	//	fromReal(title, bytes, x, y, t, l, b, r);
	//}

	~DMImage() { RemoveDataListener(); RemoveROIListener(); }

	void fromFront();

	//void fromReal(std::string title, int bytes, int x, int y, int z);
	//void fromReal(std::string title, int bytes, int x, int y, int z, long t, long l, long b, long r);

	//void fromReal(std::string title, int bytes, int x, int y);
	//void fromReal(std::string title, int bytes, int x, int y, long t, long l, long b, long r);

	//void Reshape(float Ratio, int x, int y);

	void DeleteImage() { DM::DeleteImage(Image); }

	void addROI(DMROI roi);

	void CreateROIListener();
	void AddROIListener();
	void RemoveROIListener();

	void CreateDataListener();
	void AddDataListener();
	void RemoveDataListener();

	void ClearListenables(){ (*ROIListener).ClearListeners(); (*ROIListener).ClearROIs(); }

	coord<long> GetWindowPosition() { long x, y; DM::GetWindowPosition(Image, &x, &y); return coord<long>(x, y); }
	coord<long> GetWindowSize() { long x, y; DM::GetWindowSize(Image, &x, &y); return coord<long>(x, y); }

	void SetWindowPosition(coord<long> pos) { DM::SetWindowPosition(Image, pos.x, pos.y); }
	void SetWindowSize(coord<long> size) { DM::SetWindowSize(Image, size.x, size.y); }

	// make these return a vector?



	//template <typename T>
	//void GetData(std::vector<T> &destination) { GetData(destination, 0, 0, height, width, 0, 1); }

	//template <typename T>
	//void GetData(std::vector<T> &destination, DMROI crop) { GetData(destination, crop.t, crop.l, crop.b, crop.r, 0, 1); }

	template <typename T>
	void GetSliceData(std::vector<T> &destination) { GetSliceData(destination, 0, 1); }

	template <typename T>
	void GetSliceData(std::vector<T> &destination, int slice)
	{
		if (!(slice < getDepth()) || slice < 0)
			throw std::invalid_argument("Slice not within image depth");
		GetSliceData(destination, slice, slice+1);
	}

	template <typename T>
	void GetSliceData(std::vector<T> &destination, int front, int back);

	template <typename T>
	void SetSliceData(std::vector<T> &newdata) { SetSliceData(newdata, 0, 1); }

	template <typename T>
	void SetSliceData(std::vector<T> &newdata, int slice)
	{
		if (!(slice < getDepth()) || slice < 0)
			throw std::invalid_argument("Slice not within image depth");
		SetSliceData(newdata, slice, slice + 1);
	}

	template <typename T>
	void SetSliceData(std::vector<T> &newdata, int front, int back);

	//template <typename T>
	//void SetRealData(const std::vector<T> &source, int offset);
	//template <typename T>
	//void SetRealData(const std::vector<T> &source);

	//template <typename T>
	//void SetComplexData(const std::vector<T> &source, int offset, ShowComplex doComplex);
	//template <typename T>
	//void SetComplexData(const std::vector<T> &source, ShowComplex doComplex);

	unsigned long getID() const
	{
		return Image.GetID();
	}

	bool operator==(DMImage& RHS)
	{
		return getID() == RHS.getID();
	}

	bool IsOpen()
	{
		DM::ImageDocument temp = Image.GetOrCreateImageDocument();
		DM::Window tempwin = temp.GetWindow();
		bool t = false;
		// have been getting some sort of error here so lets just try
		try
		{
			t = tempwin.IsOpen();
		}
		catch (std::exception& ex)
		{
			t = false;
			// TODO: create a member of construction that is the window
			// this error is caused as we have tried to create a window from a closed image, instead of creating a window from an open image and then testing it.
			// DMresult << ex.what() << DMendl; // probably a null window error, basically means that it is closed?
		}
		catch (...)
		{
			t = false;
		}
		return t;
	}
	
private:

	//template <typename T>
	//void GetLockerData(std::vector<T> &destination, int t, int l, int b, int r, int front, int back);

	//template <typename U, typename T>
	//void SetRealLockerData(const std::vector<T> &source, int offset);

	//template <typename U, typename T>
	//void SetComplexLockerData(const std::vector<T> &source, int offeset, ShowComplex doComplex);

};

// This is where the template parts are implemented
#include "DMImage-ReadWrite.h"