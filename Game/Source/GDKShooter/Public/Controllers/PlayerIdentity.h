// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabError.h"
#include "CoreOnline.h"

#include "UObject/NoExportTypes.h"
#include "PlayerIdentity.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOnlineServices, Log, All);

class ILoginFlowManager;
class IOnlineIdentity;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAddExternalUI, const UWidget*, ExternalLoginWidget);
DECLARE_DYNAMIC_DELEGATE(FOnRemoveExternalUI);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFullLoginComplete, const bool, Success, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFullLoginCompleteMulticast, const bool, Success, const FString&, Message);

UCLASS(BlueprintType)
class GDKSHOOTER_API UPlayerIdentity : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void Login(UObject* WorldContextObject, const FOnAddExternalUI& OnAddExternalUI, const FOnRemoveExternalUI& OnRemoveExternalUI, const FOnFullLoginComplete& OnComplete);

	void BindToLoginComplete(const FOnFullLoginComplete& OnComplete);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PlayerIdentity")
	FString GetPlayFabId();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PlayerIdentity")
	FString GetPlayerDisplayName();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PlayerIdentity")
	FString GetPlayerIdentityToken();

private:
	// First log into Google.
	void StartGoogleLogin(UObject* WorldContextObject, const FOnAddExternalUI& OnAddExternalUI, const FOnRemoveExternalUI& OnRemoveExternalUI);
	void OnGoogleLoginComplete(int32 LocalPlayerNum, bool Success, const FUniqueNetId& ID, const FString& Message);

	// Then use access token from Google to log into PlayFab.
	void StartPlayFabLogin();
	void OnPlayFabLoginSuccess(const PlayFab::ClientModels::FLoginResult& LoginResult);

	// Get player identity token from online services
	void RequestPlayerIdentityToken(const FString& SessionTicket);

	// Make sure their name is up to date (probably can do this more efficiently
	void UpdatePlayFabDisplayName();
	void OnDisplayNameUpdatedSuccess(const PlayFab::ClientModels::FUpdateUserTitleDisplayNameResult& LoginResult);

	// PlayFab API failure
	void OnPlayFabFailure(const PlayFab::FPlayFabCppError& Error);

	// Finally finish the flow.
	void OnLoginFlowComplete(bool Success, const FString& Message);

	FOnFullLoginCompleteMulticast OnCompleteDelegate;
	FDelegateHandle LoginCompleteHandle;

	TSharedPtr<ILoginFlowManager> LoginFlowManager;

	TWeakPtr<IOnlineIdentity, ESPMode::ThreadSafe> OnlineIdentityGoogle;

	FString GoogleID;
	FString GoogleAccessToken;
	FString PlayFabID;
	FString DisplayName;
	FString PlayerIdentityToken;

	bool bCurrentlyLoggingIn = false;
};
