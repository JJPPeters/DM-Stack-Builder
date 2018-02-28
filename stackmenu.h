#pragma once

#include "stdafx.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "activeimagelistener.h"
#include "imagemenuentry.h"

class ActiveImageListener; // to avoid circular dependency problems
class ImageMenuEntry;
class BuilderGeneric;
template <class T> class Builder;



class StackMenu : public DM::I_MenuBarListener, public boost::enable_shared_from_this<StackMenu>
{
	_GATAN_BEGIN_OBJECT_EXTENSION_MAP(StackMenu)
	_GATAN_OBJECT_EXTENSION_ENTRY(DM::I_MenuBarListener)
	_GATAN_END_OBJECT_EXTENSION_MAP

private:
	DM::MenuPtr		TopMenu, ImagesMenu, ModeMenu;
	DM::MenuItemPtr	AddImage, fMenuItem2, fMenuItem3, itemFixed, itemCircular, itemExpanding, itemStackSize;
	ulong_ptr_t		fMenuBarListenerID;

	//std::vector<boost::shared_ptr<ImageMenuEntry>> ImageEntries;
	std::vector<boost::shared_ptr<BuilderGeneric>> ImageEntries;

	void RemoveIndex(int index);

	boost::shared_ptr<BuilderGeneric> CreateTemplateBuilder(DM::Image &image);

	unsigned long stack_size;
	bool isFixed, isCircular, isExpanding;

public:
	StackMenu() : stack_size(3), isFixed(true), isCircular(false), isExpanding(false) {}
	virtual ~StackMenu() {};

	void RemoveImage(const DM::Image &img);

	//void RemoveImageTEST(const DM::Image &img) { DEBUG1(img.GetName()); }

	void InstallMenu();
	void UninstallMenu();

	void ImageCommandAction();
	void Command2Action();
	void Command3Action();

	void setFixed()
	{
		isFixed = true; isCircular = false; isExpanding = false;
		DM::TagGroupSetTagAsString(DM::GetPersistentTagGroup(), "Stack builder:Mode", "Fixed");
	}
	void setCircular()
	{
		isFixed = false; isCircular = true; isExpanding = false;
		DM::TagGroupSetTagAsString(DM::GetPersistentTagGroup(), "Stack builder:Mode", "Circular");
	}
	void setExpanding() 
	{ 
		isFixed = false; isCircular = false; isExpanding = true;
		DM::TagGroupSetTagAsString(DM::GetPersistentTagGroup(), "Stack builder:Mode", "Expanding");
	}

	void setStackSize()
	{
		std::string prompt = "Enter in the number of frames to use";
		float num_out;
		DM::GetNumber(prompt.c_str(), (float)stack_size, &num_out);
		if ((unsigned long)num_out > 0)
		{
			stack_size = (unsigned long)num_out;
			DM::TagGroupSetTagAsUInt32(DM::GetPersistentTagGroup(), "Stack builder:stack size", stack_size);
		}
	}

	// I_MenuBarListener methods
	virtual void OnAddedMenuItem(uint32 event_mask, const DM::MenuBarPtr &menu_bar, const DM::MenuPtr &par_menu, const DM::MenuItemPtr &menu_item);
	virtual void OnRemovedMenuItem(uint32 event_mask, const DM::MenuBarPtr &menu_bar, const DM::MenuPtr &par_menu, const DM::MenuItemPtr &menu_item);
	virtual void OnAboutToOpenMenuBar(uint32 event_mask, const DM::MenuBarPtr &menu_bar);
	virtual void OnMenuBarRebuilt(uint32 event_mask, const DM::MenuBarPtr &menu_bar);
};
