// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventorySystemTypes.h"
#include "WorldItemActor.generated.h"

class UItemDataAsset;
class UInventoryItem;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USphereComponent;
class UActorComponent; // For dynamic component creation

/**
 * Actor representing a physical item in the world
 * Can be picked up and added to inventory
 * 
 * AUTOMATIC INTERACTION INTEGRATION (v1.7.1):
 * 
 * This actor now automatically creates an InteractableComponent if:
 * 1. bAutoCreateInteractionComponent is true (default)
 * 2. Your project has UInteractableComponent class
 * 3. No InteractableComponent already exists on this actor
 * 
 * This means items work with your interaction system OUT OF THE BOX!
 * No Blueprint setup needed, no manual component adding required.
 * 
 * Configuration:
 * - Set bAutoCreateInteractionComponent = false to disable auto-creation
 * - Override ConfigureInteractionComponent() in Blueprint for custom setup
 * 
 * Manual Integration (if you prefer):
 * - Create Blueprint child class
 * - Add InteractableComponent manually
 * - Or implement IInteractableInterface in Blueprint
 * 
 * The component is created in BeginPlay, so it works for:
 * - Items placed in editor
 * - Items spawned at runtime (including dropped from inventory)
 * - Blueprint children with custom logic
 */
UCLASS()
class INVENTORYSYSTEMRUNTIME_API AWorldItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldItemActor();

protected:
	virtual void BeginPlay() override;

public:
	// ========== COMPONENTS ==========

	/** Root scene component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	/** Static mesh component for physical representation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	/** Skeletal mesh component for physical representation (alternative) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	/** Sphere component for interaction detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	// ========== ITEM DATA ==========

	/** Reference to the item data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ExposeOnSpawn = true))
	TObjectPtr<UItemDataAsset> ItemData;

	/** Quantity of items in this world item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ExposeOnSpawn = true, ClampMin = "1"))
	int32 Quantity = 1;

	/** Custom instance tags for this specific item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FGameplayTagContainer InstanceTags;

	/** Custom metadata for this item instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TMap<FName, FString> CustomMetadata;

	// ========== CONFIGURATION ==========

	/** Interaction radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Configuration")
	float InteractionRadius = 150.0f;

	/** Whether this item can be picked up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Configuration")
	bool bCanBePickedUp = true;

	/** Whether to destroy this actor after being picked up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Configuration")
	bool bDestroyOnPickup = true;

	/** Custom pickup text override */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Configuration")
	FText PickupTextOverride;

	// ========== AUTO INTERACTION INTEGRATION (NEW v1.7.1) ==========-
	
	/**
	 * Automatically create InteractableComponent at BeginPlay
	 * This makes items work with your interaction system out of the box!
	 * Set to false if you want to add the component manually or use Blueprint interface
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Interaction")
	bool bAutoCreateInteractionComponent = true;

	/**
	 * Class name of the InteractableComponent to create
	 * Default: "InteractableComponent" (searches in your project)
	 * Change if your component has different name
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Interaction")
	FString InteractableComponentClassName = TEXT("InteractableComponent");

	// ========== INTERACTION INTERFACE SUPPORT ==========-

	/**
	 * Whether to auto-implement interaction interface behavior
	 * When true and IInteractableInterface is detected, provides default implementation
	 * Override GetInteractionData_Native and BeginInteract_Native for custom behavior
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Interaction")
	bool bAutoImplementInteraction = true;

	/**
	 * Override for interaction name (used if interaction interface is present)
	 * If empty, uses ItemData name
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Interaction")
	FText InteractionNameOverride;

	/**
	 * Interaction duration in seconds (0 = instant pickup)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Interaction", meta = (ClampMin = "0.0"))
	float InteractionDuration = 0.0f;

	/**
	 * Interaction priority (higher values take precedence)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Interaction")
	int32 InteractionPriority = 0;

	// ========== INITIALIZATION ==========

	/**
	 * Initialize this world item with item data
	 * @param InItemData The item data to initialize with
	 * @param InQuantity The quantity of items
	 */
	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitializeItem(UItemDataAsset* InItemData, int32 InQuantity = 1);

	/**
	 * Initialize from an inventory item
	 * @param InventoryItem The inventory item to initialize from
	 */
	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitializeFromInventoryItem(UInventoryItem* InventoryItem);

	// ========== INTERACTION ==========

	/**
	 * Called when an actor attempts to pick up this item
	 * This is called from the InteractableComponent or directly
	 * @param Picker The actor attempting to pick up the item
	 * @return True if the item was successfully picked up
	 */
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool PickupItem(AActor* Picker);

	/**
	 * Get the pickup interaction text
	 * @return The text to display for pickup interaction
	 */
	UFUNCTION(BlueprintPure, Category = "Item")
	FText GetPickupText() const;

	/**
	 * Check if this item can be picked up by an actor
	 * @param Picker The actor attempting to pick up
	 * @return True if the item can be picked up
	 */
	UFUNCTION(BlueprintPure, Category = "Item")
	bool CanBePickedUpBy(AActor* Picker) const;

	// ========== INTERACTION INTERFACE HELPERS ==========

	/**
	 * Get interaction name for interface implementations
	 * Override this in Blueprint or C++ for custom behavior
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item|Interaction")
	FText GetInteractionName() const;

	/**
	 * Get interaction action text for interface implementations
	 * Override this in Blueprint or C++ for custom behavior
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item|Interaction")
	FText GetInteractionActionText() const;

	/**
	 * Check if interaction interface is implemented on this actor
	 * Useful for conditional logic
	 */
	UFUNCTION(BlueprintPure, Category = "Item|Interaction")
	bool HasInteractionInterface() const;

	/**
	 * Get the dynamically created InteractableComponent (if any)
	 * Returns null if auto-creation is disabled or component doesn't exist
	 */
	UFUNCTION(BlueprintPure, Category = "Item|Interaction")
	UActorComponent* GetInteractableComponent() const { return DynamicInteractableComponent; }

	// ========== COMPONENT CONFIGURATION (OVERRIDE IN BLUEPRINT) ==========

	/**
	 * Called after InteractableComponent is created
	 * Override in Blueprint to configure the component with custom data
	 * Example: Set interaction text, configure callbacks, etc.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Interaction")
	void ConfigureInteractionComponent(UActorComponent* InteractableComp);

	/**
	 * Called when InteractableComponent's OnBeginInteract fires
	 * This is bound automatically during ConfigureInteractionComponent
	 * Calls PickupItem() to handle the actual pickup
	 */
	UFUNCTION()
	void HandleInteractionBegin(AActor* InteractingActor);

	// ========== EVENTS ==========

	/**
	 * Event called when item is successfully picked up
	 * @param Picker The actor that picked up the item
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Events")
	void OnItemPickedUp(AActor* Picker);

	/**
	 * Event called when pickup attempt fails
	 * @param Picker The actor that attempted to pick up
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Events")
	void OnPickupFailed(AActor* Picker);

	/**
	 * Event called when interaction focus is gained
	 * @param InteractingActor The actor focusing on this item
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Events")
	void OnInteractionFocusGained(AActor* InteractingActor);

	/**
	 * Event called when interaction focus is lost
	 * @param InteractingActor The actor that was focusing on this item
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Events")
	void OnInteractionFocusLost(AActor* InteractingActor);

protected:
	/** Setup the physical representation based on item data */
	void SetupPhysicalRepresentation();

	/** 
	 * Create InteractableComponent dynamically if conditions are met
	 * Called in BeginPlay
	 */
	void TryCreateInteractableComponent();

	/** Called when an actor enters the interaction sphere */
	UFUNCTION()
	void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when an actor leaves the interaction sphere */
	UFUNCTION()
	void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	/** Dynamically created InteractableComponent (if auto-creation is enabled) */
	UPROPERTY()
	TObjectPtr<UActorComponent> DynamicInteractableComponent;

	/** Track if we've already bound the OnBeginInteract delegate */
	bool bHasBoundInteractionDelegate = false;
};
