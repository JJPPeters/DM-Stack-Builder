#include "imagemenuentry.h"

ImageMenuEntry::ImageMenuEntry(const DM::Image &img, DM::MenuPtr par_menu, boost::shared_ptr<StackMenu> menu_control) : image(img), parent_menu(par_menu)
{
	std::string label = image.GetLabel();
	std::string name = image.GetName();
	image_entry = DM::NewMenu(label + ": " + name);

	image_entry_remove = DM::NewMenuItem("Remove");
	image_entry_remove->SetAction(boost::bind(&StackMenu::RemoveImage, menu_control, image));
	image_entry->AddMenuItem(image_entry_remove);

	parent_menu->AddMenuItem(static_pointer_cast<DM::MenuItemPtr>(image_entry));

	image_listener = boost::make_shared<ActiveImageListener>(image, this, menu_control);

	Start();
}

ImageMenuEntry::~ImageMenuEntry()
{
	//image_listener.reset();
	Stop();

	image_entry->RemoveMenuItem(image_entry_remove);
	image_entry_remove = DM::MenuItemPtr();

	parent_menu->RemoveMenuItem(static_pointer_cast<DM::MenuItemPtr>(image_entry));
	image_entry = DM::MenuPtr();

	// might need to delete image and image_entry here (should be handled by destructor though?
}