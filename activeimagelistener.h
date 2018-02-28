#pragma once
#include "stdafx.h"

#include "stackmenu.h"
#include "datachangedlistener.h"
#include "imagemenuentry.h"

class StackMenu; // to avoid circular dependency problems
class DataChangedListener;
class ImageMenuEntry;


class ActiveImageListener : public Gatan::PlugIn::ImageListener
{
private:
	 boost::shared_ptr<StackMenu> master;
	 ImageMenuEntry* parent;

	 boost::shared_ptr<DataChangedListener> dcl;

public:
	ActiveImageListener(const DM::Image &img, ImageMenuEntry* par, boost::shared_ptr<StackMenu> mas);
	~ActiveImageListener();

	void Destruct(const DM::Image &image);
	
	void OnDataChanged(uint32 img_event_flags, const DM::Image &image);

	// This is purely for debugging...
	bool Key(const DM::Image &image, long k, long m) { DEBUG2("ActiveImageListener::Key invoked"); return true; }
};

