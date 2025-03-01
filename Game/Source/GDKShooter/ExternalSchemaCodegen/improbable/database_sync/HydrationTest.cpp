﻿// Generated by Unreal External Schema Codegen

#include "HydrationTest.h"
#include <set>
#include "../../MapEquals.h"

// Generated from C:\dev\UnrealEngine\Samples\UnrealGDKReferenceProject\dbsync\database_sync.schema(320,1)
namespace improbable {
namespace database_sync {

HydrationTest::HydrationTest(
	const FString& ProfileId, 
	int64 ValueField1, 
	int64 ValueField2, 
	const TArray<::improbable::database_sync::DatabaseSyncItem>& ItemList)
: _ProfileId{ ProfileId }
, _ValueField1{ ValueField1 }
, _ValueField2{ ValueField2 }
, _ItemList{ ItemList } {}

HydrationTest::HydrationTest() {}

bool HydrationTest::operator==(const HydrationTest& Value) const
{
	return _ProfileId == Value._ProfileId && 
	_ValueField1 == Value._ValueField1 && 
	_ValueField2 == Value._ValueField2 && 
	_ItemList == Value._ItemList;
}

bool HydrationTest::operator!=(const HydrationTest& Value) const
{
	return !operator== (Value);
}

const FString& HydrationTest::GetProfileId() const
{
	return _ProfileId;
}

FString& HydrationTest::GetProfileId()
{
	return _ProfileId; 
}

HydrationTest& HydrationTest::SetProfileId(const FString& Value)
{
	_ProfileId = Value;
	return *this;
}

int64 HydrationTest::GetValueField1() const
{
	return _ValueField1;
}

int64& HydrationTest::GetValueField1()
{
	return _ValueField1; 
}

HydrationTest& HydrationTest::SetValueField1(int64 Value)
{
	_ValueField1 = Value;
	return *this;
}

int64 HydrationTest::GetValueField2() const
{
	return _ValueField2;
}

int64& HydrationTest::GetValueField2()
{
	return _ValueField2; 
}

HydrationTest& HydrationTest::SetValueField2(int64 Value)
{
	_ValueField2 = Value;
	return *this;
}

const TArray<::improbable::database_sync::DatabaseSyncItem>& HydrationTest::GetItemList() const
{
	return _ItemList;
}

TArray<::improbable::database_sync::DatabaseSyncItem>& HydrationTest::GetItemList()
{
	return _ItemList; 
}

HydrationTest& HydrationTest::SetItemList(const TArray<::improbable::database_sync::DatabaseSyncItem>& Value)
{
	_ItemList = Value;
	return *this;
}

void HydrationTest::Serialize(Schema_ComponentData* ComponentData) const
{
	Schema_Object* FieldsObject = Schema_GetComponentDataFields(ComponentData);

	// serializing field ProfileId = 1
	::improbable::utils::AddString(FieldsObject, 1, _ProfileId);
	// serializing field ValueField1 = 2
	Schema_AddInt64(FieldsObject, 2, _ValueField1);
	// serializing field ValueField2 = 3
	Schema_AddInt64(FieldsObject, 3, _ValueField2);
	// serializing field ItemList = 4
	for (const ::improbable::database_sync::DatabaseSyncItem& Element : _ItemList)
	{
		Element.Serialize(Schema_AddObject(FieldsObject, 4));
	}
}

HydrationTest HydrationTest::Deserialize(Schema_ComponentData* ComponentData)
{
	Schema_Object * FieldsObject = Schema_GetComponentDataFields(ComponentData);
	HydrationTest Data;
	
	// deserializing field ProfileId = 1
	Data._ProfileId = ::improbable::utils::GetString(FieldsObject, 1);
	
	// deserializing field ValueField1 = 2
	Data._ValueField1 = Schema_GetInt64(FieldsObject, 2);
	
	// deserializing field ValueField2 = 3
	Data._ValueField2 = Schema_GetInt64(FieldsObject, 3);
	
	// deserializing field ItemList = 4
	{
		auto ListLength = Schema_GetObjectCount(FieldsObject, 4);
		TArray<::improbable::database_sync::DatabaseSyncItem> ItemListList;
		ItemListList.SetNum(ListLength);
		for (uint32 i = 0; i < ListLength; ++i)
		{
			ItemListList[i] = ::improbable::database_sync::DatabaseSyncItem::Deserialize(Schema_IndexObject(FieldsObject, 4, i));
		}
		Data._ItemList = ItemListList;
	}
	
	return Data;
}

bool HydrationTest::Update::operator==(const HydrationTest::Update& Value) const
{
	return _ProfileId == Value._ProfileId && 
	_ValueField1 == Value._ValueField1 && 
	_ValueField2 == Value._ValueField2 && 
	_ItemList == Value._ItemList;
}

bool HydrationTest::Update::operator!=(const HydrationTest::Update& Value) const
{
	return !operator== (Value);
}

HydrationTest::Update HydrationTest::Update::FromInitialData(const HydrationTest& Data)
{ 
	HydrationTest::Update Update;
	Update._ProfileId = Data.GetProfileId();
	Update._ValueField1 = Data.GetValueField1();
	Update._ValueField2 = Data.GetValueField2();
	Update._ItemList = Data.GetItemList();
	return Update;
}

HydrationTest HydrationTest::Update::ToInitialData() const
{
	return HydrationTest (
		*_ProfileId,
		*_ValueField1,
		*_ValueField2,
		*_ItemList);
}         

void HydrationTest::Update::ApplyTo(HydrationTest& Data) const
{
	if (_ProfileId)
	{
		Data.SetProfileId(*_ProfileId);
	}
	if (_ValueField1)
	{
		Data.SetValueField1(*_ValueField1);
	}
	if (_ValueField2)
	{
		Data.SetValueField2(*_ValueField2);
	}
	if (_ItemList)
	{
		Data.SetItemList(*_ItemList);
	}
}

const SpatialGDK::TSchemaOption<FString>& HydrationTest::Update::GetProfileId() const
{
	return _ProfileId;
}

SpatialGDK::TSchemaOption<FString>& HydrationTest::Update::GetProfileId()
{
	return _ProfileId;
}

HydrationTest::Update& HydrationTest::Update::SetProfileId(const FString& value)
{
	_ProfileId = value;
	return *this;
}

const SpatialGDK::TSchemaOption<int64>& HydrationTest::Update::GetValueField1() const
{
	return _ValueField1;
}

SpatialGDK::TSchemaOption<int64>& HydrationTest::Update::GetValueField1()
{
	return _ValueField1;
}

HydrationTest::Update& HydrationTest::Update::SetValueField1(int64 value)
{
	_ValueField1 = value;
	return *this;
}

const SpatialGDK::TSchemaOption<int64>& HydrationTest::Update::GetValueField2() const
{
	return _ValueField2;
}

SpatialGDK::TSchemaOption<int64>& HydrationTest::Update::GetValueField2()
{
	return _ValueField2;
}

HydrationTest::Update& HydrationTest::Update::SetValueField2(int64 value)
{
	_ValueField2 = value;
	return *this;
}

const SpatialGDK::TSchemaOption<TArray<::improbable::database_sync::DatabaseSyncItem>>& HydrationTest::Update::GetItemList() const
{
	return _ItemList;
}

SpatialGDK::TSchemaOption<TArray<::improbable::database_sync::DatabaseSyncItem>>& HydrationTest::Update::GetItemList()
{
	return _ItemList;
}

HydrationTest::Update& HydrationTest::Update::SetItemList(const TArray<::improbable::database_sync::DatabaseSyncItem>& value)
{
	_ItemList = value;
	return *this;
}

void HydrationTest::Update::Serialize(Schema_ComponentUpdate* ComponentUpdate) const
{
	Schema_Object* UpdatesObject = Schema_GetComponentUpdateFields(ComponentUpdate);
	// serializing field ProfileId = 1
	if (_ProfileId.IsSet())
	{
		::improbable::utils::AddString(UpdatesObject, 1, (*_ProfileId));
	}
	// serializing field ValueField1 = 2
	if (_ValueField1.IsSet())
	{
		Schema_AddInt64(UpdatesObject, 2, (*_ValueField1));
	}
	// serializing field ValueField2 = 3
	if (_ValueField2.IsSet())
	{
		Schema_AddInt64(UpdatesObject, 3, (*_ValueField2));
	}
	// serializing field ItemList = 4
	if (_ItemList.IsSet())
	{
		if (_ItemList.GetValue().Num() == 0)
		{
			Schema_AddComponentUpdateClearedField(ComponentUpdate, 4);
		}
		else
		{
			for (const ::improbable::database_sync::DatabaseSyncItem& Element : (*_ItemList))
			{
				Element.Serialize(Schema_AddObject(UpdatesObject, 4));
			}
		}
	}
}

HydrationTest::Update HydrationTest::Update::Deserialize(Schema_ComponentUpdate * ComponentUpdate)
{
	Schema_Object* UpdatesObject = Schema_GetComponentUpdateFields(ComponentUpdate);
	
	uint32_t FieldCount = Schema_GetComponentUpdateClearedFieldCount(ComponentUpdate);
	Schema_FieldId* FieldsToClear = new Schema_FieldId[FieldCount];
	Schema_GetComponentUpdateClearedFieldList(ComponentUpdate, FieldsToClear);
	std::set<Schema_FieldId> FieldsToClearSet(FieldsToClear, FieldsToClear + FieldCount * sizeof(Schema_FieldId));
	
	HydrationTest::Update Data;
	
	// deserializing field ProfileId = 1
	if (Schema_GetBytesCount(UpdatesObject, 1) > 0)
	{
		Data._ProfileId = ::improbable::utils::GetString(UpdatesObject, 1);
	}
	
	// deserializing field ValueField1 = 2
	if (Schema_GetInt64Count(UpdatesObject, 2) > 0)
	{
		Data._ValueField1 = Schema_GetInt64(UpdatesObject, 2);
	}
	
	// deserializing field ValueField2 = 3
	if (Schema_GetInt64Count(UpdatesObject, 3) > 0)
	{
		Data._ValueField2 = Schema_GetInt64(UpdatesObject, 3);
	}
	
	// deserializing field ItemList = 4
	if (Schema_GetObjectCount(UpdatesObject, 4) > 0)
	{
		auto ListLength = Schema_GetObjectCount(UpdatesObject, 4);
		TArray<::improbable::database_sync::DatabaseSyncItem> ItemListList;
		ItemListList.SetNum(ListLength);
		for (uint32 i = 0; i < ListLength; ++i)
		{
			ItemListList[i] = ::improbable::database_sync::DatabaseSyncItem::Deserialize(Schema_IndexObject(UpdatesObject, 4, i));
		}
		Data._ItemList = ItemListList;
	}
	else if (FieldsToClearSet.count(4)) // only check if lists, maps, or options should be cleared
	{
		Data._ItemList = {};
	}
	
	return Data;
}

uint32 GetTypeHash(const improbable::database_sync::HydrationTest& Value)
{
	uint32 Result = 1327;
	Result = (Result * 977) + ::GetTypeHash(Value.GetProfileId());
	Result = (Result * 977) + ::GetTypeHash(Value.GetValueField1());
	Result = (Result * 977) + ::GetTypeHash(Value.GetValueField2());
	for (const auto& item : Value.GetItemList())
	{
		Result = (Result * 977) + improbable::database_sync::GetTypeHash(item);
	}
	return Result;
}

} // namespace database_sync
} // namespace improbable
