#include "stdafx.h"

#include "DMDialog.h"
#include "StackBuilder.h"

#include "DMWrapper/DMout.h"

#include <dos.h> // for sleep

// i had to copy the uint from visual studio 2010 and newer include folder
#include <cstdint>

void StackBuilder::Process()
{
	boost::lock_guard<boost::mutex> lock(*dialogmtx);
	Start();
}

void StackBuilder::DoWork()
{
	DMresult << "Doing my listen, " << DMendl;

	unsigned long dtype = watchedImage.getDataType();

	//if
	DMresult << "got type as: " << dtype << DMendl;

	switch (dtype)
	{
	case DataType::BOOL:
		BuildStack<bool>();
		break;
	case DataType::INT_1:
		BuildStack<int8_t>();
		break;
	case DataType::INT_2:
		BuildStack<int16_t>();
		break;
	case DataType::INT_4:
		BuildStack<int32_t>();
		break;
	case DataType::UINT_1:
		BuildStack<uint8_t>();
		break;
	case DataType::UINT_2:
		BuildStack<uint16_t>();
		break;
	case DataType::UINT_4:
		BuildStack<uint32_t>();
		break;
	case DataType::FLOAT:
		BuildStack<float>();
		break;
	case DataType::DOUBLE:
		BuildStack<double>();
		break;
	case DataType::COMPLEX_FLOAT:
		BuildStack<std::complex<float>>();
		break;
	case DataType::COMPLEX_DOUBLE:
		BuildStack<std::complex<double>>();
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

	//// get current lastPixel;
	//// this template type depends on the type of the image
	//// basically this DoWork function needs to just get the data type and then call a template function to do the actual maths etc... (or just convert to a double for processing
	//double currentPixel = watchedImage.getElement<double>(sy-1, sx-1);

	//DMresult << "last: " << lastPixel << ", current: " << currentPixel << DMendl;

	//if (std::abs(std::abs(currentPixel) - std::abs(lastPixel)) > 1e-10)
	//{
	//	DMresult << "Last Pixel Changed" << DMendl;
	//}

	//lastPixel = currentPixel;
}

template <typename T>
void StackBuilder::BuildStack()
{
	int sy = watchedImage.getHeight();
	int sx = watchedImage.getWidth();

	// I think the precision will be ok for most cases?
	numbertype currentPixel = watchedImage.getElement<T>(sy - 1, sx - 1);


	// does this work with variants?
	DMresult << "last: " << lastPixel << ", current: " << currentPixel << DMendl;

	if ( !boost::apply_visitor(are_strict_equals(), currentPixel, lastPixel))
	{
		DMresult << "Last Pixel Changed" << DMendl;
	}

	lastPixel = currentPixel;

	// if label to do auto update.. (or label to create certain amount)
	// if changed then...
}