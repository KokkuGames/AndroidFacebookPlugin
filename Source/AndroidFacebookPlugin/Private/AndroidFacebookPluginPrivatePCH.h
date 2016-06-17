// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "CoreUObject.h"
#include "Engine.h"

#include "Core.h"
#include "ModuleManager.h"

#include "Json.h"
#include "OnlineSubsystem.h"

#include "AndroidFacebookPlugin.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "AndroidApplication.h"

#include "OnlineSubsystemFacebook.h"
#include "OnlineIdentityFacebook.h"
#include "OnlineFriendsFacebook.h"
#include "OnlineAvatarFacebook.h"
#endif

// You should place include statements to your module's private header files here.  You only need to
// add includes for headers that are used in most of your module's source files though.

#include "AndroidFacebookPluginClasses.h"


/** pre-pended to all Facebook logging */
#undef ONLINE_LOG_PREFIX
#define ONLINE_LOG_PREFIX TEXT("Facebook: ")

#define DECLARE_JNIMETHOD(name) \
   jmethodID	name = NULL;

#define INIT_JNIMETHOD(name, sig) \
   name = FJavaWrapper::FindMethod(FAndroidApplication::GetJavaEnv(true), FJavaWrapper::GameActivityClassID, #name, sig, false); \
   check(name);