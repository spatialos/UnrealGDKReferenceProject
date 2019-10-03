// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "PlayerIdentity.h"

#include "PartySystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct GDKSHOOTER_API FFriendDetails
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerId;
};

USTRUCT(BlueprintType)
struct GDKSHOOTER_API FInviteDetails
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString InviteId;

	UPROPERTY(BlueprintReadOnly)
	FString PartyId;
};

USTRUCT(BlueprintType)
struct GDKSHOOTER_API FFriendDetailsList
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FFriendDetails> Friends;
};

USTRUCT(BlueprintType)
struct GDKSHOOTER_API FInviteDetailsList
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FFriendDetails> SenderDetails;

	UPROPERTY(BlueprintReadOnly)
	TArray<FInviteDetails> InviteDetails;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FRequestResponse, FString, ResponseMessage, bool, bSuccess);
DECLARE_DELEGATE_OneParam(FRequestResponse_Internal, bool);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFriendListResponse, FFriendDetailsList, FriendsList);
DECLARE_DYNAMIC_DELEGATE_OneParam(FInvitesListResponse, FInviteDetailsList, InvitesList);

UCLASS()
class GDKSHOOTER_API UPartySystem : public UObject
{
	GENERATED_BODY()

public:
	// True if the PlayerIdentity etc. are ready for requests to be sent.
	UFUNCTION(BlueprintPure)
	bool IsReady();

	// At the start, 
	UFUNCTION(BlueprintCallable, Category = "PartySystem")
	void InitialiseToEmptyParty(const FRequestResponse& OnInitialisedResponse);

	UFUNCTION(BlueprintCallable, Category = "PartySystem")
	void JoinParty(FInviteDetails InviteDetails);
	UFUNCTION(BlueprintCallable, Category = "PartySystem")
	void LeavePartyAndCreateOwn();
	UFUNCTION(BlueprintCallable, Category = "PartySystem")
	void GetCurrentParty(const FFriendListResponse& OnPartyReceived);

	UFUNCTION(BlueprintCallable, Category = "PartySystem")
	void SendInviteByName(FString Name, const FRequestResponse& OnInviteResponse);
	UFUNCTION(BlueprintCallable, Category = "PartySystem")
	void SendInviteById(FString Id, const FRequestResponse& OnInviteResponse);
	UFUNCTION(BlueprintCallable, Category = "PartySystem")
	void GetInvites(const FInvitesListResponse& OnListCreated);

	// StavkaFriends needs access to the StavkaIdentity, to get the PIT, SessionTicket etc.
	UFUNCTION(BlueprintCallable, Category = "PartySystem")
	void InformOfPlayerIdentity(UPlayerIdentity* PlayerIdentity);

	UPlayerIdentity* GetPlayerIdentity() { return IsValid(PlayerIdentity) ? PlayerIdentity : nullptr; }

private:
	void CreateParty(const FRequestResponse_Internal& Response);
	void DeleteParty(const FRequestResponse_Internal& Response);
	void LeaveParty(const FRequestResponse_Internal& Response);
	void DeleteInvite(FString InviteId);

	UPlayerIdentity* PlayerIdentity;
	FString CurrentPartyId;

	// Cache the friend details you have (mapping from Ids to names), so you don't have to request each time it updates.
	TMap<FString, FFriendDetails> CachedFriendDetails;

	TArray<FFriendDetails> GetFriendDetails(TArray<FString> PlayerIds);
	void CacheDetailsFromPlayFab(FString PlayerId);
};
