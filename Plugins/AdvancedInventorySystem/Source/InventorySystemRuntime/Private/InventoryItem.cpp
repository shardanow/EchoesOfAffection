// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryItem.h"
#include "Engine/World.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

UInventoryItem::UInventoryItem()
{
	ItemInstanceId = FInventoryItemId();
	ItemData = nullptr;
	Stack = FInventoryStack(1, 1);
	bIsEquipped = false;
	LastUsedTime = 0.0f;
}

UInventoryItem* UInventoryItem::CreateInventoryItem(UObject* WorldContextObject, UItemDataAsset* InItemData, int32 InQuantity)
{
	if (!InItemData || !WorldContextObject)
	{
		return nullptr;
	}

	UInventoryItem* NewItem = NewObject<UInventoryItem>(WorldContextObject);
	if (NewItem)
	{
		NewItem->ItemData = InItemData;
		NewItem->ItemInstanceId = FInventoryItemId();
		
		int32 MaxStack = InItemData->GetEffectiveMaxStackSize();
		NewItem->Stack = FInventoryStack(InQuantity, MaxStack);
		NewItem->bIsEquipped = false;
		NewItem->LastUsedTime = 0.0f;
	}

	return NewItem;
}

FText UInventoryItem::GetDisplayName() const
{
	return ItemData ? ItemData->ItemName : FText::FromString("Unknown Item");
}

FText UInventoryItem::GetDescription() const
{
	return ItemData ? ItemData->ItemDescription : FText::GetEmpty();
}

bool UInventoryItem::IsValid() const
{
	return ItemData != nullptr && ItemInstanceId.IsValid() && Stack.Quantity > 0;
}

bool UInventoryItem::CanStackWith(const UInventoryItem* OtherItem) const
{
	if (!OtherItem || !ItemData || !OtherItem->ItemData)
	{
		return false;
	}

	// Must be the same item type
	if (ItemData != OtherItem->ItemData)
	{
		return false;
	}

	// Must be stackable
	if (!ItemData->bIsStackable)
	{
		return false;
	}

	// Instance tags must match for stacking
	if (!InstanceTags.HasAllExact(OtherItem->InstanceTags) || 
	    !OtherItem->InstanceTags.HasAllExact(InstanceTags))
	{
		return false;
	}

	return true;
}

int32 UInventoryItem::GetMaxStackSize() const
{
	return ItemData ? ItemData->GetEffectiveMaxStackSize() : 1;
}

bool UInventoryItem::IsStackFull() const
{
	return Stack.IsFull();
}

int32 UInventoryItem::GetRemainingCapacity() const
{
	return Stack.GetRemainingCapacity();
}

bool UInventoryItem::CanBeUsed(AActor* User) const
{
	if (!ItemData || !ItemData->bIsUsable)
	{
		return false;
	}

	// Check if on cooldown
	if (User)
	{
		if (IsOnCooldown(User->GetWorld()))
		{
			return false;
		}
	}

	// Check requirements
	if (User && !ItemData->MeetsRequirements(User))
	{
		return false;
	}

	return true;
}

bool UInventoryItem::IsOnCooldown(const UWorld* World) const
{
	if (!ItemData || ItemData->UseCooldown <= 0.0f || !World)
	{
		return false;
	}

	float CurrentTime = World->GetTimeSeconds();
	float TimeSinceUse = CurrentTime - LastUsedTime;

	return TimeSinceUse < ItemData->UseCooldown;
}

float UInventoryItem::GetRemainingCooldown(const UWorld* World) const
{
	if (!IsOnCooldown(World))
	{
		return 0.0f;
	}

	float CurrentTime = World->GetTimeSeconds();
	float TimeSinceUse = CurrentTime - LastUsedTime;
	
	return FMath::Max(0.0f, ItemData->UseCooldown - TimeSinceUse);
}

int32 UInventoryItem::AddQuantity(int32 Amount)
{
	if (Amount <= 0)
	{
		return 0;
	}

	int32 RemainingCapacity = GetRemainingCapacity();
	int32 AmountToAdd = FMath::Min(Amount, RemainingCapacity);

	Stack.Quantity += AmountToAdd;

	return AmountToAdd;
}

int32 UInventoryItem::RemoveQuantity(int32 Amount)
{
	if (Amount <= 0)
	{
		return 0;
	}

	int32 AmountToRemove = FMath::Min(Amount, Stack.Quantity);
	Stack.Quantity -= AmountToRemove;

	return AmountToRemove;
}

void UInventoryItem::SetQuantity(int32 NewQuantity)
{
	Stack.Quantity = FMath::Max(0, NewQuantity);
	
	// Clamp to max stack size if applicable
	int32 MaxStack = GetMaxStackSize();
	if (MaxStack > 0 && Stack.Quantity > MaxStack)
	{
		Stack.Quantity = MaxStack;
	}
}

void UInventoryItem::SetEquipped(bool bEquipped)
{
	bIsEquipped = bEquipped;
}

void UInventoryItem::UpdateLastUsedTime(const UWorld* World)
{
	if (World)
	{
		LastUsedTime = World->GetTimeSeconds();
	}
}

void UInventoryItem::SetMetadata(FName Key, const FString& Value)
{
	CustomMetadata.Add(Key, Value);
}

bool UInventoryItem::GetMetadata(FName Key, FString& OutValue) const
{
	const FString* FoundValue = CustomMetadata.Find(Key);
	if (FoundValue)
	{
		OutValue = *FoundValue;
		return true;
	}
	return false;
}

bool UInventoryItem::RemoveMetadata(FName Key)
{
	return CustomMetadata.Remove(Key) > 0;
}

FString UInventoryItem::ToJson() const
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	// Basic data
	JsonObject->SetStringField(TEXT("ItemID"), ItemData ? ItemData->ItemID.ToString() : TEXT(""));
	JsonObject->SetStringField(TEXT("InstanceID"), ItemInstanceId.ToString());
	JsonObject->SetNumberField(TEXT("Quantity"), Stack.Quantity);
	JsonObject->SetBoolField(TEXT("IsEquipped"), bIsEquipped);
	JsonObject->SetNumberField(TEXT("LastUsedTime"), LastUsedTime);

	// Instance tags
	TArray<TSharedPtr<FJsonValue>> TagArray;
	for (const FGameplayTag& Tag : InstanceTags)
	{
		TagArray.Add(MakeShared<FJsonValueString>(Tag.ToString()));
	}
	JsonObject->SetArrayField(TEXT("InstanceTags"), TagArray);

	// Custom metadata
	TSharedPtr<FJsonObject> MetadataObject = MakeShared<FJsonObject>();
	for (const auto& Pair : CustomMetadata)
	{
		MetadataObject->SetStringField(Pair.Key.ToString(), Pair.Value);
	}
	JsonObject->SetObjectField(TEXT("Metadata"), MetadataObject);

	// Serialize to string
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}

bool UInventoryItem::FromJson(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return false;
	}

	// Load basic data
	FString ItemIDString;
	if (JsonObject->TryGetStringField(TEXT("ItemID"), ItemIDString))
	{
		// Would need to load ItemData from asset manager here
		// For now, we assume ItemData is already set
	}

	FString InstanceIDString;
	if (JsonObject->TryGetStringField(TEXT("InstanceID"), InstanceIDString))
	{
		FGuid ParsedGuid;
		if (FGuid::Parse(InstanceIDString, ParsedGuid))
		{
			ItemInstanceId = FInventoryItemId(ParsedGuid);
		}
	}

	JsonObject->TryGetNumberField(TEXT("Quantity"), Stack.Quantity);
	JsonObject->TryGetBoolField(TEXT("IsEquipped"), bIsEquipped);
	JsonObject->TryGetNumberField(TEXT("LastUsedTime"), LastUsedTime);

	// Load instance tags
	const TArray<TSharedPtr<FJsonValue>>* TagArray;
	if (JsonObject->TryGetArrayField(TEXT("InstanceTags"), TagArray))
	{
		InstanceTags.Reset();
		for (const TSharedPtr<FJsonValue>& TagValue : *TagArray)
		{
			FString TagString = TagValue->AsString();
			FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString));
			if (Tag.IsValid())
			{
				InstanceTags.AddTag(Tag);
			}
		}
	}

	// Load metadata
	const TSharedPtr<FJsonObject>* MetadataObject;
	if (JsonObject->TryGetObjectField(TEXT("Metadata"), MetadataObject))
	{
		CustomMetadata.Empty();
		for (const auto& Pair : (*MetadataObject)->Values)
		{
			FString Value;
			if (Pair.Value->TryGetString(Value))
			{
				CustomMetadata.Add(FName(*Pair.Key), Value);
			}
		}
	}

	return true;
}
