// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "EngineClasses/SpatialGameInstance.h"
#include "ExternalSchemaCodegen/ExternalSchemaInterface.h"
#include "CorridorShooterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GDKSHOOTER_API UCorridorShooterGameInstance : public USpatialGameInstance
{
	GENERATED_BODY()
	
public:

	void Init();

	ExternalSchemaInterface* GetExternalSchemaInterface()
	{
		return ExternalSchema;
	}

	Worker_EntityId GetHierarchyServiceId()
	{
		return HierarchyServiceId;
	}

protected:

	UPROPERTY()
		TMap<FString, int32> PlayerScoreMap;

	ExternalSchemaInterface* ExternalSchema;
	Worker_EntityId HierarchyServiceId = 0;
};
