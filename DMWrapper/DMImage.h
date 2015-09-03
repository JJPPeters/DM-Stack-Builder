#pragma once

#include "DMImageGeneric.h"

template <typename T>
class DMImage : public DMImageGeneric
{
public:

	DMImage() : DMImageGeneric() {}

	// copy generic to the typed version
	DMImage(const DMImageGeneric& other) : DMImageGeneric(other) { }

	// not sure if returning by reference is dodgy?
	T &getElement(int j, int i)
	{
		Gatan::PlugIn::ImageDataLocker iLocker = Gatan::PlugIn::ImageDataLocker(Image);
		T* idata = static_cast<T*>(iLocker.get());

		return idata[j * width + i];
	}

	void GetData(std::vector<T> &destination);

	// want versions that return a vector
	void GetSliceData(std::vector<T> &destination) { GetSliceData(destination, 0, 1); }

	void GetSliceData(std::vector<T> &destination, int slice)
	{
		if (!(slice < getDepth()) || slice < 0)
			throw std::invalid_argument("Slice not within image depth");
		GetSliceData(destination, slice, slice+1);
	}

	void GetSliceData(std::vector<T> &destination, int front, int back);

	void SetData(std::vector<T> &newdata)

	void SetSliceData(std::vector<T> &newdata) { SetSliceData(newdata, 0, 1); }

	void SetSliceData(std::vector<T> &newdata, int slice)
	{
		if (!(slice < getDepth()) || slice < 0)
			throw std::invalid_argument("Slice not within image depth");
		SetSliceData(newdata, slice, slice + 1);
	}

	void SetSliceData(std::vector<T> &newdata, int front, int back);
};

// This is where the template parts are implemented
#include "DMImage-ReadWrite.h"