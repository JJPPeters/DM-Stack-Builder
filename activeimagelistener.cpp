#include "activeimagelistener.h"

ActiveImageListener::ActiveImageListener(const DM::Image &img, ImageMenuEntry* par, boost::shared_ptr<StackMenu> mas): Gatan::PlugIn::ImageListener(img), master(mas), parent(par)
{
	DEBUG2("ActiveImageListener constructed");
	//imgObject = img;
	dcl = boost::make_shared<DataChangedListener>(this);
	dcl->AddDataChangedListener(img);
}

ActiveImageListener::~ActiveImageListener()
{
	DEBUG2("ActiveImageListener destroyed");
}

void ActiveImageListener::Destruct(const DM::Image &image)
{
	DEBUG2("ActiveImageListener::Destruct invoked");

	DetachFromImage(image); // THIS SEEMS TO BE CRUCIAL...
	dcl->RemoveDataChangedListener(image);
	dcl.reset();

	master->RemoveImage(image);
}

void ActiveImageListener::OnDataChanged(uint32 img_event_flags, const DM::Image &image)
{
	DEBUG2("ActiveImageListener::OnDataChanged invoked");
	parent->RunWork();
}