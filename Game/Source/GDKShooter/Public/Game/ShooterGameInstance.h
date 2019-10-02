// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "EngineClasses/SpatialGameInstance.h"
#include "PlayerIdentity.h"
#include "ShooterGameInstance.generated.h"


UCLASS()
class GDKSHOOTER_API UShooterGameInstance : public USpatialGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init();

	UPROPERTY(BlueprintReadOnly)
	class UPlayerIdentity* PlayerIdentity;

private:
	void SetupPlayFab();
};
