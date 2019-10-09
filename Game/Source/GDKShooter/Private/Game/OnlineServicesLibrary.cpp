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

void UOnlineServicesLibrary::SendPOSTRequest(FString Endpoint, FString Path, TSharedPtr<FJsonObject> Content, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback)
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

	UE_LOG(LogTemp, Log, TEXT("Sent POST request to %s: %s"), *HttpRequest->GetURL(), *OutputString);

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

void UOnlineServicesLibrary::SendAuthenticatedPOSTRequest(FString Endpoint, FString Path, FString PIT, TSharedPtr<FJsonObject> Content, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback)
{
	// Get the CDO so we can read the config values.
	UOnlineServicesLibrary* DefaultObject = Cast<UOnlineServicesLibrary>(UOnlineServicesLibrary::StaticClass()->GetDefaultObject());

	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Content.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetHeader("player-identity-token", PIT);
	HttpRequest->SetURL(*FString::Printf(TEXT("http://%s.endpoints.%s.cloud.goog/v1/%s"), *Endpoint, *DefaultObject->GoogleCloudProjectID, *Path));

	UE_LOG(LogTemp, Log, TEXT("Sent POST request to %s: %s"), *HttpRequest->GetURL(), *OutputString);

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

void UOnlineServicesLibrary::SendAuthenticatedGETRequest(FString Endpoint, FString Path, FString PIT, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback)
{
	// Get the CDO so we can read the config values.
	UOnlineServicesLibrary* DefaultObject = Cast<UOnlineServicesLibrary>(UOnlineServicesLibrary::StaticClass()->GetDefaultObject());

	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetVerb("GET");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetHeader("player-identity-token", PIT);
	HttpRequest->SetURL(*FString::Printf(TEXT("http://%s.endpoints.%s.cloud.goog/v1/%s"), *Endpoint, *DefaultObject->GoogleCloudProjectID, *Path));

	UE_LOG(LogTemp, Log, TEXT("Sent GET request to %s"), *HttpRequest->GetURL());

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

void UOnlineServicesLibrary::SendAuthenticatedPlayFabPOSTRequest(FString Path, FString SessionTicket, TSharedPtr<FJsonObject> Content, TFunction<void(const bool, TSharedPtr<FJsonObject>)> Callback)
{
	UOnlineServicesLibrary* DefaultObject = Cast<UOnlineServicesLibrary>(UOnlineServicesLibrary::StaticClass()->GetDefaultObject());

	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Content.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetHeader("X-Authentication", SessionTicket);
	HttpRequest->SetURL(*FString::Printf(TEXT("https://%s.playfabapi.com/%s"), *DefaultObject->PlayFabTitleID, *Path));

	UE_LOG(LogTemp, Log, TEXT("Sent POST request to %s: %s"), *HttpRequest->GetURL(), *OutputString);

	HttpRequest->SetContentAsString(OutputString);

	HttpRequest->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful)
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

FString UOnlineServicesLibrary::GetFieldFromJson(FString ContentString, FString StringField)
{
	if (ContentString == "")
	{
		return "";
	}

	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		return GetFieldFromJson(JsonObject, StringField);
	}

	return "";
}

FString UOnlineServicesLibrary::GetFieldFromJson(TSharedPtr<FJsonObject> Content, FString StringField)
{
	FString ResultString;
	if (Content->TryGetStringField(StringField, ResultString))
	{
		return ResultString;
	}
	else
	{
		// If the result is an object, convert it to a string.
		const TSharedPtr<FJsonObject>* FieldAsJsonObject;
		if (Content->TryGetObjectField(StringField, FieldAsJsonObject))
		{
			FString OutputString;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(FieldAsJsonObject->ToSharedRef(), Writer);
			return OutputString;
		}

		UE_LOG(LogTemp, Warning, TEXT("The content string does not contain the field %s"), *StringField);

	}

	return "";
}

TArray<FString> UOnlineServicesLibrary::GetListFromJson(FString ContentString, FString StringField)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentString);

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		return GetListFromJson(JsonObject, StringField);
	}

	return TArray<FString>();
}

TArray<FString> UOnlineServicesLibrary::GetListFromJson(TSharedPtr<FJsonObject> Content, FString StringField)
{
	TArray<FString> StringList;

	const TArray<TSharedPtr<FJsonValue>>* Array;
	if (Content->TryGetArrayField(StringField, Array))
	{
		for (TSharedPtr<FJsonValue> JsonValue : *Array)
		{
			FString JsonAsString;
			if (JsonValue->TryGetString(JsonAsString))
			{
				StringList.Add(JsonAsString);
			}
			else
			{
				FString OutputString;
				TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
				FJsonSerializer::Serialize(JsonValue->AsObject().ToSharedRef(), Writer);
				StringList.Add(OutputString);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The content string does not contain the field %s"), *StringField);
	}

	return StringList;
}
