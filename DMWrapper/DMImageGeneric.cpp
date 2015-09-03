#include "stdafx.h"

#include "DMImageGeneric.h"

void DMImageGeneric::fromFront()
{
	int success = DM::GetFrontImage(Image);

	if (!success)
		throw std::invalid_argument("Cannot get front image");
	// is there ever an index != 0?
	Display = Image.GetImageDisplay(0);
	Image.GetDimensionSizes(width, height, depth);
	Image.GetDimensionSizes();
	CreateDataListener();
}

void DMImageGeneric::fromType(std::string title, int x, int y, int z, DataType::DataTypes dtype)
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

void DMImageGeneric::Show()
{
	Image.GetOrCreateImageDocument().Show();
	Image.GetOrCreateImageDocument().Clean(); // not sure if this is needed
}

void DMImageGeneric::ShowAt(int t, int l, int b, int r)
{
	Image.GetOrCreateImageDocument().ShowAtRect(t, l, b, r);
	Image.GetOrCreateImageDocument().Clean();
}

void DMImageGeneric::Reshape(int x, int y, int z)
{
	coord<long> pos = GetWindowPosition();
	coord<long> size = GetWindowSize();

	std::string Title = Image.GetName();
	long Bytes = Image.GetDataElementBitSize() / 8;

	DataType::DataTypes dtype = static_cast<DataType::DataTypes>(getDataType());

	RemoveDataListener(); // Causes crashes if listener hasn't been created
	RemoveROIListener();

	// Ideall would copy data down
	// copy data down

	DeleteImage(); // Doesnt delete ROIs but they need re-adding

	fromType(Title, x, y, z, dtype);

	// copy data to new image

	ShowAt(pos.y, pos.x, pos.y + size.y, pos.x + size.x);

	AddROIListener();
	AddDataListener();

	// transfer new data

	//for (int i = 0; i < ROIs.size(); i++)
	//{
	//	ROIs[i]->scaleRect(Ratio);
	//	ROIs[i]->addROI(Display);
	//}
}

void DMImageGeneric::addROI(DMROI roi)
{
	ROIs.push_back(boost::make_shared<DMROI>(roi));

	if (!(*ROIListener).IsActive())
		CreateROIListener();

	(*ROIListener).addROI(ROIs.back().get());
}

void DMImageGeneric::CreateROIListener()
{
	ROIListener = DMListener::AddImageDisplayListener_Ptr(Display, "ROIListener");
}

void DMImageGeneric::RemoveROIListener()
{
	ROIListener->RemoveDisplayEventListener(Display);
}

void DMImageGeneric::AddROIListener()
{
	ROIListener->AddDisplayEventListener(Display);
}

void DMImageGeneric::CreateDataListener()
{
	DataListener = DMListener::AddImageListener_Ptr(Image, "DataListener");
}

void DMImageGeneric::RemoveDataListener()
{
	DataListener->RemoveImageEventListener(Image);
}

void DMImageGeneric::AddDataListener()
{
	DataListener->AddImageEventListener(Image);
}

bool DMImageGeneric::IsOpen()
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