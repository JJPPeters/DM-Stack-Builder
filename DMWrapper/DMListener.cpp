#include "stdafx.h"
#include "DMListener.h"

void DMListener::AddListenable(DMListenable* listenable)
{
	boost::lock_guard<boost::mutex> lock(_mtx);
	//TODO: check if the item already exists in list, will need comparison operator
	Listeners.push_back(listenable);
}

void DMListener::RemoveListenable(DMListenable* listenable)
{
	boost::lock_guard<boost::mutex> lock(_mtx);
	//TODO: implement this
}

boost::shared_ptr<DMImageListener> DMImageListener::AddImageListener_Ptr(const DM::Image &img, const std::string &prefix)
{
	// create the new class
	boost::shared_ptr<DMImageListener> ptr(new DMImageListener(prefix));

	// this is some DM magic, absolutely no idea what is happening here
	ulong_ptr_t temp = DM::ImageAddEventListener
		(
		img
		, DM::NewAnonymousCppPtrScriptObject(ptr
		, dmscript::function_list
		(
		dmscript::dms_bindfn_2<cpp_method_traits_2<DMImageListener, void, uint32, const DM::Image &>
		, &DMImageListener::OnDataChanged>("OnDataChanged", dmscript::dms_func_attr_threadsafe)
		)
		)
		, "data_changed,slices_changed:OnDataChanged"
		, prefix
		);

	ptr->SetEventHandler(temp);
	ptr->SetActive(true);
	return ptr;
}

void DMImageListener::OnDataChanged(uint32 img_event_flags, const DM::Image &img)
{
	// BE CAREFUL!! The listener spams out 3 or 4 signals each time. This should be safe as the Process method checks to see if it is already working.

	DMresult << "Data changed. Is active: " << IsActive() << DMendl;

	// if mutex is locked then we just return, otherwise transfer to scope locked mutex
	if (!_mtx.try_lock() || !IsActive())
		return;
	boost::lock_guard<boost::mutex> lock(_mtx, boost::adopt_lock_t());

	DMDebug << "!!data listen!!" << DMendl;

	// call process function on all listenables attached to this instance
	for (int i = 0; i < Listeners.size(); i++)
		(Listeners[i])->Process();
}

void DMImageListener::AddImageEventListener(DM::Image &img)
{
	SetActive(false);
	boost::lock_guard<boost::mutex> lock(_mtx);

	boost::shared_ptr<DMImageListener> ptr = get_this_shared();

	// don't think we need the ptr->
	EventHandler = DM::ImageAddEventListener
		(
		img
		, DM::NewAnonymousCppPtrScriptObject(ptr
		, dmscript::function_list
		(
		dmscript::dms_bindfn_2<cpp_method_traits_2<DMImageListener, void, uint32, const DM::Image &>
		, &DMImageListener::OnDataChanged>("OnDataChanged", dmscript::dms_func_attr_threadsafe)
		)
		)
		, "data_changed,slices_changed:OnDataChanged"
		, fPrefix
		);

	SetActive(true);
}

void DMImageListener::RemoveImageEventListener(DM::Image &image)
{
	DMDebug << "Remove image listener." << DMendl;
	boost::lock_guard<boost::mutex> lock(_mtx);
	DMDebug << "got lock" << DMendl;
	try
	{
		if (IsActive())
		{
			DMDebug << "listener is active" << DMendl;
			SetActive(false);
			DMDebug << "image name: " << image.GetName() << DMendl;
			DM::ImageRemoveEventListener(image, EventHandler);
			DMDebug << "removed" << DMendl;
		}
	}
	catch (const std::exception& ex)
	{
		DMresult << "Error: Removing image event listener: " << ex.what() << DMendl;
		return;
	}
}






boost::shared_ptr<DMDisplayListener> DMDisplayListener::AddImageDisplayListener_Ptr(const DM::ImageDisplay &img_disp, const std::string &prefix)
{
	boost::shared_ptr<DMDisplayListener> ptr(new DMDisplayListener(prefix));

	ulong_ptr_t temp = DM::ImageDisplayAddEventListener
		(
		img_disp
		, DM::NewAnonymousCppPtrScriptObject(ptr
		, dmscript::function_list
		(
		dmscript::dms_bindfn_2<cpp_method_traits_5<DMDisplayListener, void, uint32, const DM::ImageDisplay &, uint32, uint32, const DM::ROI &>
		, &DMDisplayListener::OnROIChanged>("OnROIChanged", dmscript::dms_func_attr_threadsafe)
		)
		)
		, "roi_changed:OnROIChanged"
		, prefix
		);

	ptr->SetEventHandler(temp);
	ptr->SetActive(true);
	return ptr;
}

void DMDisplayListener::OnROIChanged(uint32 img_disp_event_flags, const DM::ImageDisplay &img_disp, uint32 roi_change_flag, uint32 roi_disp_change_flags, const DM::ROI &roi)
{
	// BE CAREFUL!! The listener spams out 3 or 4 signals each time. This should be safe as the Process method checks to see if it is already working.

	if (!_mtx.try_lock() || !IsActive())
		return;
	boost::lock_guard<boost::mutex> lock(_mtx, boost::adopt_lock_t());

	for (int i = 0; i < ROIs.size(); i++)
	{
		if ((*(ROIs[i])).roi.GetID() == roi.GetID())
		{
			(ROIs[i])->Process();
		}
	}

	for (int i = 0; i < Listeners.size(); i++)
	{
		(Listeners[i])->Process();
	}
}

void DMDisplayListener::AddDisplayEventListener(DM::ImageDisplay &disp)
{
	SetActive(false);
	boost::lock_guard<boost::mutex> lock(_mtx);

	boost::shared_ptr<DMDisplayListener> ptr = get_this_shared();

	EventHandler = DM::ImageDisplayAddEventListener
		(
		disp
		, DM::NewAnonymousCppPtrScriptObject(ptr
		, dmscript::function_list
		(
		dmscript::dms_bindfn_2<cpp_method_traits_5<DMDisplayListener, void, uint32, const DM::ImageDisplay &, uint32, uint32, const DM::ROI &>
		, &DMDisplayListener::OnROIChanged>("OnROIChanged", dmscript::dms_func_attr_threadsafe)
		)
		)
		, "roi_changed:OnROIChanged"
		, fPrefix
		);

	SetActive(true);
}

void DMDisplayListener::RemoveDisplayEventListener(DM::ImageDisplay &disp)
{
	boost::lock_guard<boost::mutex> lock(_mtx);
	try
	{
		if (IsActive())
		{
			SetActive(false);
			DM::ImageDisplayRemoveEventListener(disp, EventHandler);
		}
	}
	catch (const std::exception& ex)
	{
		DMresult << "Error: Removing display event listener: " << ex.what() << DMendl;
		return;
	}
}

void DMDisplayListener::AddROI(DMROI* roi)
{
	boost::lock_guard<boost::mutex> lock(_mtx);
	ROIs.push_back(roi);
}



