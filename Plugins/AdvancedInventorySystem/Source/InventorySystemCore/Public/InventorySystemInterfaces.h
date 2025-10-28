// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "InventorySystemTypes.h"
#include "InventorySystemInterfaces.generated.h"

class UItemDataAsset;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UInventoryHolder : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can hold inventory
 */
class INVENTORYSYSTEMCORE_API IInventoryHolder
{
	GENERATED_BODY()

public:
	/**
	 * Get the inventory component from this actor
	 * @return The inventory component, or nullptr if not found
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	UActorComponent* GetInventoryComponent();
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UNeedsSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for external Needs System integration
 */
class INVENTORYSYSTEMCORE_API INeedsSystemInterface
{
	GENERATED_BODY()

public:
	/**
	 * Modify a specific need value
	 * @param NeedTag The gameplay tag identifying the need
	 * @param DeltaValue The amount to change the need by
	 * @param bClamp Whether to clamp the value within valid range
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void ModifyNeed(const FGameplayTag& NeedTag, float DeltaValue, bool bClamp = true);

	/**
	 * Get current value of a specific need
	 * @param NeedTag The gameplay tag identifying the need
	 * @return The current need value
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	float GetNeedValue(const FGameplayTag& NeedTag) const;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UDialogueSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for external Dialogue System integration
 */
class INVENTORYSYSTEMCORE_API IDialogueSystemInterface
{
	GENERATED_BODY()

public:
	/**
	 * Trigger a dialogue event
	 * @param DialogueTag The gameplay tag identifying the dialogue
	 * @param OptionalContext Optional context object for the dialogue
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue")
	void TriggerDialogue(const FGameplayTag& DialogueTag, UObject* OptionalContext = nullptr);

	/**
	 * Check if a dialogue condition is met
	 * @param DialogueTag The gameplay tag identifying the dialogue condition
	 * @return True if the condition is met
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue")
	bool CheckDialogueCondition(const FGameplayTag& DialogueTag) const;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UTimeOfDaySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for external Time of Day System integration
 */
class INVENTORYSYSTEMCORE_API ITimeOfDaySystemInterface
{
	GENERATED_BODY()

public:
	/**
	 * Get the current time of day in hours (0-24)
	 * @return Current hour of the day
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time")
	float GetCurrentTimeOfDay() const;

	/**
	 * Get the current day number
	 * @return Current day index
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time")
	int32 GetCurrentDay() const;

	/**
	 * Check if current time is within a specific time range
	 * @param StartHour Start of the time range
	 * @param EndHour End of the time range
	 * @return True if current time is within range
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time")
	bool IsTimeInRange(float StartHour, float EndHour) const;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UItemEffectTargetSelector : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for custom item effect target selection
 */
class INVENTORYSYSTEMCORE_API IItemEffectTargetSelector
{
	GENERATED_BODY()

public:
	/**
	 * Select targets for an item effect
	 * @param ItemData The item data being used
	 * @param Instigator The actor using the item
	 * @param OutTargets Array to fill with selected targets
	 * @return True if target selection was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item Effects")
	bool SelectTargets(const UItemDataAsset* ItemData, AActor* Instigator, TArray<AActor*>& OutTargets);
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UShopPriceModifier : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for custom shop price modification
 */
class INVENTORYSYSTEMCORE_API IShopPriceModifier
{
	GENERATED_BODY()

public:
	/**
	 * Calculate the final price for an item
	 * @param BasePrice The base price of the item
	 * @param ItemData The item data asset
	 * @param Buyer The actor buying the item
	 * @param Quantity Number of items being purchased
	 * @return The modified price
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shop")
	int32 CalculatePrice(int32 BasePrice, const UItemDataAsset* ItemData, AActor* Buyer, int32 Quantity);

	/**
	 * Calculate the sellback price for an item
	 * @param OriginalPrice The original purchase price
	 * @param ItemData The item data asset
	 * @param Seller The actor selling the item
	 * @param Quantity Number of items being sold
	 * @return The sellback price
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shop")
	int32 CalculateSellbackPrice(int32 OriginalPrice, const UItemDataAsset* ItemData, AActor* Seller, int32 Quantity);
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UInventorySystemSaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for save/load support
 */
class INVENTORYSYSTEMCORE_API IInventorySystemSaveInterface
{
	GENERATED_BODY()

public:
	/**
	 * Save data to JSON string
	 * @param OutJsonString The resulting JSON string
	 * @return True if save was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
	bool SaveToJson(FString& OutJsonString);

	/**
	 * Load data from JSON string
	 * @param JsonString The JSON string to load from
	 * @return True if load was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
	bool LoadFromJson(const FString& JsonString);
};
