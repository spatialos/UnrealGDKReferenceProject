// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineServicesLibrary.generated.h"

UCLASS(config=Engine)
class GDKSHOOTER_API UOnlineServicesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SetupPlayFabSettings();
	static void SendHTTPRequest(FString Endpoint, FString Path, TSharedPtr<FJsonObject> Content, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback);

	UPROPERTY(Config)
	FString PlayFabTitleID;

	UPROPERTY(Config)
	FString PlayFabSecretKey;

	UPROPERTY(Config)
	FString GoogleCloudProjectID;
};
