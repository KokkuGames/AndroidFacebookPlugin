// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


// Module includes
#include "AndroidFacebookPluginPrivatePCH.h"
#include "OnlineIdentityFacebook.h"

#include "Android/AndroidJNI.h"

///////////////////////////////////////////////////////////////////////////////////////
// FUserOnlineAccountFacebook implementation


FUserOnlineAccountFacebook::FUserOnlineAccountFacebook(const FString& InUserId, const FString& InAuthTicket) 
		: AuthTicket(InAuthTicket)
		, UserId(new FUniqueNetIdString(InUserId))
{
	GConfig->GetString(TEXT("OnlineSubsystemFacebook.Login"), TEXT("AuthToken"), AuthTicket, GEngineIni);
}

TSharedRef<const FUniqueNetId> FUserOnlineAccountFacebook::GetUserId() const
{
	return UserId;
}

FString FUserOnlineAccountFacebook::GetRealName() const
{
	return UserName;
}

FString FUserOnlineAccountFacebook::GetDisplayName() const
{
	return UserName;
}

bool FUserOnlineAccountFacebook::GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	return false;
}

bool FUserOnlineAccountFacebook::SetUserAttribute(const FString & AttrName, const FString & AttrValue)
{
	return false;
}

FString FUserOnlineAccountFacebook::GetAccessToken() const
{
	return AuthTicket;
}

bool FUserOnlineAccountFacebook::GetAuthAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////
// FOnlineIdentityFacebook implementation

DECLARE_JNIMETHOD(AndroidThunkJava_Facebook_Login);
DECLARE_JNIMETHOD(AndroidThunkJava_Facebook_Logout);
DECLARE_JNIMETHOD(AndroidThunkJava_Facebook_GetProfileName);

FOnlineIdentityFacebook::FOnlineIdentityFacebook()
	: UserAccount( MakeShareable(new FUserOnlineAccountFacebook()) )
	, LoginStatus( ELoginStatus::NotLoggedIn )
{
	static bool bGotMethods = false;
	if (!bGotMethods)
	{
		bGotMethods = true;

      INIT_JNIMETHOD(AndroidThunkJava_Facebook_Login, "(J)V");
      INIT_JNIMETHOD(AndroidThunkJava_Facebook_Logout, "(J)V");
      INIT_JNIMETHOD(AndroidThunkJava_Facebook_GetProfileName, "()Ljava/lang/String;");
	}
}


TSharedPtr<FUserOnlineAccount> FOnlineIdentityFacebook::GetUserAccount(const FUniqueNetId& UserId) const
{
	return UserAccount;
}


TArray<TSharedPtr<FUserOnlineAccount> > FOnlineIdentityFacebook::GetAllUserAccounts() const
{
	TArray<TSharedPtr<FUserOnlineAccount> > Result;

	Result.Add( UserAccount );

	return Result;
}


TSharedPtr<const FUniqueNetId> FOnlineIdentityFacebook::GetUniquePlayerId(int32 LocalUserNum) const
{
	
	return UserAccount->GetUserId();
}

typedef void (*NativeFbLoginCompletedFunc) (bool, const FString&, const FString&, const FString&);

extern "C" void Java_com_epicgames_ue4_GameActivity_nativeFbLoginCompleted(JNIEnv* jenv, jobject thiz, jboolean success, jstring userId, jstring accessToken, jstring realName, jlong handle)
{

	FOnlineIdentityFacebook* Identity = reinterpret_cast<FOnlineIdentityFacebook*>(handle);
	Identity->SetLoginResults(success, ToFString(userId), ToFString(accessToken), ToFString(realName));
}


bool FOnlineIdentityFacebook::Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
	bool bTriggeredLogin = true;

	

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AndroidThunkJava_Facebook_Login, reinterpret_cast<jlong>(this));

	/*
	dispatch_async(dispatch_get_main_queue(),^ 
		{
			// Retrieve the user info when logged in
			void(^CompletionBlock)(bool, const FUniqueNetId&) = ^ (bool bWasSucessful, const FUniqueNetId& UserId)
			{
				[[[FBSDKGraphRequest alloc] initWithGraphPath:@"me" parameters:nil]
				startWithCompletionHandler:^ (FBSDKGraphRequestConnection *connection, id result, NSError *error) {
					if (!error) {
						const FString RealName(result[@"name"]);
						UserAccount->UserName = RealName;
						UE_LOG(LogOnline, Display, TEXT("Got Facebook name: %s"), *RealName);
					}

					[FIOSAsyncTask CreateTaskWithBlock : ^ bool(void)
					{						
						TriggerOnLoginCompleteDelegates(LocalUserNum, bWasSucessful, UserId, TEXT(""));
						return true;
					}];
				}];
			};


			FBSDKAccessToken *accessToken = [FBSDKAccessToken currentAccessToken];
			if (accessToken == nil)
			{
				FBSDKLoginManager* loginManager = [[FBSDKLoginManager alloc] init];
				[loginManager logInWithReadPermissions : @[@"public_profile"]
					fromViewController: nil
					handler: ^(FBSDKLoginManagerLoginResult* result, NSError* error)
					{
						UE_LOG(LogOnline, Display, TEXT("[FBSDKLoginManager logInWithReadPermissions]"));
						bool bSuccessfulLogin = false;
				 
						if(error)
						{
							UE_LOG(LogOnline, Display, TEXT("[FBSDKLoginManager logInWithReadPermissions = error[%d]]"), [error code]);
						}
						else if(result.isCancelled)
						{
							UE_LOG(LogOnline, Display, TEXT("[FBSDKLoginManager logInWithReadPermissions = cancelled"));
						}
						else
						{
							UE_LOG(LogOnline, Display, TEXT("[FBSDKLoginManager logInWithReadPermissions = true]"));
				 
							FString Token([result token].tokenString);
							UserAccount->AuthTicket = Token;
							GConfig->SetString(TEXT("OnlineSubsystemFacebook.Login"), TEXT("AuthToken"), *Token, GEngineIni);
				 
							const FString UserId([result token].userID);
							UserAccount->UserId = MakeShareable(new FUniqueNetIdString(UserId));

							const FString RealName([FBSDKProfile currentProfile].name);
							UserAccount->UserName = RealName;
							UE_LOG(LogOnline, Display, TEXT("Got Facebook name: %s"), *RealName);
				 
							bSuccessfulLogin = true;
						}
				 
						LoginStatus = bSuccessfulLogin ? ELoginStatus::LoggedIn : ELoginStatus::NotLoggedIn;
												
						FUniqueNetIdString TempId;
						CompletionBlock(bSuccessfulLogin, (bSuccessfulLogin ? UserAccount->UserId.Get() : TempId));
				 
						UE_LOG(LogOnline, Display, TEXT("Facebook login was successful? - %d"), bSuccessfulLogin);
					}
				];
			}
			else
			{
				LoginStatus = ELoginStatus::LoggedIn;
				const FString UserId([accessToken userID]);
				UserAccount->UserId = MakeShareable(new FUniqueNetIdString(UserId));

				FString Token([accessToken tokenString]);
				UserAccount->AuthTicket = Token;
				
				CompletionBlock(true, UserAccount->UserId.Get());

				UE_LOG(LogOnline, Display, TEXT("Facebook login was successful? - Already had token!"));
			}
		}
	);
	*/
	
	return bTriggeredLogin;	
}


TSharedPtr<const FUniqueNetId> FOnlineIdentityFacebook::CreateUniquePlayerId(uint8* Bytes, int32 Size)
{
	if (Bytes != NULL && Size > 0)
	{
		FString StrId(Size, (TCHAR*)Bytes);
		return MakeShareable(new FUniqueNetIdString(StrId));
	}
	return NULL;
}


TSharedPtr<const FUniqueNetId> FOnlineIdentityFacebook::CreateUniquePlayerId(const FString& Str)
{
	return MakeShareable(new FUniqueNetIdString(Str));
}


bool FOnlineIdentityFacebook::Logout(int32 LocalUserNum)
{
	//@todo samz - login status change delegate
	/*
	dispatch_async(dispatch_get_main_queue(),^ 
		{
			if ([FBSDKAccessToken currentAccessToken])
			{
				FBSDKLoginManager *loginManager = [[FBSDKLoginManager alloc] init];
				[loginManager logOut];
				
				LoginStatus = ELoginStatus::NotLoggedIn;
			}
		}
	);
	*/

	return true;
}


bool FOnlineIdentityFacebook::AutoLogin(int32 LocalUserNum)
{
	return false;
}


ELoginStatus::Type FOnlineIdentityFacebook::GetLoginStatus(int32 LocalUserNum) const
{
	return LoginStatus;
}

ELoginStatus::Type FOnlineIdentityFacebook::GetLoginStatus(const FUniqueNetId& UserId) const 
{
	return LoginStatus;
}


FString FOnlineIdentityFacebook::GetPlayerNickname(int32 LocalUserNum) const
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
	{	
		jstring jsString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, AndroidThunkJava_Facebook_GetProfileName);
		check(jsString);

		const char * nativeName = Env->GetStringUTFChars(jsString, 0);
		FString ResultName = FString(nativeName);
		Env->ReleaseStringUTFChars(jsString, nativeName);
		
		Env->DeleteLocalRef(jsString);

		return ResultName;
	}
	else {
		check(0);
	}

	return TEXT("");
}

FString FOnlineIdentityFacebook::GetPlayerNickname(const FUniqueNetId& UserId) const 
{	
	return GetPlayerNickname(0);
}


FString FOnlineIdentityFacebook::GetAuthToken(int32 LocalUserNum) const
{
	return UserAccount->GetAccessToken();
}

void FOnlineIdentityFacebook::GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
	Delegate.ExecuteIfBound(UserId, Privilege, (uint32)EPrivilegeResults::NoFailures);
}

FPlatformUserId FOnlineIdentityFacebook::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId)
{
	for (int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		auto CurrentUniqueId = GetUniquePlayerId(i);
		if (CurrentUniqueId.IsValid() && (*CurrentUniqueId == UniqueNetId))
		{
			return i;
		}
	}

	return PLATFORMUSERID_NONE;
}

void FOnlineIdentityFacebook::SetLoginResults(bool bSuccess, const FString & Id, const FString & Ticket, const FString & UserName)
{
   LoginStatus = bSuccess ? ELoginStatus::LoggedIn : ELoginStatus::NotLoggedIn;

	if (!bSuccess)
	{
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]()
			{
				FUniqueNetIdString TempId;
				TriggerOnLoginCompleteDelegates(0, false, TempId, TEXT(""));
			}),
			TStatId(),
			nullptr,
			ENamedThreads::GameThread
			);
	}
	else
	{
      UE_LOG(LogOnline, Display, TEXT("Facebook login was successful!- %s %s"), *UserName, *Ticket);

		UserAccount->UserId = MakeShareable(new FUniqueNetIdString(Id));
		UserAccount->AuthTicket = Ticket;
		UserAccount->UserName = UserName;

		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]()
			{
				TriggerOnLoginCompleteDelegates(0, true, UserAccount->UserId.Get(), TEXT(""));
			}),
			TStatId(),
			nullptr,
			ENamedThreads::GameThread
			);
	}
}


FString FOnlineIdentityFacebook::GetAuthType() const
{
	return TEXT("");
}
