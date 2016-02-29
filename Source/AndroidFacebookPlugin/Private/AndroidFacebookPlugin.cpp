// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "AndroidFacebookPluginPrivatePCH.h"
#include "AndroidFacebookSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FAndroidFacebookPluginModule"

/**
* Class responsible for creating instance(s) of the subsystem
*/
class FOnlineFactoryFacebook : public IOnlineFactory
{
public:

	FOnlineFactoryFacebook() {}
	virtual ~FOnlineFactoryFacebook() {}

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName)
	{
#if PLATFORM_ANDROID
		FOnlineSubsystemFacebookPtr OnlineSub = MakeShareable(new FOnlineSubsystemFacebook());

		if (OnlineSub->IsEnabled())
		{
			if (!OnlineSub->Init())
			{
				UE_LOG(LogOnline, Warning, TEXT("Facebook Android API failed to initialize!"));
				OnlineSub->Shutdown();
				OnlineSub = NULL;
			}
		}
		else
		{
			UE_LOG(LogOnline, Warning, TEXT("Facebook Android API disabled!"));
			OnlineSub->Shutdown();
			OnlineSub = NULL;
		}

		return OnlineSub;
#else
		return nullptr;
#endif
	}
};

void FAndroidFacebookPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogOnline, Log, TEXT("Facebook Android Startup!"));

	// register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Facebook (Android)",
			LOCTEXT("RuntimeSettingsName", "AndroidFacebook"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Facebook Android plugin"),
			GetMutableDefault<UAndroidFacebookSettings>()
			);
	}

#if PLATFORM_ANDROID
	FacebookFactory = new FOnlineFactoryFacebook();

	// Create and register our singleton factory with the main online subsystem for easy access
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.RegisterPlatformService(FACEBOOK_SUBSYSTEM, FacebookFactory);
#endif
}

void FAndroidFacebookPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UE_LOG(LogOnline, Log, TEXT("Facebook Android Shutdown!"));

#if PLATFORM_ANDROID
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.UnregisterPlatformService(FACEBOOK_SUBSYSTEM);

	delete FacebookFactory;
	FacebookFactory = NULL;
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAndroidFacebookPluginModule, AndroidFacebookPlugin)