#include "stackmenu.h"

///
/// The following function adds two menu items and a divider to
/// a top-level menu named 'MenuExample'
///
void StackMenu::InstallMenu()
{
	DEBUG2("StackMenu::InstallMenu invoked");

	boost::shared_ptr<StackMenu> ref_this_ptr = shared_from_this();

	// create the top level menu
	TopMenu = DM::NewMenu("Stack");

	// create 'Images' sub menu
	ImagesMenu = DM::NewMenu("Images");

	// Add the 'add' entry and bind it to the function it will use
	AddImage = DM::NewMenuItem("Add front");
	AddImage->SetAction(boost::bind(&StackMenu::ImageCommandAction, ref_this_ptr));
	AddImage->SetAccelerator(DM::MakeKeystroke("shift-alt-a"));
	ImagesMenu->AddMenuItem(AddImage);

	// add a separator
	ImagesMenu->AddMenuItem(DM::NewMenuSeparator());

	TopMenu->AddMenuItem(static_pointer_cast<DM::MenuItemPtr>(ImagesMenu)); // cast to pointer too add a menu

	TopMenu->AddMenuItem(DM::NewMenuSeparator());

	ModeMenu = DM::NewMenu("Mode");

	itemFixed = DM::NewMenuItem("Fixed");
	itemFixed->SetAction(boost::bind(&StackMenu::setFixed, ref_this_ptr));
	ModeMenu->AddMenuItem(itemFixed);

	itemCircular = DM::NewMenuItem("Circular");
	itemCircular->SetAction(boost::bind(&StackMenu::setCircular, ref_this_ptr));
	ModeMenu->AddMenuItem(itemCircular);

	itemExpanding = DM::NewMenuItem("Expanding");
	itemExpanding->SetAction(boost::bind(&StackMenu::setExpanding, ref_this_ptr));
	ModeMenu->AddMenuItem(itemExpanding);

	TopMenu->AddMenuItem(static_pointer_cast<DM::MenuItemPtr>(ModeMenu));

	itemStackSize = DM::NewMenuItem("Stack size: --");
	itemStackSize->SetAction(boost::bind(&StackMenu::setStackSize, ref_this_ptr));

	TopMenu->AddMenuItem(itemStackSize);

	TopMenu->AddMenuItem(DM::NewMenuSeparator());

	// Add an extra item as a pause (may be moved later)
	fMenuItem2 = DM::NewMenuItem("Pause");
	fMenuItem2->SetAction(boost::bind(&StackMenu::Command2Action, ref_this_ptr));
	fMenuItem2->SetAccelerator(DM::MakeKeystroke("shift-alt-d"));
	TopMenu->AddMenuItem(fMenuItem2);

	// Add an extra item as a stop (may be moved later)
	fMenuItem3 = DM::NewMenuItem("Start");
	fMenuItem3->SetAction(boost::bind(&StackMenu::Command3Action, ref_this_ptr));
	fMenuItem3->SetAccelerator(DM::MakeKeystroke("shift-alt-s"));
	TopMenu->AddMenuItem(fMenuItem3);

	// set everything we want to be enabled
	AddImage->SetIsEnabled(true);
	itemCircular->SetIsEnabled(true);
	itemFixed->SetIsEnabled(true);
	itemExpanding->SetIsEnabled(true);
	itemStackSize->SetIsEnabled(true);

	// add the top menu to the menu bar
	DM::MenuBarPtr menuBar = DM::GetMenuBar();
	menuBar->AddMenuItem(static_pointer_cast<DM::MenuItemPtr>(TopMenu));
	menuBar->RebuildMenuBar();

	// add listener for all menu bar events
	// I assume this is what calls the 'OnAboutToOpenMenuBar' method
	fMenuBarListenerID = menuBar->AddMenuBarListener(ref_this_ptr);

	DM::TagGroup PersistentTags = DM::GetPersistentTagGroup();

	DM::String _md;
	DM::TagGroupGetTagAsString(PersistentTags, "Stack builder:Mode", _md);
	std::string md(_md);

	if (md == "")
		md = "None";

	DEBUG1("StackMenu::InstallMenu - Found mode from tags: " << md);

	if (md == "Circular")
	{
		isFixed = false;
		isCircular = true;
		isExpanding = false;
	}
	else if (md == "Expanding")
	{
		isFixed = false;
		isCircular = false;
		isExpanding = true;
	}
	else // Fixed is our fallback
	{
		isFixed = true;
		isCircular = false;
		isExpanding = false;
	}

	//DM::TagGroupGetTagAsString(PersistentTags, "Stack builder:stack size");
	Gatan::ulong st_sz = 0;
	DM::TagGroupGetTagAsUInt32(PersistentTags, "Stack builder:stack size", &st_sz);

	if (st_sz > 0)
	{
		stack_size = st_sz;
		DEBUG1("StackMenu::InstallMenu - Found stack size from tags: " << stack_size);
	}
	else
	{
		stack_size = 3;
		DEBUG1("StackMenu::InstallMenu - No stack size tag, using default value: " << stack_size);
	}
}

///
/// The following function removes the plug-in defined menu form the
///  top level menu
///
void StackMenu::UninstallMenu()
{
	DM::MenuBarPtr menuBar = DM::GetMenuBar();
	if (menuBar != NULL)
	{
		menuBar->RemoveMenuBarListener(fMenuBarListenerID);
		menuBar->RemoveMenuItem(static_pointer_cast<DM::MenuItemPtr>(TopMenu));

		AddImage = DM::MenuItemPtr();
		fMenuItem2 = DM::MenuItemPtr();
		ImagesMenu = DM::MenuPtr();
		TopMenu = DM::MenuPtr();
	}
}

///
/// 'OnAddedMenuItem' is called whenever a menu item is added to the menu bar or any
/// of its submenus. If a menu is added that already contains menu items, the event
/// is only definitely dispatched for the added menu; it might or might not be called
/// for each of the submenus
///
void StackMenu::OnAddedMenuItem(uint32 event_mask, const DM::MenuBarPtr &menu_bar, const DM::MenuPtr &par_menu, const DM::MenuItemPtr &menu_item)
{
	DEBUG2("StackMenu::OnAddedMenuItem invoked");
}
///
/// 'OnRemovedMenuItem' is called whenever a menu item is removed from the menu bar
/// or any of its submenus. If a menu is removed that already contains menu items,
/// the event is only definitely dispatched for the removed menu; it might or be called
/// for each of the submenus.
///
void StackMenu::OnRemovedMenuItem(uint32 event_mask, const DM::MenuBarPtr &menu_bar, const DM::MenuPtr &par_menu, const DM::MenuItemPtr &menu_item)
{
	DEBUG2("StackMenu::OnRemovedMenuItem invoked");
}
///
/// 'OnAboutToOpenMenuBar' is called whenever the user touches the menu bar.
/// It allows the plugin to modify characteristics of the menu
/// items depending on the current state.  All items are implicitly
/// disabled.  The following code enables the first command if
/// there is an open image window, and it always enables the second
/// command.
/// 
void StackMenu::OnAboutToOpenMenuBar(uint32 event_mask, const DM::MenuBarPtr &menu_bar)
{
	DEBUG2("StackMenu::OnAboutToOpenMenuBar invoked");

	itemStackSize->SetName("Stack size: " + boost::lexical_cast<std::string>(stack_size));

	// This does nothing if entry is on third level?
	itemExpanding->SetIsChecked(isExpanding);
	itemFixed->SetIsChecked(isFixed);
	itemCircular->SetIsChecked(isCircular);

	std::string md_str = "";
	if (isFixed) md_str += "Fixed";
	if (isCircular) md_str += "Circular";
	if (isExpanding) md_str += "Expanding";

	DEBUG1("StackMenu::OnAboutToOpenMenuBar - Found current mode is " << md_str);

	if (isExpanding)
		ModeMenu->SetName("Mode: E");
	else if (isCircular)
		ModeMenu->SetName("Mode: C");
	else
		ModeMenu->SetName("Mode: F");


	bool is_building = false;
	for (int i = 0; i < ImageEntries.size(); ++i)
		is_building |= ImageEntries[i]->IsBuilding();
	
	std::string temp = "";
	if (!is_building) temp += "not ";
	DEBUG1("StackMenu::OnAboutToOpenMenuBar - Currently am " << temp << "building");

	if (is_building)
		fMenuItem3->SetName("Stop");
	else
	{
		fMenuItem3->SetName("Start");
		//fMenuItem2->SetIsEnabled(false);
	}

	bool is_paused = false;
	for (int i = 0; i < ImageEntries.size(); ++i)
		is_paused |= ImageEntries[i]->IsPaused();

	std::string p_str = "";
	if (is_paused) p_str = "Paused";
	else p_str = "Unpaused";

	DEBUG1("StackMenu::OnAboutToOpenMenuBar - Found builders are " << p_str);
	if (is_paused)
		fMenuItem2->SetName("UnPause");
	else
		fMenuItem2->SetName("Pause");


}
///
/// 'OnMenuBarRebuilt' is called whenever the application undergoes a major state change
/// that may require a different menu structure. The most common case is when the application
/// switches between novice, expert, and service modes. A client should do any restructuring
/// necessary for the menu items it controls.
///
void StackMenu::OnMenuBarRebuilt(uint32 event_mask, const DM::MenuBarPtr &menu_bar)
{
	DEBUG2("StackMenu::OnMenuBarRebuilt invoked");
}

void StackMenu::ImageCommandAction()
{
	DEBUG1("StackMenu::ImageCommandAction invoked");

	DM::Image image = DM::FindFrontImage();
	if (image.IsValid() && image.CountImageDisplays() > 0)
	{
		int i = 0;
		for (i = 0; i < ImageEntries.size(); ++i)
			if (*ImageEntries[i] == image)
				break;
		if (i == ImageEntries.size()) // if we didnt find the entry within the vector
		{
			ImageEntries.push_back(CreateTemplateBuilder(image));
			DMResult << "Added image " << image.GetName() << " to watch list" << std::endl;
		}
		else
			DMResult << "Image already in watch list" << std::endl;

		// set our menu states
		//fMenuItem2->SetIsEnabled(!ImageEntries.empty());
		fMenuItem3->SetIsEnabled(!ImageEntries.empty());

	} else
	{
		DEBUG1("StackMenu::ImageCommandAction No valid image");
	}
}

boost::shared_ptr<BuilderGeneric> StackMenu::CreateTemplateBuilder(DM::Image &image)
{
	DEBUG1("StackMenu::CreateTemplateBuilder invoked");
	boost::shared_ptr<BuilderGeneric> temp_ptr;
	DataType::DataTypes dtype = static_cast<DataType::DataTypes>(image.GetDataType());

	switch (dtype)
	{
	case DataType::BOOL:
		temp_ptr.reset(new Builder<bool>(image, ImagesMenu, shared_from_this())); break;
	case DataType::INT_1:
		temp_ptr.reset(new Builder<Gatan::int8>(image, ImagesMenu, shared_from_this())); break;
	case DataType::INT_2:
		temp_ptr.reset(new Builder<Gatan::int16>(image, ImagesMenu, shared_from_this())); break;
	case DataType::INT_4:
		temp_ptr.reset(new Builder<Gatan::int32>(image, ImagesMenu, shared_from_this())); break;
	case DataType::UINT_1:
		temp_ptr.reset(new Builder<Gatan::uint8>(image, ImagesMenu, shared_from_this())); break;
	case DataType::UINT_2:
		temp_ptr.reset(new Builder<Gatan::uint16>(image, ImagesMenu, shared_from_this())); break;
	case DataType::UINT_4:
		temp_ptr.reset(new Builder<Gatan::uint32>(image, ImagesMenu, shared_from_this())); break;
	case DataType::FLOAT:
		temp_ptr.reset(new Builder<float>(image, ImagesMenu, shared_from_this())); break;
	case DataType::DOUBLE:
		temp_ptr.reset(new Builder<double>(image, ImagesMenu, shared_from_this())); break;
	case DataType::COMPLEX_FLOAT:
		temp_ptr.reset(new Builder<std::complex<float>>(image, ImagesMenu, shared_from_this())); break;
	case DataType::COMPLEX_DOUBLE:
		temp_ptr.reset(new Builder<std::complex<double>>(image, ImagesMenu, shared_from_this())); break;
	case DataType::RGB:
		Gatan::PlugIn::gResultOut << "RGB images are not supported yet." << std::endl; break;
	default:
		Gatan::PlugIn::gResultOut << "I don't know this image type." << std::endl; break;
	}

	return temp_ptr;
}

void StackMenu::Command2Action()
{
	DEBUG1("StackMenu::Pause invoked");

	if (fMenuItem2->GetName().get_string() == "Pause") {
		for (int i = 0; i < ImageEntries.size(); ++i)
			ImageEntries[i]->Pause(true);
		fMenuItem2->SetName("UnPause");
	}
	else {
		for (int i = 0; i < ImageEntries.size(); ++i)
			ImageEntries[i]->Pause(false);
		fMenuItem2->SetName("Pause");
	}
}

void StackMenu::Command3Action()
{
	DEBUG1("StackMenu::Start invoked");
	//TODO: this doesnt want to actually stop the thread? just stop it running, so will need a different function
	//TODO: will have something similar to start it

	if (fMenuItem3->GetName().get_string() == "Start")
	{
		for (int i = 0; i < ImageEntries.size(); ++i)
		{
			if (isFixed)
				ImageEntries[i]->StartBuildingFixed(stack_size);
			else if (isCircular)
				ImageEntries[i]->StartBuildingCircular(stack_size);
			else if (isExpanding)
				ImageEntries[i]->StartBuildingExpanding(stack_size);
		}
		fMenuItem2->SetIsEnabled(true);
		fMenuItem3->SetName("Stop");
	}
	else
	{
		// just testing the stop ability for now
		for (int i = 0; i < ImageEntries.size(); ++i)
			ImageEntries[i]->StopBuild();
		fMenuItem2->SetIsEnabled(false);
		fMenuItem2->SetName("Pause");
		fMenuItem3->SetName("Start");
	}
}

void StackMenu::RemoveIndex(int index)
{
	DEBUG1("StackMenu::RemoveIndex invoked");
	ImageEntries.erase(ImageEntries.begin() + index);
}

void StackMenu::RemoveImage(const DM::Image &img)
{
	DEBUG1("StackMenu::RemoveImage invoked");
	for (int i = 0; i < ImageEntries.size(); ++i)
	{
		if (*ImageEntries[i] == img)
			RemoveIndex(i);
	}

	// set our menu states
	if (ImageEntries.empty())
	{
		fMenuItem2->SetIsEnabled(false);
		fMenuItem3->SetIsEnabled(false);
	}
}