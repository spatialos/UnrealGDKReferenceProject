// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "OnlineServicesLibrary.h"

#include "PlayFab.h"
#include "Core/PlayFabAdminAPI.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "JsonUtilities.h"

void UOnlineServicesLibrary::SetupPlayFabSettings()
{
	// Get the CDO so we can read the config values.
	UOnlineServicesLibrary* DefaultObject = Cast<UOnlineServicesLibrary>(UOnlineServicesLibrary::StaticClass()->GetDefaultObject());

	PlayFabAdminPtr AdminApi = IPlayFabModuleInterface::Get().GetAdminAPI();
	AdminApi->SetTitleId(DefaultObject->PlayFabTitleID);
	AdminApi->SetDevSecretKey(DefaultObject->PlayFabSecretKey);
}

void UOnlineServicesLibrary::SendHTTPRequest(FString Endpoint, FString Path, TSharedPtr<FJsonObject> Content, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback)
{
	// Get the CDO so we can read the config values.
	UOnlineServicesLibrary* DefaultObject = Cast<UOnlineServicesLibrary>(UOnlineServicesLibrary::StaticClass()->GetDefaultObject());

	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Content.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetURL(*FString::Printf(TEXT("http://%s.endpoints.%s.cloud.goog/v1/%s"), *Endpoint, *DefaultObject->GoogleCloudProjectID, *Path));

	HttpRequest->SetContentAsString(OutputString);

	HttpRequest->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response->GetContentType() == "application/json")
		{
			TSharedPtr<FJsonObject> Output = MakeShareable(new FJsonObject());
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
			FJsonSerializer::Deserialize(JsonReader, Output);

			Callback(true, Output);
		}
		else
		{
			Callback(false, nullptr);
		}
	});

	HttpRequest->ProcessRequest();
}
