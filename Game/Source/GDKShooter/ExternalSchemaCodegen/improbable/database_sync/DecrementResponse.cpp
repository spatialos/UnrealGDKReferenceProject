﻿// Generated by Unreal External Schema Codegen

#include "DecrementResponse.h"
#include <set>
#include "../../MapEquals.h"

// Generated from C:\dev\UnrealEngine\Samples\UnrealGDKReferenceProject\dbsync\database_sync.schema(151,1)
namespace improbable {
namespace database_sync {

DecrementResponse::DecrementResponse(
	int64 NewCount)
: _NewCount{ NewCount } {}

DecrementResponse::DecrementResponse() {}

bool DecrementResponse::operator==(const DecrementResponse& Value) const
{
	return _NewCount == Value._NewCount;
}

bool DecrementResponse::operator!=(const DecrementResponse& Value) const
{
	return !operator== (Value);
}

int64 DecrementResponse::GetNewCount() const
{
	return _NewCount;
}

int64& DecrementResponse::GetNewCount()
{
	return _NewCount; 
}

DecrementResponse& DecrementResponse::SetNewCount(int64 Value)
{
	_NewCount = Value;
	return *this;
}

void DecrementResponse::Serialize(Schema_Object* SchemaObject) const
{
	// serializing field NewCount = 1
	Schema_AddInt64(SchemaObject, 1, _NewCount);
}

DecrementResponse DecrementResponse::Deserialize(Schema_Object* SchemaObject)
{
	DecrementResponse Data;
	
	// deserializing field NewCount = 1
	Data._NewCount = Schema_GetInt64(SchemaObject, 1);
	
	return Data;
}

uint32 GetTypeHash(const improbable::database_sync::DecrementResponse& Value)
{
	uint32 Result = 1327;
	Result = (Result * 977) + ::GetTypeHash(Value.GetNewCount());
	return Result;
}

} // namespace database_sync
} // namespace improbable
