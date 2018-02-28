#pragma once
#include "stdafx.h"
#include "activeimagelistener.h"
#include "stackmenu.h"
#include "threadworker.h"
#include "dmimage.h"
#include <boost/cstdint.hpp>

#include <limits> // for nan
#include <boost/lexical_cast.hpp>
#include <boost/chrono.hpp>

class StackMenu;
class ActiveImageListener;

class ImageMenuEntry : public ThreadWorker
{
protected:
	DM::Image image;
	DM::MenuPtr image_entry;
	DM::MenuItemPtr image_entry_remove;
	boost::shared_ptr<ActiveImageListener> image_listener;
	DM::MenuPtr parent_menu;

public:
	ImageMenuEntry(const DM::Image &img, DM::MenuPtr par_menu, boost::shared_ptr<StackMenu> menu_control);
	~ImageMenuEntry();

	bool operator==(const DM::Image& other)
	{
		return image == other;
	}
	
};

// purely virtual, but allows the templated class to be accessed without knowing type
class BuilderGeneric : public ImageMenuEntry
{
public:

	BuilderGeneric(const DM::Image &img, DM::MenuPtr par_menu, boost::shared_ptr<StackMenu> menu_control) : ImageMenuEntry(img, par_menu, menu_control) {};
	virtual ~BuilderGeneric() {};

	virtual void StopBuild() = 0;

	virtual void StartBuildingFixed(int slices) = 0;
	virtual void StartBuildingExpanding(int initial_slices) = 0;
	virtual void StartBuildingCircular(int slices) = 0;

	virtual void DoWork() = 0;

	virtual bool IsBuilding() = 0;
};

// template class that handles the type related stuff
template <class T>
class Builder : public BuilderGeneric
{
private:
	enum BuildMode {
		Fixed,
		Circular,
		Expanding
	};

	DM::Image build_image;
	bool image_created, is_building;

	T last_pixel;
	unsigned long next_slice;

	BuildMode mode;

	unsigned long expand_size;
	unsigned long current_size;

	boost::chrono::high_resolution_clock::time_point time_last;
public:

	bool IsBuilding()
	{
		return is_building;
	}

	void StopBuild()
	{
		is_building = false;
		image_created = false;
		Pause(false);
		//build_image = DM::Image();
		build_image.release();
	}

	void StartBuildingFixed(int slices) { mode = BuildMode::Fixed; is_building = true; image_created = false; expand_size = slices; current_size = slices; next_slice = 0; }
	void StartBuildingExpanding(int slices) { mode = BuildMode::Expanding; is_building = true; image_created = false; expand_size = slices; current_size = slices; next_slice = 0; }
	void StartBuildingCircular(int slices){ mode = BuildMode::Circular; is_building = true; image_created = false; expand_size = slices; current_size = slices; next_slice = 0; }

	Builder(const DM::Image &img, DM::MenuPtr par_menu, boost::shared_ptr<StackMenu> menu_control) : BuilderGeneric(img, par_menu, menu_control)
	{
		mode = BuildMode::Fixed;
		expand_size = 3;

		next_slice = 0;
		current_size = expand_size;
		is_building = false;
		image_created = false;
		last_pixel = std::numeric_limits<T>::quiet_NaN(); // this will make the image acquire whatever is currently displayed when you start it
	}

	void CreateBuildImage(std::string name, unsigned long x, unsigned long y, unsigned long z)
	{
		DEBUG1("Builder::CreateBuildImage invoked");
		build_image = CreateImage(name, x, y, z);
		image_created = true;

		//build_image.GetTagGroup().GetOrCreateTagGroup("Stack builder").GetOrCreateTagGroup("INDEX GOES HERE").CreateNewLabeledTag()

		//DM::TagGroupSetTagAsString(tg, "Stack builder:")

		// get image dimensions
		long px, py, sx, sy;
		DM::Window win = image.GetOrCreateImageDocument().GetWindow();
		DM::WindowGetFramePosition(win, &px, &py);
		DM::WindowGetFrameSize(win, &sx, &sy);
		//DM::WindowGet

		//Gatan::DM::GetWindowPosition(image, &px, &py); // this didn't seem consistent, maybe someting to do with the multiple monitors??
		/*Gatan::DM::GetWindowSize(image, &sx, &sy);*/

		DEBUG1("Builder::CreateBuildImage - Watched image position is (" << px << ", " << py << ")");
		DEBUG1("Builder::CreateBuildImage - Watched image size is (" << sx << ", " << sy << ")");

		// TODO: don't just copy size, make it so the new window is the size needed (otherwise we will copy over silly image dimensions)

		// test if image will be off the screen...
		long t, l, b, r;
		Gatan::DM::ApplicationGetBounds(&t, &l, &b, &r); // this is the 'viewable' image area (i.e. not the main window rect)

		DEBUG1("Builder::CreateBuildImage - Application rect is (t, l, b, r) (" << t << ", " << l << ", " << b << ", " << r << ")");

		int shift = sx;
		int pad = 40; // number is arbitrary, just set so it gives a nice area to click and drag window
		if (px + sx + pad > r)
			shift = (r - px) - pad;

		// show to right of the image we are getting data from
		//build_image.GetOrCreateImageDocument().ShowAtRect(py, px+shift, py+sy, px+shift+sx);
		long min_pos = std::numeric_limits<long>::min();
		build_image.GetOrCreateImageDocument().ShowAtPosition(min_pos, min_pos); // show the image miles off so we don't get some odd artifacts when it is shown
		DM::Window win2 = build_image.GetOrCreateImageDocument().GetWindow();
		DM::WindowSetFrameSize(win2, sx, sy);
		DM::WindowSetFramePosition(win2, px+shift, py);
		DEBUG1("Builder::CreateBuildImage - Made the stack image");
		//DM::Window win2 = build_image.GetOrCreateImageDocument().GetWindow();


	}

	DM::Image CreateImage(std::string name, unsigned long x, unsigned long y, unsigned long z)
	{
		DataType::DataTypes dtype = static_cast<DataType::DataTypes>(image.GetDataType());

		DM::Image temp_image;

		switch (dtype)
		{
		case DataType::BOOL:
			temp_image = DM::BinaryImage(name.c_str(), x, y, z);
			break;
		case DataType::INT_1:
			temp_image = DM::IntegerImage(name.c_str(), 1, false, x, y, z);
			break;
		case DataType::INT_2:
			temp_image = DM::IntegerImage(name.c_str(), 2, false, x, y, z);
			break;
		case DataType::INT_4:
			temp_image = DM::IntegerImage(name.c_str(), 4, false, x, y, z);
			break;
		case DataType::UINT_1:
			temp_image = DM::IntegerImage(name.c_str(), 1, true, x, y, z);
			break;
		case DataType::UINT_2:
			temp_image = DM::IntegerImage(name.c_str(), 2, true, x, y, z);
			break;
		case DataType::UINT_4:
			temp_image = DM::IntegerImage(name.c_str(), 4, true, x, y, z);
			break;
		case DataType::FLOAT:
			temp_image = DM::RealImage(name, 4, x, y, z);
			break;
		case DataType::DOUBLE:
			temp_image = DM::RealImage(name, 8, x, y, z);
			break;
		case DataType::COMPLEX_FLOAT:
			temp_image = DM::ComplexImage(name.c_str(), 8, x, y, z);
			break;
		case DataType::COMPLEX_DOUBLE:
			temp_image = DM::ComplexImage(name.c_str(), 16, x, y, z);
			break;
		case DataType::RGB:
			DMResult << "RGB images are not supported yet." << std::endl; // possible throw?
			break;
		default:
			DMResult << "I don't know this image type." << std::endl;
			break;
		}

		return temp_image;
	}

	void ExpandBuildImage()
	{
		DEBUG1("Builder::ExpandBuildImage invoked");
		// create the new image	
		std::string name = build_image.GetName();
		unsigned long x, y, z;
		build_image.GetDimensionSizes(x, y, z);
		unsigned long new_z = z + expand_size;

		DEBUG1("Builder::ExpandBuildImage - New size is " << new_z);

		DM::Image new_build = CreateImage(name, x, y, new_z);

		new_build.GetOrCreateImageDocument().Show();
		new_build.GetOrCreateImageDocument().Hide();

		DEBUG1("Builder::ExpandBuildImage - Created the new image");

		// copy data across
		Gatan::PlugIn::ImageDataLocker iLocker_current = Gatan::PlugIn::ImageDataLocker(build_image);
		T* idata_current = static_cast<T*>(iLocker_current.get());

		Gatan::PlugIn::ImageDataLocker iLocker_new = Gatan::PlugIn::ImageDataLocker(new_build);
		T* idata_new = static_cast<T*>(iLocker_new.get());

		std::memcpy(idata_new, idata_current, x * y * z * sizeof(T));
		
		DEBUG1("Builder::ExpandBuildImage - Copied data to new image");

		// copy tags
		new_build.GetTagGroup().CopyTagsFrom(build_image.GetTagGroup());
		DEBUG1("Builder::ExpandBuildImage - Copied tags to new image");

		// copy calibration
		new_build.CopyCalibrationFrom(build_image);
		DEBUG1("Builder::ExpandBuildImage - Copied calibration to new image");

		// copy image position/size
		long px, py, sx, sy;
		Gatan::DM::GetWindowPosition(build_image, &px, &py);
		Gatan::DM::GetWindowSize(build_image, &sx, &sy);

		// Get current slice view
		long start, end;
		build_image.GetImageDisplay(0).GetDisplayedLayers(&start, &end);

		// set the same slice view as before
		new_build.GetImageDisplay(0).SetDisplayedLayers(start, end);
		DEBUG1("Builder::ExpandBuildImage - Set current viewed slice");

		// delete old image
		DM::DeleteImage(build_image);
		DEBUG1("Builder::ExpandBuildImage - Deleted the original stack");

		// show image at same position using rect
		new_build.GetOrCreateImageDocument().ShowAtRect(py, px, py+sy, px+sx);
		DEBUG1("Builder::ExpandBuildImage - Showed new image at old position");

		// finally update our class member
		build_image = new_build;

		// update our size so we will resize again when needed
		current_size = new_z;
		DEBUG1("Builder::ExpandBuildImage - Updated class members");
	}

	// this is the bit that is run in the thread
	void DoWork()
	{
		DEBUG1("Builder::DoWork invoked");
		// get last pixel (do this all the time so it is up to date
		// not sure if this has some sort of performance penalty....

		unsigned long xlp, ylp;
		image.GetDimensionSizes(xlp, ylp);
		unsigned long szlp = xlp * ylp;

		Gatan::PlugIn::ImageDataLocker iLocker_lp = Gatan::PlugIn::ImageDataLocker(image, Gatan::PlugIn::ImageDataLocker::LockDataFlags::lock_data_WONT_WRITE);
		T* idata_lp = static_cast<T*>(iLocker_lp.get());
		T new_last = idata_lp[szlp - 1];
		DEBUG1("Builder::DoWork - Last pixel was " << last_pixel << ", now is " << new_last);

		// test if it has changed
		if (new_last == last_pixel)
		{
			DEBUG1("Builder::DoWork - Last pixel hasn't changed");
			return; // nothing to do here
		}

		// set the last pixel for next time
		last_pixel = new_last;

		if (!is_building)
		{
			DEBUG1("Builder::DoWork - Not building yet");
			return; // nothing to do here
		}

		// get the detail from the image (move to create new image fucntion?)
		std::string n = image.GetName();

		// TODO: could create image on when adding the image, but don't show it until it is started
		if (!image_created)
		{
			unsigned long x, y;
			image.GetDimensionSizes(x, y);
			DEBUG1("Builder::DoWork - Making a build image for " + n);
			CreateBuildImage(n + " stack", x, y, expand_size); // x, y defined at top now
			time_last = boost::chrono::high_resolution_clock::now();
		}

		if (!build_image.IsValid() || build_image.CountImageDisplays() < 1)
		{
			DEBUG1("Builder::DoWorkBuild - Stack image not valid");
			is_building = false;
			image_created = false;
			return;
		}

		if (next_slice >= current_size)
		{
			DEBUG1("Builder::DoWork - Trying to add data to slice that does not exist");
			return;
		}

		if (IsPaused())
		{
			DEBUG1("Builder::DoWork - Is paused");
			return;
		}

		if (IsStopping())
		{
			DEBUG1("Builder::DoWork - Is stopping");
			is_building = false;
			image_created = false;
			return;
		}

		DEBUG1("Builder::DoWork - Doing the building for " << n);

		// copy data to 'next_slice'
		{
			// slice we are monitoring
			// TODO: is there any problem with opening this locker twice?
			//Gatan::PlugIn::ImageDataLocker iLocker_live = Gatan::PlugIn::ImageDataLocker(image);
			//T* idata_live = static_cast<T*>(iLocker_lp.get());
			T* idata_live = idata_lp;

			// image to copy to
			Gatan::PlugIn::ImageDataLocker iLocker_stack = Gatan::PlugIn::ImageDataLocker(build_image);
			T* idata_stack = static_cast<T*>(iLocker_stack.get());

			unsigned long x, y;
			image.GetDimensionSizes(x, y);
			unsigned long sz = x * y;

			unsigned long slice_ind = next_slice * sz;
			unsigned long n_bytes = sz * sizeof(T);

			// copy the data over
			std::memcpy(idata_stack + slice_ind, idata_live, n_bytes);
			DEBUG1("Builder::DoWork - Copied memory across");
			
			boost::chrono::high_resolution_clock::time_point t_now = boost::chrono::high_resolution_clock::now();
			boost::chrono::duration<double> time_span = boost::chrono::duration_cast<boost::chrono::duration<double>>(t_now - time_last);
			
			double tim = time_span.count();
			time_last = t_now;

			DEBUG1("Builder::DoWork - Got time of current slice");

			DM::TagGroup tg = build_image.GetTagGroup();
			DM::TagGroup sb_tg = tg.GetOrCreateTagGroup("Stack builder");
			DM::TagGroup sb_i_tg = sb_tg.GetOrCreateTagGroup(boost::lexical_cast<std::string>(next_slice));
			sb_i_tg.SetTagAsDouble("Clock time", tim);

			sb_tg.SetTagAsLong("Last update", next_slice);
			//std::string tag = "Stack builder:" + boost::lexical_cast<std::string>(next_slice) + ":Clock time";
			//double time = time_span.count();
			//DM::TagGroupSetTagAsDouble(tg, tag.c_str(), time);

			// not sure copying the cotrast limits is strictly needed, but meh
			float low, high;
			image.GetImageDisplay(0).GetContrastLimits(&low, &high);
			build_image.GetImageDisplay(0).SetContrastLimits(low, high);

			DEBUG1("Builder::DoWork - Updated stack image");
		}

		DEBUG1("Builder::DoWork - Set data changed");
		build_image.DataChanged();
		DEBUG1("Builder::DoWork - Set visible layer");
		build_image.GetImageDisplay(0).SetDisplayedLayers(next_slice, next_slice);
		// increment the next slice
		++next_slice;

		// need to handle the different modes here

		// if just one run, exit and stop build if we are at out stack limit
		// if we are looping, reset index
		// if we are expanding, create new build image
		if (next_slice >= current_size)
		{
			DEBUG1("Builder::DoWork - At end of current stack");
			if (mode == BuildMode::Fixed)
			{
				DEBUG1("Builder::DoWork - Stoping");
				StopBuild();
			}
			else if (mode == BuildMode::Circular)
			{
				next_slice = 0;
				DEBUG1("Builder::DoWork - Reset slice");
			}
			else if (mode == BuildMode::Expanding)
			{
				ExpandBuildImage();
				DEBUG1("Builder::DoWork - Resized image");
			}
		}

	}
};