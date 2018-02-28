#include "menuplugin.h"

PluginUtilityClasses::PluginUtilityClasses()
{
}
///
/// This is called when the plugin is loaded.  Whenever DM is
/// launched, it calls 'Start' for each installed plug-in.
/// When it is called, there is no guarantee that any given
/// plugin has already been loaded, so the code should not
/// rely on scripts installed from other plugins.  The primary
/// use is to install script functions.
///
void PluginUtilityClasses::Start()
{
}

///
/// This is called when the plugin is loaded, after the 'Start' method.
/// Whenever DM is launched, it calls the 'Run' method for
/// each installed plugin after the 'Start' method has been called
/// for all such plugins and all script packages have been installed.
/// Thus it is ok to use script functions provided by other plugins.
///
void PluginUtilityClasses::Run()
{
	fMenu = boost::shared_ptr<StackMenu>(new StackMenu());
	fMenu->InstallMenu();

	// ensure to load the tool bitmap from the correct resource module
	//GTN_AFX_MANAGE_STATE();
	//fTool = boost::shared_ptr<ToolExample>(new ToolExample());
}

///
/// This is called when the plugin is unloaded.  Whenever DM is
/// shut down, the 'Cleanup' method is called for all installed plugins
/// before script packages are uninstalled and before the 'End'
/// method is called for any plugin.  Thus, script functions provided
/// by other plugins are still available.  This method should release
/// resources allocated by 'Run'.
///
void PluginUtilityClasses::Cleanup()
{
	fMenu->UninstallMenu();

	fMenu = boost::shared_ptr<StackMenu>();
	//fTool = boost::shared_ptr<ToolExample>();
}

///
/// This is called when the plugin is unloaded.  Whenever DM is shut
/// down, the 'End' method is called for all installed plugins after
/// all script packages have been unloaded, and other installed plugins
/// may have already been completely unloaded, so the code should not
/// rely on scripts installed from other plugins.  This method should
/// release resources allocated by 'Start', and in particular should
/// uninstall all installed script functions.
///
void PluginUtilityClasses::End()
{
}
