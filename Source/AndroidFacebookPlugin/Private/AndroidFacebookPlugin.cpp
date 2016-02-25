// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "AndroidFacebookPluginPrivatePCH.h"
#include "AndroidFacebookSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FAndroidFacebookPluginModule"

void FAndroidFacebookPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Facebook (Android)",
			LOCTEXT("RuntimeSettingsName", "AndroidFacebook"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Facebook Android plugin"),
			GetMutableDefault<UAndroidFacebookSettings>()
			);
	}

}

void FAndroidFacebookPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAndroidFacebookPluginModule, AndroidFacebookPlugin)