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
	static void SendPOSTRequest(FString Endpoint, FString Path, TSharedPtr<FJsonObject> Content, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback);
	static void SendAuthenticatedPOSTRequest(FString Endpoint, FString Path, FString PIT, TSharedPtr<FJsonObject> Content, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback);
	static void SendAuthenticatedGETRequest(FString Endpoint, FString Path, FString PIT, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback);
	static void SendAuthenticatedPlayFabPOSTRequest(FString Path, FString SessionTicket, TSharedPtr<FJsonObject> Content, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback);
	static FString GetFieldFromJson(TSharedPtr<FJsonObject> Content, FString StringField);
	static TArray<FString> GetListFromJson(TSharedPtr<FJsonObject> Content, FString StringField);
	static FString GetFieldFromJson(FString ContentString, FString StringField);
	static TArray<FString> GetListFromJson(FString ContentString, FString StringField);

	UPROPERTY(Config)
	FString PlayFabTitleID;

	UPROPERTY(Config)
	FString PlayFabSecretKey;

	UPROPERTY(Config)
	FString GoogleCloudProjectID;
};
