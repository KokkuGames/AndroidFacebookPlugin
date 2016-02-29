#include "AndroidFacebookPluginPrivatePCH.h"
#include "OnlineAvatarFacebook.h"


TSharedRef<FUniqueNetId> FUserAvatarFacebook::GetUserId() const
{
	return UserId;
}

FString FUserAvatarFacebook::GetThumbnailURL() const
{
	return FString::Printf(TEXT("https://graph.facebook.com/%s/picture"), *(UserId->ToString()));
}

FString FUserAvatarFacebook::GetPictureURL() const
{
	return FString::Printf(TEXT("https://graph.facebook.com/%s/picture?type=large"), *(UserId->ToString()));
}


/* FOnlineAvatarFacebook */
bool FOnlineAvatarFacebook::RetrieveAvatar(const FUniqueNetId & PlayerId, const FOnAvatarRetrievedDelegate& Delegate)
{
	FString PlayerIdString = PlayerId.ToString();

	FUserAvatarFacebookPtr CachedAvatar(NULL);
	FUserAvatarFacebookPtr* CachedAvatarPtr = UserAvatarMap.Find(PlayerIdString);
	if (CachedAvatarPtr != NULL)
	{
		CachedAvatar = *CachedAvatarPtr;
	}
	else
	{
		CachedAvatar = MakeShareable(new FUserAvatarFacebook(PlayerIdString));
		UserAvatarMap.Add(PlayerIdString, CachedAvatar);
	}

	Delegate.ExecuteIfBound(PlayerId, CachedAvatar);

	return true;
}
