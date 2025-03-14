﻿// Generated by Unreal External Schema Codegen

#pragma once

#include "CoreMinimal.h"
#include "Utils/SchemaOption.h"
#include <WorkerSDK/improbable/c_schema.h>
#include <WorkerSDK/improbable/c_worker.h>

#include "../../ExternalSchemaHelperFunctions.h"

namespace improbable {
namespace database_sync {

// Generated from C:\dev\UnrealEngine\Samples\UnrealGDKReferenceProject\dbsync\database_sync.schema(213,1)
class AssociatePathWithClientRequest : public improbable::SpatialType
{
public:
	// Creates a new instance with specified arguments for each field.
	AssociatePathWithClientRequest(const FString& Path, const FString& ClientWorkerId);
	// Creates a new instance with default values for each field.
	AssociatePathWithClientRequest();
	// Creates a new instance with default values for each field. This is
	// equivalent to a default-constructed instance.
	static AssociatePathWithClientRequest Create() { return {}; }
	// Copyable and movable.
	AssociatePathWithClientRequest(AssociatePathWithClientRequest&&) = default;
	AssociatePathWithClientRequest(const AssociatePathWithClientRequest&) = default;
	AssociatePathWithClientRequest& operator=(AssociatePathWithClientRequest&&) = default;
	AssociatePathWithClientRequest& operator=(const AssociatePathWithClientRequest&) = default;
	~AssociatePathWithClientRequest() = default;
	
	bool operator==(const AssociatePathWithClientRequest&) const;
	bool operator!=(const AssociatePathWithClientRequest&) const;
	
	// Serialize this object data into the C API argument
	void Serialize(Schema_Object* SchemaObject) const override;
	
	// Deserialize the C API object argument into an instance of this class and return it
	static AssociatePathWithClientRequest Deserialize(Schema_Object* SchemaObject);
	
	// Field Path = 1
	const FString& GetPath() const;
	FString& GetPath();
	AssociatePathWithClientRequest& SetPath(const FString&);
	
	// Field ClientWorkerId = 2
	const FString& GetClientWorkerId() const;
	FString& GetClientWorkerId();
	AssociatePathWithClientRequest& SetClientWorkerId(const FString&);
	
private:
	FString _Path;
	FString _ClientWorkerId;
};

inline uint32 GetTypeHash(const AssociatePathWithClientRequest& Value);

} // namespace database_sync
} // namespace improbable

