#include "datachangedlistener.h"

void DataChangedListener::AddDataChangedListener(const DM::Image &image)
{
	// create the new class
	boost::shared_ptr<DataChangedListener> self_ptr = shared_from_this(); // WARNING: for this to work, there needs to be a shared pointer to this class instance somewhere...

	DataChangedHandler = DM::ImageAddEventListener(
		image,
		DM::NewAnonymousCppPtrScriptObject(
			self_ptr,
			dmscript::function_list(
				dmscript::dms_bindfn_2<cpp_method_traits_2<DataChangedListener, void, uint32, const DM::Image &>,
				&DataChangedListener::OnDataChanged>("OnDataChanged", dmscript::dms_func_attr_threadsafe)
			)
		),
		"data_changed,slices_changed:OnDataChanged",
		"StackPlugIn"
	);
}


void DataChangedListener::RemoveDataChangedListener(const DM::Image &image)
{
	DM::ImageRemoveEventListener(image, DataChangedHandler);
}


void DataChangedListener::OnDataChanged(uint32 img_event_flags, const DM::Image &image)
{
	parent->OnDataChanged(img_event_flags, image);
}
