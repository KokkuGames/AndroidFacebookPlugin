#pragma once

#include "OnlineAvatarInterface.h"
#include "OnlineSubsystemFacebookPackage.h"


class FUserAvatarFacebook :
	public FUserAvatar
{
	
public:
	FUserAvatarFacebook(const FString& InUserId = TEXT("")) :
		UserId(new FUniqueNetIdString(InUserId))
	{

	}

	virtual ~FUserAvatarFacebook() 
	{

	}	

	virtual TSharedRef<FUniqueNetId> GetUserId() const override;

	virtual FString GetThumbnailURL() const override;

	virtual FString GetPictureURL() const override;

	/** User Id represented as a FUniqueNetId */
	TSharedRef<FUniqueNetId> UserId;
};
typedef TSharedPtr<FUserAvatarFacebook> FUserAvatarFacebookPtr;


class FOnlineAvatarFacebook : public IOnlineAvatar
{
public:
	virtual ~FOnlineAvatarFacebook()
	{

	}
public:
		
	virtual bool RetrieveAvatar(const FUniqueNetId & PlayerId, const FOnAvatarRetrievedDelegate& Delegate) override;

private:
	TMap< FString, FUserAvatarFacebookPtr > UserAvatarMap;
};

