// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "AndroidFacebookPluginPrivatePCH.h"

#include "OnlineSharingFacebook.h"
#include "OnlineUserFacebook.h"
#include "CallbackDevice.h"

FOnlineSubsystemFacebook::FOnlineSubsystemFacebook() 
	: FacebookIdentity(nullptr)
	, FacebookFriends(nullptr)
	, FacebookSharing(nullptr)
	, FacebookUser(nullptr)
{

}

FOnlineSubsystemFacebook::~FOnlineSubsystemFacebook()
{
	FacebookIdentity = nullptr;
	FacebookFriends = nullptr;
	FacebookSharing = nullptr; 
	FacebookUser = nullptr;
}

IOnlineSessionPtr FOnlineSubsystemFacebook::GetSessionInterface() const
{
	return nullptr;
}

IOnlineIdentityPtr FOnlineSubsystemFacebook::GetIdentityInterface() const
{
	return FacebookIdentity;
}

IOnlineFriendsPtr FOnlineSubsystemFacebook::GetFriendsInterface() const
{
	return FacebookFriends;
}

IOnlineGroupsPtr FOnlineSubsystemFacebook::GetGroupsInterface() const
{
	return nullptr;
}

IOnlinePartyPtr FOnlineSubsystemFacebook::GetPartyInterface() const
{
	return nullptr;
}

IOnlineSharedCloudPtr FOnlineSubsystemFacebook::GetSharedCloudInterface() const
{
	return nullptr;
}

IOnlineUserCloudPtr FOnlineSubsystemFacebook::GetUserCloudInterface() const
{
	return nullptr;
}

IOnlineLeaderboardsPtr FOnlineSubsystemFacebook::GetLeaderboardsInterface() const
{
	return nullptr;
}

IOnlineVoicePtr FOnlineSubsystemFacebook::GetVoiceInterface() const
{
	return nullptr;
}

IOnlineExternalUIPtr FOnlineSubsystemFacebook::GetExternalUIInterface() const	
{
	return nullptr;
}

IOnlineTimePtr FOnlineSubsystemFacebook::GetTimeInterface() const
{
	return nullptr;
}

IOnlineTitleFilePtr FOnlineSubsystemFacebook::GetTitleFileInterface() const
{
	return nullptr;
}

IOnlineEntitlementsPtr FOnlineSubsystemFacebook::GetEntitlementsInterface() const
{
	return nullptr;
}

IOnlineStorePtr FOnlineSubsystemFacebook::GetStoreInterface() const
{
	return nullptr;
}

IOnlineEventsPtr FOnlineSubsystemFacebook::GetEventsInterface() const
{
	return nullptr;
}

IOnlineAchievementsPtr FOnlineSubsystemFacebook::GetAchievementsInterface() const
{
	return nullptr;
}

IOnlineSharingPtr FOnlineSubsystemFacebook::GetSharingInterface() const
{
	return FacebookSharing;
}

IOnlineUserPtr FOnlineSubsystemFacebook::GetUserInterface() const
{
	return FacebookUser;
}

IOnlineMessagePtr FOnlineSubsystemFacebook::GetMessageInterface() const
{
	return nullptr;
}

IOnlinePresencePtr FOnlineSubsystemFacebook::GetPresenceInterface() const
{
	return nullptr;
}

IOnlineChatPtr FOnlineSubsystemFacebook::GetChatInterface() const
{
	return nullptr;
}

IOnlineTurnBasedPtr FOnlineSubsystemFacebook::GetTurnBasedInterface() const
{
	return nullptr;
}

//static void ListenFacebookOpenURL(UIApplication* application, NSURL* url, NSString* sourceApplication, id annotation)
//{
//	
//	[[FBSDKApplicationDelegate sharedInstance] application:application
//												   openURL:url
//									 sourceApplication:sourceApplication
//												annotation:annotation];												
//}

IOnlineAvatarPtr FOnlineSubsystemFacebook::GetAvatarInterface() const
{
	return FacebookAvatar;
}

bool FOnlineSubsystemFacebook::Init() 
{
	bool bSuccessfullyStartedUp = true;
	
	/*
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[FBSDKAppEvents activateApp];
	});
	*/

	//FIOSCoreDelegates::OnOpenURL.AddStatic(&ListenFacebookOpenURL);

	FCoreDelegates::ApplicationHasReactivatedDelegate.AddStatic([]()
	{
		/*
		dispatch_async(dispatch_get_main_queue(), ^
		{
			[FBSDKAppEvents activateApp];
		});
		*/
	});
	
	/*
	dispatch_async(dispatch_get_main_queue(), ^
	{
		[[FBSDKApplicationDelegate sharedInstance] application:[UIApplication sharedApplication]
				didFinishLaunchingWithOptions : [IOSAppDelegate GetDelegate].launchOptions];
	});
	*/
	
	FacebookIdentity = MakeShareable(new FOnlineIdentityFacebook());
	FacebookSharing = MakeShareable(new FOnlineSharingFacebook(this));
	FacebookFriends = MakeShareable(new FOnlineFriendsFacebook(this));
	FacebookUser = MakeShareable(new FOnlineUserFacebook(this));
	FacebookAvatar = MakeShareable(new FOnlineAvatarFacebook());
	
	return bSuccessfullyStartedUp;
}

bool FOnlineSubsystemFacebook::Shutdown() 
{
	bool bSuccessfullyShutdown = true;

	bSuccessfullyShutdown = FOnlineSubsystemImpl::Shutdown();
	return bSuccessfullyShutdown;
}

FString FOnlineSubsystemFacebook::GetAppId() const 
{
	return TEXT( "" );
}

bool FOnlineSubsystemFacebook::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)  
{
	return false;
}

bool FOnlineSubsystemFacebook::Tick(float DeltaTime)
{
	if (!FOnlineSubsystemImpl::Tick(DeltaTime))
	{
		return false;
	}

	return true;
}

bool FOnlineSubsystemFacebook::IsEnabled()
{
	// Check the ini for disabling Facebook
	bool bEnableFacebookSupport = false;
	// Whilst we deprecate the old warning, let's still check if it's available	
	GConfig->GetBool(TEXT("/Script/AndroidFacebookPlugin.AndroidFacebookSettings"), TEXT("bEnableFacebookSupport"), bEnableFacebookSupport, GEngineIni);
	
#if !UE_BUILD_SHIPPING
	// Check the commandline for disabling Facebook
	bEnableFacebookSupport = bEnableFacebookSupport && !FParse::Param(FCommandLine::Get(), TEXT("NOFACEBOOK"));
#endif

	return bEnableFacebookSupport;
}
