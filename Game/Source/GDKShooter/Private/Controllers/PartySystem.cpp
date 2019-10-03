// Copyright (c) Improbable Worlds Ltd, All Rights Reserved


#include "PartySystem.h"
#include "JsonObject.h"
#include "JsonObjectConverter.h"
#include "PlayerIdentity.h"
#include "OnlineServicesLibrary.h"

bool UPartySystem::IsReady()
{
	return PlayerIdentity != nullptr && PlayerIdentity->GetPlayerIdentityToken() != "";
}

void UPartySystem::InformOfPlayerIdentity(UPlayerIdentity* NewPlayerIdentity)
{
	PlayerIdentity = NewPlayerIdentity;

	FFriendDetails OwnDetails = FFriendDetails();
	OwnDetails.PlayerId = PlayerIdentity->GetPlayFabId();
	OwnDetails.DisplayName = PlayerIdentity->GetPlayerDisplayName();
}

void UPartySystem::InitialiseToEmptyParty(const FRequestResponse& OnInitialisedResponse)
{
	DeleteParty(FRequestResponse_Internal::CreateLambda([this, OnInitialisedResponse](bool bSuccess) {
		if (bSuccess)
		{
			CreateParty(FRequestResponse_Internal::CreateLambda([this, OnInitialisedResponse](bool bSuccess) {
				if (bSuccess)
				{
					OnInitialisedResponse.ExecuteIfBound("", true);
				}
				else
				{
					OnInitialisedResponse.ExecuteIfBound("Failed to create party", false);
				}
			}));
		}
		else
		{
			OnInitialisedResponse.ExecuteIfBound("Failed to delete party", false);
		}
	}));
}

void UPartySystem::CreateParty(const FRequestResponse_Internal& CreateResponse)
{
	//rpc CreateParty(CreatePartyRequest) returns(CreatePartyResponse) {
	//	option(google.api.http) = {
	//		post: "/v1/create_party"
	//		body : "*"
	//	};
	//}

	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	Content->SetNumberField(TEXT("minMembers"), 1);
	Content->SetNumberField(TEXT("maxMembers"), 5);
	UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "create_party", PlayerIdentity->GetPlayerIdentityToken(), Content, [this, CreateResponse](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		UE_LOG(LogTemp, Log, TEXT("Success: %d"), bSuccess);
		if (bSuccess)
		{
			CurrentPartyId = UOnlineServicesLibrary::GetFieldFromJson(Output, "partyId");
		}
		CreateResponse.ExecuteIfBound(bSuccess);
	});
}

void UPartySystem::DeleteParty(const FRequestResponse_Internal& DeleteResponse)
{
	//rpc DeleteParty(DeletePartyRequest) returns(DeletePartyResponse) {
	//	option(google.api.http) = {
	//		post: "/v1/delete_party"
	//	};

	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "delete_party", PlayerIdentity->GetPlayerIdentityToken(), Content, [this, DeleteResponse](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		UE_LOG(LogTemp, Log, TEXT("Success: %d"), bSuccess);
		DeleteResponse.ExecuteIfBound(bSuccess);
	});
}

void UPartySystem::LeaveParty(const FRequestResponse_Internal& LeaveResponse)
{
	//rpc LeaveParty(LeavePartyRequest) returns(LeavePartyResponse) {
	//	option(google.api.http) = {
	//		post: "/v1/leave_party"
	//	};
	//}

	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "leave_party", PlayerIdentity->GetPlayerIdentityToken(), Content, [this, LeaveResponse](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{		
		UE_LOG(LogTemp, Log, TEXT("Success: %d"), bSuccess);
		if (bSuccess)
		{
			LeaveResponse.ExecuteIfBound(bSuccess);
		}
	});
}

void UPartySystem::JoinParty(FInviteDetails InviteDetails)
{
	//rpc JoinParty(JoinPartyRequest) returns(JoinPartyResponse) {
	//	option(google.api.http) = {
	//		post: "/v1/join_party/{party_id}"
	//	};
	//}

	// You need to leave your current party to join your new party.
	// You can't leave a party of size 1, so instead delete it.

	DeleteParty(FRequestResponse_Internal::CreateLambda([this, InviteDetails](bool bSuccess) {
		if (bSuccess)
		{
			TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
			UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "join_party/" + InviteDetails.PartyId, PlayerIdentity->GetPlayerIdentityToken(), Content, [this, InviteDetails](const bool bSuccess, TSharedPtr<FJsonObject> Output)
			{
				UE_LOG(LogTemp, Log, TEXT("Success: %d"), bSuccess);
				if (bSuccess)
				{
					CurrentPartyId = UOnlineServicesLibrary::GetFieldFromJson(Output, "partyId");

					DeleteInvite(InviteDetails.InviteId);
				}
			});
		}
	}));
}

void UPartySystem::DeleteInvite(FString InviteId)
{
	//rpc DeleteInvite(DeleteInviteRequest) returns(DeleteInviteResponse) {
	//	option(google.api.http) = {
	//		post: "/v1/delete_invite/{invite_id}"
	//	};
	//}

	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "delete_invite/" + InviteId, PlayerIdentity->GetPlayerIdentityToken(), Content, [this](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		UE_LOG(LogTemp, Log, TEXT("Success: %d"), bSuccess);
	});
}

void UPartySystem::LeavePartyAndCreateOwn()
{
	LeaveParty(FRequestResponse_Internal::CreateLambda([this](bool bSuccess) {
		if (bSuccess)
		{
			CreateParty(FRequestResponse_Internal::CreateLambda([this](bool bSuccess) {
				UE_LOG(LogTemp, Log, TEXT("Successfully left and recreated party."));
			}));
		}
	}));
}

void UPartySystem::SendInviteByName(FString Name, const FRequestResponse& OnInviteResponse)
{
	// Send a request to playfab to get the player ID.

	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	Content->SetStringField("TitleDisplayName", Name);
	UOnlineServicesLibrary::SendAuthenticatedPlayFabPOSTRequest("/Client/GetAccountInfo", PlayerIdentity->GetSessionTicket(), Content, [this, Name, OnInviteResponse](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		if (!bSuccess)
		{
			OnInviteResponse.ExecuteIfBound("Failed to get the Id from PlayFab.", false);
		}
		else
		{
			// Json is structured as data -> AccountInfo -> PlayFabId
			FString Data = UOnlineServicesLibrary::GetFieldFromJson(Output, "data");
			FString AccountInfo = UOnlineServicesLibrary::GetFieldFromJson(Data, "AccountInfo");
			FString Id = UOnlineServicesLibrary::GetFieldFromJson(AccountInfo, "PlayFabId");
			if (Id == "")
			{
				OnInviteResponse.ExecuteIfBound("Could not find player with name " + Name, false);
			}
			else
			{
				SendInviteById(Id, OnInviteResponse);
			}
		}
	});
}

void UPartySystem::SendInviteById(FString Id, const FRequestResponse& OnInviteResponse)
{
	//rpc CreateInvite(CreateInviteRequest) returns(CreateInviteResponse) {
	//	option(google.api.http) = {
	//		post: "/v1/create_invite"
	//		body : "*"
	//	};
	//}

	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	Content->SetStringField(TEXT("receiverPlayerId"), Id);
	UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "create_invite", PlayerIdentity->GetPlayerIdentityToken(), Content, [this, OnInviteResponse](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		if (bSuccess)
		{
			FString InviteId = UOnlineServicesLibrary::GetFieldFromJson(Output, "inviteId");
			if (InviteId != "")
			{
				OnInviteResponse.ExecuteIfBound("Invite successfully sent!", true);
				return;
			}
			FString Message = UOnlineServicesLibrary::GetFieldFromJson(Output, "message");
			if (Message != "")
			{
				OnInviteResponse.ExecuteIfBound(Message, false);
				return;
			}
		}
		OnInviteResponse.ExecuteIfBound("Failed to send invite to player...", false);
	});
}

void UPartySystem::GetInvites(const FInvitesListResponse& OnListCreated)
{
	//rpc ListAllInvites(ListAllInvitesRequest) returns(ListAllInvitesResponse) {
	//	option(google.api.http) = {
	//		post: "/v1/list_all_invites"
	//	};
	//}

	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "list_all_invites", PlayerIdentity->GetPlayerIdentityToken(), Content, [this, OnListCreated](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		if (bSuccess)
		{
			TArray<FString> InboundInvites = UOnlineServicesLibrary::GetListFromJson(Output, "inboundInvites");

			FInviteDetailsList InvitesList = FInviteDetailsList();
			InvitesList.SenderDetails = TArray<FFriendDetails>();
			InvitesList.InviteDetails = TArray<FInviteDetails>();

			TArray<FString> SenderIds = TArray<FString>();

			for (int n = 0; n < InboundInvites.Num(); n++)
			{
				FString Invite = InboundInvites[n];
				FString CurrentStatus = UOnlineServicesLibrary::GetFieldFromJson(Invite, "currentStatus");
				if (CurrentStatus == "PENDING")
				{
					FInviteDetails InviteDetails = FInviteDetails();
					InviteDetails.InviteId = UOnlineServicesLibrary::GetFieldFromJson(Invite, "id");
					InviteDetails.PartyId = UOnlineServicesLibrary::GetFieldFromJson(Invite, "partyId");
					FString PlayerId = UOnlineServicesLibrary::GetFieldFromJson(Invite, "senderPlayerId");

					SenderIds.Add(PlayerId);
					InvitesList.InviteDetails.Add(InviteDetails);
				}
			}
			InvitesList.SenderDetails = GetFriendDetails(SenderIds);
			OnListCreated.ExecuteIfBound(InvitesList);
		}
	});
}

void UPartySystem::GetCurrentParty(const FFriendListResponse& OnPartyReceived)
{
	//rpc GetPartyByPlayerId(GetPartyByPlayerIdRequest) returns(GetPartyByPlayerIdResponse) {
	//	option(google.api.http) = {
	//		post: "/v1/get_party_by_player_id"
	//	};

	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	UOnlineServicesLibrary::SendAuthenticatedPOSTRequest("party", "get_party_by_player_id", PlayerIdentity->GetPlayerIdentityToken(), Content, [this, OnPartyReceived](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		if (bSuccess)
		{
			FString Party = UOnlineServicesLibrary::GetFieldFromJson(Output, "party");
			TArray<FString> PartyIds = UOnlineServicesLibrary::GetListFromJson(Party, "memberIds");

			FFriendDetailsList FriendsList = FFriendDetailsList();
			FriendsList.Friends = GetFriendDetails(PartyIds);
			OnPartyReceived.ExecuteIfBound(FriendsList);
		}
	});
}

TArray<FFriendDetails> UPartySystem::GetFriendDetails(TArray<FString> PlayerIds)
{
	TArray<FFriendDetails> ResultList = TArray<FFriendDetails>();
	for (int n = 0; n < PlayerIds.Num(); n++)
	{
		if (CachedFriendDetails.Contains(PlayerIds[n]))
		{
			ResultList.Add(CachedFriendDetails[PlayerIds[n]]);
		}
		else
		{
			FFriendDetails Details = FFriendDetails();
			Details.PlayerId = PlayerIds[n];
			Details.DisplayName = "Acquiring name...";
			ResultList.Add(Details);
			CacheDetailsFromPlayFab(PlayerIds[n]);
		}
	}
	return ResultList;
}

void UPartySystem::CacheDetailsFromPlayFab(FString PlayerId)
{
	// Send a request to playfab to get the player ID.

	TSharedPtr<FJsonObject> Content = MakeShareable(new FJsonObject());
	Content->SetStringField("PlayFabId", PlayerId);
	UOnlineServicesLibrary::SendAuthenticatedPlayFabPOSTRequest("/Client/GetAccountInfo", PlayerIdentity->GetSessionTicket(), Content, [this, PlayerId](const bool bSuccess, TSharedPtr<FJsonObject> Output)
	{
		if (bSuccess)
		{
			// Json is structured as data -> AccountInfo -> PlayFabId
			FString Data = UOnlineServicesLibrary::GetFieldFromJson(Output, "data");
			FString AccountInfo = UOnlineServicesLibrary::GetFieldFromJson(Data, "AccountInfo");
			FString TitleInfo = UOnlineServicesLibrary::GetFieldFromJson(AccountInfo, "TitleInfo");
			FString DisplayName = UOnlineServicesLibrary::GetFieldFromJson(TitleInfo, "DisplayName");
			if (DisplayName != "")
			{
				FFriendDetails Details = FFriendDetails();
				Details.PlayerId = PlayerId;
				Details.DisplayName = DisplayName;
				CachedFriendDetails.Add(PlayerId, Details);
			}
		}
	});
}
