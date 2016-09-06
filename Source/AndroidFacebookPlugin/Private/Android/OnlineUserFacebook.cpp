// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


// Module includes
#include "AndroidFacebookPluginPrivatePCH.h"
#include "OnlineUserFacebook.h"


// FOnlineUserInfoFacebook

TSharedRef<const FUniqueNetId> FOnlineUserInfoFacebook::GetUserId() const
{
	return UserId;
}

FString FOnlineUserInfoFacebook::GetRealName() const
{
	FString Result;
	GetAccountData(TEXT("name"), Result);
	return Result;
}

FString FOnlineUserInfoFacebook::GetDisplayName(const FString& Platform) const
{
	FString Result;
	GetAccountData(TEXT("username"), Result);
	return Result;
}

bool FOnlineUserInfoFacebook::GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const
{
	return GetAccountData(AttrName, OutAttrValue);
}

// FOnlineUserFacebook

FOnlineUserFacebook::FOnlineUserFacebook(class FOnlineSubsystemFacebook* InSubsystem)
{
	// Get our handle to the identity interface
	IdentityInterface = (FOnlineIdentityFacebook*)InSubsystem->GetIdentityInterface().Get();
}


FOnlineUserFacebook::~FOnlineUserFacebook()
{

}

bool FOnlineUserFacebook::QueryUserInfo(int32 LocalUserNum, const TArray<TSharedRef<const FUniqueNetId> >& UserIds)
{
	bool bTriggeredRequest = false;
	
	if(UserIds.Num() > 0 && IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
	{
		bTriggeredRequest = true;
		CachedUsers.Empty();

		/*
		dispatch_async(dispatch_get_main_queue(),^ 
			{
				// We only ever have a single user in the facebook OSS.
				FString ErrorStr;
				bool bGatheredUserInfo = false;
				if([FBSDKAccessToken currentAccessToken])
				{
					CachedUsers.Empty();
					
					bool bValidUserRequested = false;
					const FString UserName([FBSDKProfile currentProfile].userID);
					for(auto& NextUser : UserIds)
					{
						if(NextUser->ToString() == UserName)
						{
							bValidUserRequested = true;
							break;
						}
					}
					if(bValidUserRequested)
					{
						const FString RealName([FBSDKProfile currentProfile].name);
						
						TSharedRef<FOnlineUserInfoFacebook> FBUserInfo = MakeShareable(new FOnlineUserInfoFacebook(UserIds[0]->ToString()));
						FBUserInfo->AccountData.Add(TEXT("name"), RealName);
						FBUserInfo->AccountData.Add(TEXT("username"), UserName);
						
						UE_LOG(LogOnline, Display, TEXT("User Found: u:%s r:%s"), *UserName, *RealName);
						CachedUsers.Add( FBUserInfo );
						bGatheredUserInfo = true;
					}
					else
					{
						ErrorStr = TEXT("No user ids matched those of the single facebook user.");
						UE_LOG(LogOnline, Display, TEXT("Failed to gather user information: %s"), *ErrorStr);
					}

				}
				else
				{
					ErrorStr = TEXT("No valid login.");
					UE_LOG(LogOnline, Display, TEXT("Failed to gather user information: %s"), *ErrorStr);
				}

				[FIOSAsyncTask CreateTaskWithBlock : ^ bool(void)
				{
					TriggerOnQueryUserInfoCompleteDelegates(LocalUserNum, bGatheredUserInfo, UserIds, *ErrorStr);
					return true;
				}];
			}
		);*/
	}
	else
	{
		TriggerOnQueryUserInfoCompleteDelegates(LocalUserNum, false, UserIds, UserIds.Num() > 0 ? TEXT("Not logged in.") : TEXT("No users requested."));
	}
	
	return bTriggeredRequest;
}

bool FOnlineUserFacebook::GetAllUserInfo(int32 LocalUserNum, TArray< TSharedRef<class FOnlineUser> >& OutUsers)
{
	UE_LOG(LogOnline, Verbose, TEXT("FOnlineUserFacebook::GetAllUserInfo()"));

	for (int32 Idx=0; Idx < CachedUsers.Num(); Idx++)
	{
		OutUsers.Add(CachedUsers[Idx]);
	}
	return true;
}

TSharedPtr<FOnlineUser> FOnlineUserFacebook::GetUserInfo(int32 LocalUserNum, const class FUniqueNetId& UserId)
{
	TSharedPtr<FOnlineUser> Result;

	UE_LOG(LogOnline, Verbose, TEXT("FOnlineUserFacebook::GetUserInfo()"));

	for (int32 Idx=0; Idx < CachedUsers.Num(); Idx++)
	{
		if (*(CachedUsers[Idx]->GetUserId()) == UserId)
		{
			Result = CachedUsers[Idx];
			break;
		}
	}
	return Result;
}

bool FOnlineUserFacebook::QueryUserIdMapping(const FUniqueNetId& UserId, const FString& DisplayNameOrEmail, const FOnQueryUserMappingComplete& Delegate)
{
	Delegate.ExecuteIfBound(false, UserId, DisplayNameOrEmail, FUniqueNetIdString(), TEXT("not implemented"));
	return false;
}

bool FOnlineUserFacebook::QueryExternalIdMappings(const FUniqueNetId& LocalUserId, const FExternalIdQueryOptions& QueryOptions, const TArray<FString>& ExternalIds, const FOnQueryExternalIdMappingsComplete& Delegate)
{
	Delegate.ExecuteIfBound(false, LocalUserId, QueryOptions, ExternalIds, TEXT("not implemented"));
	return false;
}

void FOnlineUserFacebook::GetExternalIdMappings(const FExternalIdQueryOptions& QueryOptions, const TArray<FString>& ExternalIds, TArray<TSharedPtr<const FUniqueNetId>>& OutIds)
{
	// Not implemented for PS4 - return an array full of empty values
	OutIds.SetNum(ExternalIds.Num());
}

TSharedPtr<const FUniqueNetId> FOnlineUserFacebook::GetExternalIdMapping(const FExternalIdQueryOptions& QueryOptions, const FString& ExternalId)
{
	return TSharedPtr<FUniqueNetId>();
}
