﻿// Generated by Unreal External Schema Codegen

#pragma once

#include "CoreMinimal.h"
#include "Utils/SchemaOption.h"
#include <WorkerSDK/improbable/c_schema.h>
#include <WorkerSDK/improbable/c_worker.h>

#include "../../ExternalSchemaHelperFunctions.h"

namespace improbable {
namespace database_sync {

// Generated from C:\dev\UnrealEngine\Samples\UnrealGDKReferenceProject\dbsync\database_sync.schema(117,1)
class IncrementRequest : public improbable::SpatialType
{
public:
	// Creates a new instance with specified arguments for each field.
	IncrementRequest(const FString& Path, int64 Amount, const FString& WorkerId);
	// Creates a new instance with default values for each field.
	IncrementRequest();
	// Creates a new instance with default values for each field. This is
	// equivalent to a default-constructed instance.
	static IncrementRequest Create() { return {}; }
	// Copyable and movable.
	IncrementRequest(IncrementRequest&&) = default;
	IncrementRequest(const IncrementRequest&) = default;
	IncrementRequest& operator=(IncrementRequest&&) = default;
	IncrementRequest& operator=(const IncrementRequest&) = default;
	~IncrementRequest() = default;
	
	bool operator==(const IncrementRequest&) const;
	bool operator!=(const IncrementRequest&) const;
	
	// Serialize this object data into the C API argument
	void Serialize(Schema_Object* SchemaObject) const override;
	
	// Deserialize the C API object argument into an instance of this class and return it
	static IncrementRequest Deserialize(Schema_Object* SchemaObject);
	
	// Field Path = 1
	const FString& GetPath() const;
	FString& GetPath();
	IncrementRequest& SetPath(const FString&);
	
	// Field Amount = 2
	int64 GetAmount() const;
	int64& GetAmount();
	IncrementRequest& SetAmount(int64);
	
	// Field WorkerId = 3
	const FString& GetWorkerId() const;
	FString& GetWorkerId();
	IncrementRequest& SetWorkerId(const FString&);
	
private:
	FString _Path;
	int64 _Amount;
	FString _WorkerId;
};

inline uint32 GetTypeHash(const IncrementRequest& Value);

} // namespace database_sync
} // namespace improbable

