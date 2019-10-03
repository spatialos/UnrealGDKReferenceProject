// Copyright (c) Improbable Worlds Ltd, All Rights Reserved


#include "PlayerIdentity.h"

#include "PlayFab.h"
#include "Core/PlayFabClientAPI.h"
#include "OnlineServicesLibrary.h"

#if PLATFORM_WINDOWS
#include "Components/NativeWidgetHost.h"
#include "ILoginFlowModule.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#endif

DEFINE_LOG_CATEGORY(LogOnlineServices);

using namespace PlayFab;

const FString CACHED_AUTH_FILE(FPaths::Combine(FPlatformProcess::UserSettingsDir(), TEXT("ReferenceProject"), TEXT("CachedAuth.ini")));
const FString CACHED_AUTH_GOOGLE_SECTION(TEXT("/Google"));

const FString CACHED_AUTH_ID(TEXT("Id"));
const FString CACHED_AUTH_TOKEN(TEXT("RefreshToken"));

void UPlayerIdentity::Login(UObject* WorldContextObject, const FOnAddExternalUI& OnAddExternalUI, const FOnRemoveExternalUI& OnRemoveExternalUI, const FOnFullLoginComplete& OnComplete)
{
	if (bCurrentlyLoggingIn == true)
	{
		return;
	}

	bCurrentlyLoggingIn = true;

	OnCompleteDelegate.Add(OnComplete);
	StartGoogleLogin(WorldContextObject, OnAddExternalUI, OnRemoveExternalUI);
}

void UPlayerIdentity::BindToLoginComplete(const FOnFullLoginComplete& OnComplete)
{
	OnCompleteDelegate.Add(OnComplete);
}

void UPlayerIdentity::StartGoogleLogin(UObject* WorldContextObject, const FOnAddExternalUI& OnAddExternalUI, const FOnRemoveExternalUI& OnRemoveExternalUI)
{
#if PLATFORM_WINDOWS
	checkf(ILoginFlowModule::IsAvailable(), TEXT("LoginFlow module must be available."));
	LoginFlowManager = ILoginFlowModule::Get().CreateLoginFlowManager();

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	check(World);

	FName GoogleIdentifier = Online::GetUtils()->GetOnlineIdentifier(World, GOOGLE_SUBSYSTEM);

	ILoginFlowManager::FOnDisplayPopup DisplayPopup = ILoginFlowManager::FOnDisplayPopup::CreateLambda(
		[OnAddExternalUI, OnRemoveExternalUI](const TSharedRef<SWidget>& LoginWidget) {
		UNativeWidgetHost* WidgetHost = NewObject<UNativeWidgetHost>();
		WidgetHost->SetContent(LoginWidget);

		OnAddExternalUI.ExecuteIfBound(WidgetHost);

		return ILoginFlowManager::FOnPopupDismissed::CreateLambda([OnRemoveExternalUI]() {
			OnRemoveExternalUI.ExecuteIfBound();
		});
	});

	verifyf(LoginFlowManager->AddLoginFlow(GoogleIdentifier, DisplayPopup, DisplayPopup), TEXT("Couldn't add external UI login flow for Google identity."));

	IOnlineSubsystem* oss = IOnlineSubsystem::Get(GoogleIdentifier);
	checkf(oss, TEXT("Couldn't get OnlineSubsystem for %s"), *GoogleIdentifier.ToString());

	IOnlineIdentityPtr identity = oss->GetIdentityInterface();
	checkf(identity.IsValid(), TEXT("Couldn't get IdentityInterface for OSS %s"), *GoogleIdentifier.ToString());

	OnlineIdentityGoogle = identity;
	LoginCompleteHandle = identity->OnLoginCompleteDelegates->AddUObject(this, &UPlayerIdentity::OnGoogleLoginComplete);

	FOnlineAccountCredentials credentials;

	FString googleId;
	FString googleToken;

	if (GConfig->GetString(*CACHED_AUTH_GOOGLE_SECTION, *CACHED_AUTH_ID, googleId, CACHED_AUTH_FILE) &&
		GConfig->GetString(*CACHED_AUTH_GOOGLE_SECTION, *CACHED_AUTH_TOKEN, googleToken, CACHED_AUTH_FILE))
	{
		credentials.Id = googleId;
		credentials.Token = googleToken;
		credentials.Type = TEXT("google");

		UE_LOG(LogOnlineServices, Log, TEXT("Trying to login using cached credentials (Google ID %s)."), *googleId);
	}

	if (!identity->Login(0, credentials))
	{
		UE_LOG(LogOnlineServices, Error, TEXT("Failed to start Google login."));
	}
#endif
}

void UPlayerIdentity::OnGoogleLoginComplete(int32 LocalPlayerNum, bool Success, const FUniqueNetId& ID, const FString& Message)
{
#if PLATFORM_WINDOWS
	if (LocalPlayerNum != 0)
	{
		UE_LOG(LogOnlineServices, Warning, TEXT("OnLoginComplete called with non-zero local player number."));
		return;
	}

	TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> identity = OnlineIdentityGoogle.Pin();
	check(identity.IsValid());

	identity->OnLoginCompleteDelegates->Remove(LoginCompleteHandle);
	LoginFlowManager->Reset();	// remove the registered login flow, manager should be cleaned up when it goes out of scope

	FString AccessToken;
	FString Name;

	if (Success)
	{
		// On success, we retrieve the access token which we need to send to the backend service to login to PlayFab, and then
		// retrieve and store the refresh token in a local config file for the next time we try to log in as this is much longer
		// lived than the access token.
		TSharedPtr<FUserOnlineAccount> Account = identity->GetUserAccount(ID);
		AccessToken = Account->GetAccessToken();
		Name = Account->GetDisplayName();

		FString RefreshToken;
		Account->GetAuthAttribute(AUTH_ATTR_REFRESH_TOKEN, RefreshToken);

		UE_LOG(LogOnlineServices, Log, TEXT("Logged in as Google ID %s (%s) with token %s, caching refresh token."), *ID.ToDebugString(),
			*Account->GetDisplayName(), *AccessToken);

		GConfig->SetString(*CACHED_AUTH_GOOGLE_SECTION, *CACHED_AUTH_ID, *ID.ToString(), CACHED_AUTH_FILE);
		GConfig->SetString(*CACHED_AUTH_GOOGLE_SECTION, *CACHED_AUTH_TOKEN, *RefreshToken, CACHED_AUTH_FILE);
		GConfig->Flush(true, CACHED_AUTH_FILE);

		GoogleID = ID.ToString();
		GoogleAccessToken = AccessToken;
		DisplayName = Name;

		StartPlayFabLogin();
	}
	else
	{
		if (Message != LOGIN_CANCELLED) // avoid clearing cache if the popup was closed
		{
			UE_LOG(LogOnlineServices, Warning, TEXT("Couldn't log in (%s), resetting cached credentials."), *Message);

			GConfig->RemoveKey(*CACHED_AUTH_GOOGLE_SECTION, *CACHED_AUTH_ID, CACHED_AUTH_FILE);
			GConfig->RemoveKey(*CACHED_AUTH_GOOGLE_SECTION, *CACHED_AUTH_TOKEN, CACHED_AUTH_FILE);
			GConfig->Flush(true, CACHED_AUTH_FILE);
		}

		OnLoginFlowComplete(false, Message);
	}
#endif
}

void UPlayerIdentity::StartPlayFabLogin()
{
	PlayFab::ClientModels::FLoginWithGoogleAccountRequest Request;
	Request.CreateAccount = true;
	Request.AccessToken = GoogleAccessToken;

	PlayFabClientPtr ClientApi = IPlayFabModuleInterface::Get().GetClientAPI();

	ClientApi->LoginWithGoogleAccount(Request,
		PlayFab::UPlayFabClientAPI::FLoginWithGoogleAccountDelegate::CreateUObject(this, &UPlayerIdentity::OnPlayFabLoginSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UPlayerIdentity::OnPlayFabFailure));
}

void UPlayerIdentity::OnPlayFabLoginSuccess(const ClientModels::FLoginResult& LoginResult)
{
	PlayFabID = LoginResult.PlayFabId;
	SessionTicket = LoginResult.SessionTicket;

	RequestPlayerIdentityToken(LoginResult.SessionTicket);
}

void UPlayerIdentity::RequestPlayerIdentityToken(const FString& SessionTicket)
{
	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	Content->SetStringField(TEXT("playfabToken"), *FString::Printf(TEXT("%s"), *SessionTicket));

	UOnlineServicesLibrary::SendPOSTRequest("playfab-auth", "exchange_playfab_token", Content, [this](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		if (bSuccess)
		{
			PlayerIdentityToken = Output->GetStringField("playerIdentityToken");
			UE_LOG(LogTemp, Log, TEXT("Get PIT: %s"), *PlayerIdentityToken);
			UpdatePlayFabDisplayName();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get Player Identity Token"));
			OnCompleteDelegate.Broadcast(false, "Failed to get Player Identity Token");
		}
	});
}

void UPlayerIdentity::UpdatePlayFabDisplayName()
{
	PlayFab::ClientModels::FUpdateUserTitleDisplayNameRequest Request;
	Request.DisplayName = DisplayName;

	PlayFabClientPtr ClientApi = IPlayFabModuleInterface::Get().GetClientAPI();
	ClientApi->UpdateUserTitleDisplayName(Request,
		PlayFab::UPlayFabClientAPI::FUpdateUserTitleDisplayNameDelegate::CreateUObject(this, &UPlayerIdentity::OnDisplayNameUpdatedSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UPlayerIdentity::OnPlayFabFailure));
}

void UPlayerIdentity::OnDisplayNameUpdatedSuccess(const ClientModels::FUpdateUserTitleDisplayNameResult& LoginResult)
{
	OnLoginFlowComplete(true, TEXT("Login Successful!"));
}

void UPlayerIdentity::OnLoginFlowComplete(bool Success, const FString& Message)
{
	bCurrentlyLoggingIn = false;
	OnCompleteDelegate.Broadcast(Success, Message);
}

void UPlayerIdentity::OnPlayFabFailure(const FPlayFabCppError& Error)
{
	UE_LOG(LogTemp, Error, TEXT("PlayFab API Error: %s"), *Error.GenerateErrorReport());
	OnCompleteDelegate.Broadcast(false, Error.GenerateErrorReport());
}

void UPlayerIdentity::CreateParty(const FOnOperationComplete& OnComplete)
{
	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "delete_party", PlayerIdentityToken, Content, [this, OnComplete](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(Output.ToSharedRef(), JsonWriter);

		UE_LOG(LogTemp, Log, TEXT("Party response: %s"), *OutputString);

		if (bSuccess)
		{
			TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
			Content->SetNumberField(TEXT("minMembers"), 1);
			Content->SetNumberField(TEXT("maxMembers"), 3);

			UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "create_party", PlayerIdentityToken, Content, [this, OnComplete](const bool bSuccess, TSharedPtr<FJsonObject> Output)
			{
				FString OutputString;
				TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
				FJsonSerializer::Serialize(Output.ToSharedRef(), JsonWriter);

				UE_LOG(LogTemp, Log, TEXT("Party response: %s"), *OutputString);

				if (bSuccess)
				{
					PartyID = Output->GetStringField(TEXT("partyId"));
					OnComplete.ExecuteIfBound(true, PartyID);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to Create Party"));
					OnComplete.ExecuteIfBound(false, TEXT("Failed to Create Party"));
				}
			});
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to Create Party"));
			OnComplete.ExecuteIfBound(false, TEXT("Failed to Create Party"));
		}
	});
}

void UPlayerIdentity::JoinMatchmaking(const FOnOperationComplete& OnComplete)
{
	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	Content->SetStringField(TEXT("matchmakingType"), TEXT("match"));

	UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("gateway", "join", PlayerIdentityToken, Content, [this, OnComplete](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		FString OutputString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(Output.ToSharedRef(), JsonWriter);

		UE_LOG(LogTemp, Log, TEXT("Join response: %s"), *OutputString);

		UOnlineServicesLibrary::SendAuthenticatedGETRequest("gateway", *FString::Printf(TEXT("operations/%s"), *PlayFabID), PlayerIdentityToken, [this, OnComplete](const bool bSuccess, TSharedPtr<FJsonObject> Output)
		{
			FString OutputString;
			TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(Output.ToSharedRef(), JsonWriter);

			UE_LOG(LogTemp, Log, TEXT("Get response: %s"), *OutputString);
		});
		
		// OnComplete.ExecuteIfBound(true, *FString::Printf(TEXT("%s"), *OutputString));
	});
}

FString UPlayerIdentity::GetPlayFabId()
{
	return PlayFabID;
}

FString UPlayerIdentity::GetPlayerDisplayName()
{
	return DisplayName;
}

FString UPlayerIdentity::GetPlayerIdentityToken()
{
	return PlayerIdentityToken;
}

FString UPlayerIdentity::GetSessionTicket()
{
	return SessionTicket;
}

void UPlayerIdentity::DebugLoginWitCustomPlayFabIdAndName(FString CustomID, FString DisplayName)
{
	this->DisplayName = DisplayName;
	PlayFab::ClientModels::FLoginWithCustomIDRequest Request;
	Request.CustomId = CustomID;
	Request.CreateAccount = true;

	PlayFabClientPtr ClientApi = IPlayFabModuleInterface::Get().GetClientAPI();

	ClientApi->LoginWithCustomID(Request,
		PlayFab::UPlayFabClientAPI::FLoginWithCustomIDDelegate::CreateUObject(this, &UPlayerIdentity::OnLoginWithPlayFabCustomID),
		PlayFab::FPlayFabErrorDelegate::CreateLambda([](const PlayFab::FPlayFabCppError&) {
		UE_LOG(LogTemp, Error, TEXT("Failed to login to PlayFab!"));
	}));
}

void UPlayerIdentity::OnLoginWithPlayFabCustomID(const PlayFab::ClientModels::FLoginResult& LoginResult)
{
	PlayFabID = LoginResult.PlayFabId;
	SessionTicket = LoginResult.SessionTicket;

	PlayFab::ClientModels::FUpdateUserTitleDisplayNameRequest Request;
	Request.DisplayName = DisplayName;

	PlayFabClientPtr ClientApi = IPlayFabModuleInterface::Get().GetClientAPI();

	ClientApi->UpdateUserTitleDisplayName(Request,
		PlayFab::UPlayFabClientAPI::FUpdateUserTitleDisplayNameDelegate::CreateUObject(this, &UPlayerIdentity::OnUpdateUserTitleDisplayName),
		PlayFab::FPlayFabErrorDelegate::CreateLambda([](const PlayFab::FPlayFabCppError&) {
		UE_LOG(LogTemp, Error, TEXT("Failed to update PlayFab display name!"));
	})
	);
}

void UPlayerIdentity::OnUpdateUserTitleDisplayName(const PlayFab::ClientModels::FUpdateUserTitleDisplayNameResult& UpdateDisplayNameResult)
{
	DisplayName = UpdateDisplayNameResult.DisplayName;
	UE_LOG(LogTemp, Display, TEXT("Successfully updated display name to %s"), *UpdateDisplayNameResult.DisplayName);
}
