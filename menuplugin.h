#pragma once

#include "stdafx.h"

#include <boost/shared_ptr.hpp>

#include "stackmenu.h"

class PluginUtilityClasses : public Gatan::PlugIn::PlugInMain
{
private:
	boost::shared_ptr<StackMenu> fMenu;
public:
	PluginUtilityClasses();
	virtual void Start();
	virtual void Run();
	virtual void Cleanup();
	virtual void End();
};