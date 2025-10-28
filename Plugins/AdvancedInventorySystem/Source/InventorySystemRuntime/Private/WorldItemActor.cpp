// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldItemActor.h"
#include "ItemDataAsset.h"
#include "InventoryItem.h"
#include "InventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"

AWorldItemActor::AWorldItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComponent;

	// Create static mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	StaticMeshComponent->SetSimulatePhysics(false);

	// Create skeletal mesh component
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SkeletalMeshComponent->SetVisibility(false);

	// Create interaction sphere
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(InteractionRadius);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	InteractionSphere->SetGenerateOverlapEvents(true);

	// Default values
	Quantity = 1;
	InteractionRadius = 150.0f;
	bCanBePickedUp = true;
	bDestroyOnPickup = true;
	bAutoImplementInteraction = true;
	bAutoCreateInteractionComponent = true;
	InteractionDuration = 0.0f;
	InteractionPriority = 0;
	InteractableComponentClassName = TEXT("InteractableComponent");

	// Set this actor to be replicated
	bReplicates = true;
}

void AWorldItemActor::BeginPlay()
{
	Super::BeginPlay();

	// Setup physical representation
	SetupPhysicalRepresentation();

	// Bind overlap events
	if (InteractionSphere)
	{
		InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWorldItemActor::OnInteractionSphereBeginOverlap);
		InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AWorldItemActor::OnInteractionSphereEndOverlap);
		InteractionSphere->SetSphereRadius(InteractionRadius);
	}

	// Try to create InteractableComponent dynamically
	TryCreateInteractableComponent();
}

void AWorldItemActor::TryCreateInteractableComponent()
{
	// Skip if auto-creation is disabled
	if (!bAutoCreateInteractionComponent)
	{
		return;
	}

	// Skip if component already exists (manually added in Blueprint/Editor)
	if (FindComponentByClass(UActorComponent::StaticClass()))
	{
		// Check if any component implements the interactable interface
		TArray<UActorComponent*> Components;
		GetComponents(Components);
		
		for (UActorComponent* Comp : Components)
		{
			if (Comp && Comp->GetClass()->GetName().Contains(TEXT("Interactable")))
			{
				UE_LOG(LogTemp, Log, TEXT("WorldItemActor: InteractableComponent already exists on %s, skipping auto-creation"), *GetName());
				return;
			}
		}
	}

	// Try to find InteractableComponent class by name
	UClass* InteractableClass = FindObject<UClass>(nullptr, *FString::Printf(TEXT("/Script/EchoesOfAffection.%s"), *InteractableComponentClassName));
	
	if (!InteractableClass)
	{
		// Try alternative search patterns
		FString SearchPaths[] = {
			FString::Printf(TEXT("Class'/Script/EchoesOfAffection.%s'"), *InteractableComponentClassName),
			InteractableComponentClassName
		};

		for (const FString& SearchPath : SearchPaths)
		{
			InteractableClass = FindObject<UClass>(nullptr, *SearchPath);
			if (InteractableClass)
			{
				break;
			}
		}
	}

	if (!InteractableClass)
	{
		// Component class not found - this is OK, means project doesn't have it
		// Don't spam log, silently skip
		return;
	}

	// Verify it's actually a component class
	if (!InteractableClass->IsChildOf(UActorComponent::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldItemActor: Found class %s but it's not an ActorComponent"), *InteractableComponentClassName);
		return;
	}

	// Create the component dynamically
	DynamicInteractableComponent = NewObject<UActorComponent>(
		this, 
		InteractableClass,
		FName(*FString::Printf(TEXT("%s_Dynamic"), *InteractableComponentClassName))
	);

	if (DynamicInteractableComponent)
	{
		// Register the component
		DynamicInteractableComponent->RegisterComponent();

		// Configure the component with item-specific data
		ConfigureInteractionComponent(DynamicInteractableComponent);

		UE_LOG(LogTemp, Log, TEXT("WorldItemActor: Auto-created %s for %s"), *InteractableComponentClassName, *GetName());
	}
}

void AWorldItemActor::ConfigureInteractionComponent_Implementation(UActorComponent* InteractableComp)
{
	if (!InteractableComp)
	{
		return;
	}

	UClass* CompClass = InteractableComp->GetClass();

	// Set interaction data via property reflection
	FProperty* DataProp = CompClass->FindPropertyByName(TEXT("InteractionData"));
	if (DataProp && DataProp->IsA<FStructProperty>())
	{
		FStructProperty* StructProp = static_cast<FStructProperty*>(DataProp);
		void* DataPtr = StructProp->ContainerPtrToValuePtr<void>(InteractableComp);
		
		if (DataPtr)
		{
			UScriptStruct* Struct = StructProp->Struct;
			
			// Set interaction name
			if (FProperty* NameProp = Struct->FindPropertyByName(TEXT("InteractionName")))
			{
				if (FTextProperty* TextProp = CastField<FTextProperty>(NameProp))
				{
					FText* Value = TextProp->ContainerPtrToValuePtr<FText>(DataPtr);
					if (Value) *Value = GetInteractionName();
				}
			}
			
			// Set action text
			if (FProperty* ActionProp = Struct->FindPropertyByName(TEXT("ActionText")))
			{
				if (FTextProperty* TextProp = CastField<FTextProperty>(ActionProp))
				{
					FText* Value = TextProp->ContainerPtrToValuePtr<FText>(DataPtr);
					if (Value) *Value = GetInteractionActionText();
				}
			}
			
			// Set duration
			if (FProperty* DurationProp = Struct->FindPropertyByName(TEXT("InteractionDuration")))
			{
				if (FFloatProperty* FloatProp = CastField<FFloatProperty>(DurationProp))
				{
					float* Value = FloatProp->ContainerPtrToValuePtr<float>(DataPtr);
					if (Value) *Value = InteractionDuration;
				}
			}
			
			// Set distance
			if (FProperty* DistProp = Struct->FindPropertyByName(TEXT("MaxInteractionDistance")))
			{
				if (FFloatProperty* FloatProp = CastField<FFloatProperty>(DistProp))
				{
					float* Value = FloatProp->ContainerPtrToValuePtr<float>(DataPtr);
					if (Value) *Value = InteractionRadius;
				}
			}
			
			// Set priority
			if (FProperty* PriorityProp = Struct->FindPropertyByName(TEXT("InteractionPriority")))
			{
				if (FIntProperty* IntProp = CastField<FIntProperty>(PriorityProp))
				{
					int32* Value = IntProp->ContainerPtrToValuePtr<int32>(DataPtr);
					if (Value) *Value = InteractionPriority;
				}
			}
			
			// Set can interact multiple times = false
			if (FProperty* MultipleProp = Struct->FindPropertyByName(TEXT("bCanInteractMultipleTimes")))
			{
				if (FBoolProperty* BoolProp = CastField<FBoolProperty>(MultipleProp))
				{
					void* Value = BoolProp->ContainerPtrToValuePtr<void>(DataPtr);
					if (Value) BoolProp->SetPropertyValue(Value, false);
				}
			}
			
			UE_LOG(LogTemp, Log, TEXT("WorldItemActor: Configured InteractionData for %s"), *GetName());
		}
	}

	// Bind to OnBeginInteract delegate using manual pointer calculation
	// IMPORTANT: Only bind once to avoid duplicate binding!
	if (FProperty* DelegateProp = CompClass->FindPropertyByName(TEXT("OnBeginInteract")))
	{
		// Check if we've already bound this delegate
		if (bHasBoundInteractionDelegate)
		{
			UE_LOG(LogTemp, Log, TEXT("WorldItemActor: OnBeginInteract delegate already bound for %s, skipping"), *GetName());
			return; // Already bound, don't bind again
		}

		// Calculate pointer to delegate using offset
		uint8* ComponentAddress = reinterpret_cast<uint8*>(InteractableComp);
		uint8* DelegateAddress = ComponentAddress + DelegateProp->GetOffset_ForInternal();
		FMulticastScriptDelegate* DelegateInstance = reinterpret_cast<FMulticastScriptDelegate*>(DelegateAddress);
		
		if (DelegateInstance)
		{
			FScriptDelegate Callback;
			Callback.BindUFunction(this, FName("HandleInteractionBegin"));
			DelegateInstance->Add(Callback);
			
			// Mark as bound
			bHasBoundInteractionDelegate = true;
			
			UE_LOG(LogTemp, Log, TEXT("WorldItemActor: Bound OnBeginInteract delegate for %s"), *GetName());
		}
	}
}

// This function will be called by the InteractableComponent's OnBeginInteract delegate
void AWorldItemActor::HandleInteractionBegin(AActor* InteractingActor)
{
	// Call PickupItem - this is where the actual pickup logic happens
	PickupItem(InteractingActor);
}

void AWorldItemActor::InitializeItem(UItemDataAsset* InItemData, int32 InQuantity)
{
	ItemData = InItemData;
	Quantity = FMath::Max(1, InQuantity);

	SetupPhysicalRepresentation();
	
	// Reconfigure interaction component if it exists
	if (DynamicInteractableComponent)
	{
		ConfigureInteractionComponent(DynamicInteractableComponent);
	}
}

void AWorldItemActor::InitializeFromInventoryItem(UInventoryItem* InventoryItem)
{
	if (!InventoryItem)
	{
		return;
	}

	ItemData = InventoryItem->ItemData;
	Quantity = InventoryItem->GetQuantity();
	InstanceTags = InventoryItem->InstanceTags;
	CustomMetadata = InventoryItem->CustomMetadata;

	SetupPhysicalRepresentation();
	
	// Reconfigure interaction component if it exists
	if (DynamicInteractableComponent)
	{
		ConfigureInteractionComponent(DynamicInteractableComponent);
	}
}

bool AWorldItemActor::PickupItem(AActor* Picker)
{
	UE_LOG(LogTemp, Log, TEXT("WorldItemActor::PickupItem called by %s"), Picker ? *Picker->GetName() : TEXT("NULL"));
	
	if (!CanBePickedUpBy(Picker))
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldItemActor::PickupItem FAILED: CanBePickedUpBy returned false"));
		OnPickupFailed(Picker);
		return false;
	}

	// Try to find inventory component on picker
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(
		Picker->GetComponentByClass(UInventoryComponent::StaticClass())
	);

	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldItemActor::PickupItem FAILED: Picker has no InventoryComponent"));
		OnPickupFailed(Picker);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("WorldItemActor::PickupItem: Found InventoryComponent, attempting to add item..."));

	// Try to add item to inventory
	EInventoryOperationResult Result;
	UInventoryItem* AddedItem = InventoryComp->AddItem(ItemData, Quantity, Result);

	if (Result == EInventoryOperationResult::Success && AddedItem)
	{
		UE_LOG(LogTemp, Log, TEXT("WorldItemActor::PickupItem SUCCESS: Item added to inventory"));
		
		// Transfer instance data
		AddedItem->InstanceTags = InstanceTags;
		AddedItem->CustomMetadata = CustomMetadata;

		// Notify pickup
		OnItemPickedUp(Picker);

		// Destroy or hide this actor
		if (bDestroyOnPickup)
		{
			Destroy();
		}
		else
		{
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
		}

		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("WorldItemActor::PickupItem FAILED: AddItem result = %d"), (int32)Result);
	OnPickupFailed(Picker);
	return false;
}

FText AWorldItemActor::GetPickupText() const
{
	// 1. Use override if explicitly set
	if (!PickupTextOverride.IsEmpty())
	{
		return PickupTextOverride;
	}

	// 2. Use ItemData's pickup text if set
	if (ItemData && !ItemData->PhysicalRepresentation.PickupInteractionText.IsEmpty())
	{
		// Check if it's the default "Pick up" text
		FString DefaultPickupText = TEXT("Pick up");
		FString DataPickupText = ItemData->PhysicalRepresentation.PickupInteractionText.ToString();
		
		// If it's default text, append item name automatically
		if (DataPickupText.Equals(DefaultPickupText, ESearchCase::IgnoreCase))
		{
			return FText::Format(
				FText::FromString("{0} {1}"),
				ItemData->PhysicalRepresentation.PickupInteractionText,
				ItemData->ItemName
			);
		}
		
		// If custom text, check if it already contains item name
		if (DataPickupText.Contains(ItemData->ItemName.ToString()))
		{
			// Already contains item name, use as is
			return ItemData->PhysicalRepresentation.PickupInteractionText;
		}
		
		// Custom text without item name, append it
		return FText::Format(
			FText::FromString("{0} {1}"),
			ItemData->PhysicalRepresentation.PickupInteractionText,
			ItemData->ItemName
		);
	}

	// 3. Fallback with item name if available
	if (ItemData)
	{
		return FText::Format(
			FText::FromString("Pick up {0}"),
			ItemData->ItemName
		);
	}

	// 4. Final fallback
	return FText::FromString("Pick up");
}

bool AWorldItemActor::CanBePickedUpBy(AActor* Picker) const
{
	if (!bCanBePickedUp)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldItemActor::CanBePickedUpBy: bCanBePickedUp = false"));
		return false;
	}
	
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldItemActor::CanBePickedUpBy: No ItemData assigned!"));
		return false;
	}
	
	if (!Picker)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldItemActor::CanBePickedUpBy: Picker is NULL"));
		return false;
	}

	if (!ItemData->PhysicalRepresentation.bCanBePickedUp)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldItemActor::CanBePickedUpBy: ItemData.PhysicalRepresentation.bCanBePickedUp = false"));
		return false;
	}

	// Check if picker has inventory component
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(
		Picker->GetComponentByClass(UInventoryComponent::StaticClass())
	);

	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldItemActor::CanBePickedUpBy: Picker has no InventoryComponent"));
		return false;
	}

	// Check if inventory can accept this item
	bool bCanAdd = InventoryComp->CanAddItem(ItemData, Quantity);
	
	if (!bCanAdd)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldItemActor::CanBePickedUpBy: Inventory cannot accept item (full or incompatible)"));
	}
	
	return bCanAdd;
}

FText AWorldItemActor::GetInteractionName_Implementation() const
{
	// Use override if set
	if (!InteractionNameOverride.IsEmpty())
	{
		return InteractionNameOverride;
	}

	// Use item name if available
	if (ItemData)
	{
		return ItemData->ItemName;
	}

	// Fallback
	return FText::FromString("Item");
}

FText AWorldItemActor::GetInteractionActionText_Implementation() const
{
	// Use pickup text logic with item name
	FText PickupText = GetPickupText();
	
	// If we have ItemData, format with item name
	if (ItemData)
	{
		// If pickup text already contains the item name, return as is
		if (PickupText.ToString().Contains(ItemData->ItemName.ToString()))
		{
			return PickupText;
		}
		
		// Otherwise, append item name
		return FText::Format(
			FText::FromString("{0} {1}"),
			PickupText,
			ItemData->ItemName
		);
	}
	
	return PickupText;
}

bool AWorldItemActor::HasInteractionInterface() const
{
	// Check if this actor implements IInteractableInterface
	// This works without hard dependency - just checks if interface is present
	static const FName InteractableInterfaceName = TEXT("InteractableInterface");
	
	for (const FImplementedInterface& Interface : GetClass()->Interfaces)
	{
		if (Interface.Class && Interface.Class->GetFName() == InteractableInterfaceName)
		{
			return true;
		}
	}

	// Also check if we have an InteractableComponent
	if (DynamicInteractableComponent)
	{
		return true;
	}

	// Check for manually added component
	TArray<UActorComponent*> Components;
	GetComponents(Components);
	
	for (UActorComponent* Comp : Components)
	{
		if (Comp && Comp->GetClass()->GetName().Contains(TEXT("Interactable")))
		{
			return true;
		}
	}

	return false;
}

void AWorldItemActor::SetupPhysicalRepresentation()
{
	if (!ItemData)
	{
		// If no ItemData but mesh is set manually in editor, keep it
		if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh())
		{
			StaticMeshComponent->SetVisibility(true);
		}
		return;
	}

	const FItemPhysicalRepresentation& PhysRep = ItemData->PhysicalRepresentation;

	// Update interaction settings from ItemDataAsset
	InteractionRadius = PhysRep.InteractionRadius;
	InteractionDuration = PhysRep.InteractionDuration;
	InteractionPriority = PhysRep.InteractionPriority;
	
	// Update interaction sphere radius
	if (InteractionSphere)
	{
		InteractionSphere->SetSphereRadius(InteractionRadius);
	}

	// If no mesh in PhysRep but StaticMeshComponent already has a mesh (set in editor), keep it
	if (!PhysRep.StaticMesh && !PhysRep.SkeletalMesh)
	{
		if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh())
		{
			// Keep the mesh set in editor
			StaticMeshComponent->SetVisibility(true);
			return;
		}
	}

	// Hide both mesh components initially
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetVisibility(false);
	}
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetVisibility(false);
	}

	// Setup static mesh if available
	if (PhysRep.StaticMesh && StaticMeshComponent)
	{
		StaticMeshComponent->SetStaticMesh(PhysRep.StaticMesh);
		StaticMeshComponent->SetRelativeLocation(PhysRep.LocationOffset);
		StaticMeshComponent->SetRelativeRotation(PhysRep.RotationOffset);
		StaticMeshComponent->SetRelativeScale3D(PhysRep.Scale);
		StaticMeshComponent->SetVisibility(true);
	}
	// Setup skeletal mesh if static mesh is not available
	else if (PhysRep.SkeletalMesh && SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetSkeletalMesh(PhysRep.SkeletalMesh);
		SkeletalMeshComponent->SetRelativeLocation(PhysRep.LocationOffset);
		SkeletalMeshComponent->SetRelativeRotation(PhysRep.RotationOffset);
		SkeletalMeshComponent->SetRelativeScale3D(PhysRep.Scale);
		SkeletalMeshComponent->SetVisibility(true);
	}

	// Update pickup ability
	bCanBePickedUp = PhysRep.bCanBePickedUp;
}

void AWorldItemActor::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Notify focus gained
	OnInteractionFocusGained(OtherActor);
}

void AWorldItemActor::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Notify focus lost
	OnInteractionFocusLost(OtherActor);
}
