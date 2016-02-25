// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AndroidFacebookSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class UAndroidFacebookSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UAndroidFacebookSettings(const FObjectInitializer& ObjectInitializer);
	
	// General	
	UPROPERTY(Config, EditAnywhere, Category=General, meta=(DisplayName="Facebook App Id"))
	FString FacebookAppId;	
};
