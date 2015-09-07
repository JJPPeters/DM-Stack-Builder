#include "stdafx.h"

#include "DMImageGeneric.h"

void DMImageGeneric::fromFront()
{
	int success = DM::GetFrontImage(Image);

	if (!success)
		throw std::invalid_argument("Cannot get front image");
	Display = Image.GetImageDisplay(0);
	Image.GetDimensionSizes(width, height, depth);
	NewDataListener();
}

void DMImageGeneric::MakeFromType(std::string title, int x, int y, int z, DataType::DataTypes dtype)
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
		DMresult << "RGB images are not supported yet." << DMendl; // possible throw?
		return;
		break;
	default:
		DMresult << "I don't know this image type." << DMendl;
		return;
		break;
	}
}

// need to consider methods to show image when creating (maybe an overload with a boolean?)
// and is it possible to get the display without showing the image
void DMImageGeneric::fromType(std::string title, int x, int y, int z, DataType::DataTypes dtype)
{
	MakeFromType(title, x, y, z, dtype);
	NewDataListener();
	// image might need to be shown to get the display
	Image.GetOrCreateImageDocument().Show();
	Display = Image.GetImageDisplay(0);
	Image.GetOrCreateImageDocument().Hide();
	Image.GetDimensionSizes(width, height, depth);
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

//TODO: test if the get/set displayed layers function works with non stack images
void DMImageGeneric::Reshape(int x, int y, int z)
{
	coord<long> pos = GetWindowPosition();
	coord<long> size = GetWindowSize();

	std::string Title = Image.GetName();
	long Bytes = Image.GetDataElementBitSize() / 8;

	DataType::DataTypes dtype = static_cast<DataType::DataTypes>(getDataType());

	RemoveDataListener(); // Causes crashes if listener hasn't been created
	RemoveROIListener();

	// Ideall would copy data down, but we don't know the type in the generic

	long start, end;

	Display.GetDisplayedLayers(&start, &end);

	DeleteImage(); // Doesnt delete ROIs but they need re-adding

	MakeFromType(Title, x, y, z, dtype);

	// copy data to new image

	//reset stack position
	ShowAt(pos.y, pos.x, pos.y + size.y, pos.x + size.x);

	// does show need to have been called for this to work?
	Display = Image.GetImageDisplay(0);

	Display.SetDisplayedLayers(start, end);

	// test that these get readded in a sensible way
	AddROIListener(); // might jsut need to reset ptr properties instead?
	AddDataListener(); // not sure it actually matters, might just need to re-add listenables (copy over vector of <*DMlistenables>)

	// transfer new data

	// ill reimplement this when i need it, dose not fit with new workflow
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
		NewROIListener();

	(*ROIListener).addROI(ROIs.back().get());
}

void DMImageGeneric::NewROIListener()
{
	ROIListener = DMListener::AddImageDisplayListener_Ptr(Display, "ROIListener");
}

void DMImageGeneric::RemoveROIListener()
{
	if (ROIListener)
		ROIListener->RemoveDisplayEventListener(Display);
}

void DMImageGeneric::AddROIListener()
{
	if (ROIListener)
		ROIListener->AddDisplayEventListener(Display);
}

void DMImageGeneric::NewDataListener()
{
	DataListener = DMListener::AddImageListener_Ptr(Image, "DataListener");
}

void DMImageGeneric::RemoveDataListener()
{
	if (DataListener)
		DataListener->RemoveImageEventListener(Image);
}

void DMImageGeneric::AddDataListener()
{
	if(DataListener)
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