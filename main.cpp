#include "stdafx.h"

#include "DMPlugInMain.h"

#include "menuplugin.h"

// using MFC objects, like bitmaps or other resources inside DM, which are provided by a plugin, requires
// that the plugin is a MFC extension dll
extern AFX_EXTENSION_MODULE gPlugInExtensionModule = { NULL, NULL };

PluginUtilityClasses gTemplatePlugIn;