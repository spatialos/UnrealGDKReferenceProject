// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "ShooterGameInstance.h"

#include "OnlineServicesLibrary.h"

void UShooterGameInstance::Init()
{
	Super::Init();

	PlayerIdentity = NewObject<UPlayerIdentity>(this);

	SetupPlayFab();
}

void UShooterGameInstance::SetupPlayFab()
{
	UOnlineServicesLibrary::SetupPlayFabSettings();
}
