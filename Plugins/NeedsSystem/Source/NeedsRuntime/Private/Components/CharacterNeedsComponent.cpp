// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/CharacterNeedsComponent.h"
#include "Subsystems/NeedsSubsystem.h"
#include "Data/NeedsSettings.h"
#include "Engine/World.h"

UCharacterNeedsComponent::UCharacterNeedsComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // NO TICK! Batch updates via subsystem
	bWantsInitializeComponent = false;
	
	// Generate unique ID
	static int32 GlobalComponentIDCounter = 1;
	ComponentID = GlobalComponentIDCounter++;
}

void UCharacterNeedsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get subsystem
	CachedSubsystem = GetSubsystem();
	if (!CachedSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterNeedsComponent: Failed to get NeedsSubsystem!"));
		return;
	}

	// Initialize needs
	InitializeNeeds();

	// Register with subsystem for batch updates
	if (CachedSubsystem->Implements<UNeedsManager>())
	{
		INeedsManager::Execute_RegisterNeedsComponent(CachedSubsystem, this);
	}

	UE_LOG(LogTemp, Log, TEXT("CharacterNeedsComponent initialized on %s with %d needs"), 
		*GetOwner()->GetName(), Needs.Num());
}

void UCharacterNeedsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from subsystem
	if (CachedSubsystem && CachedSubsystem->Implements<UNeedsManager>())
	{
		INeedsManager::Execute_UnregisterNeedsComponent(CachedSubsystem, this);
	}

	Super::EndPlay(EndPlayReason);
}

// ========================================
// INITIALIZATION
// ========================================

void UCharacterNeedsComponent::InitializeNeeds()
{
	Needs.Empty();

	// Determine which needs to use
	TArray<UNeedDefinitionAsset*> NeedsToUse;
	
	if (OverrideNeeds.Num() > 0)
	{
		// Use override needs
		NeedsToUse = OverrideNeeds;
	}
	else if (CachedSubsystem)
	{
		// Use needs from settings
		UNeedsSettings* Settings = CachedSubsystem->GetSettings();
		if (Settings)
		{
			NeedsToUse = Settings->AvailableNeeds;
		}
	}

	// Initialize each need
	for (UNeedDefinitionAsset* NeedDef : NeedsToUse)
	{
		if (!NeedDef || !NeedDef->NeedTag.IsValid())
		{
			continue;
		}

		FNeedData& Data = Needs.Add(NeedDef->NeedTag);
		Data.Definition = NeedDef;
		Data.State.CurrentValue = NeedDef->InitialValue;
		Data.State.bIsSuppressed = NeedDef->bStartSuppressed;
		Data.State.bIsFrozen = NeedDef->bStartFrozen;
		Data.State.CurrentThreshold = NeedDef->ThresholdConfig.EvaluateThreshold(
			NeedDef->InitialValue, ENeedThresholdLevel::Normal);
		Data.State.PreviousThreshold = Data.State.CurrentThreshold;
	}

	// Initial mood calculation
	UpdateMood();
}

// ========================================
// INeedsAccessor INTERFACE
// ========================================

float UCharacterNeedsComponent::GetNeedValue_Implementation(const FGameplayTag& NeedTag) const
{
	const FNeedData* Data = GetNeedData(NeedTag);
	return Data ? Data->State.CurrentValue : 0.0f;
}

FNeedHandle UCharacterNeedsComponent::GetNeedHandle_Implementation(const FGameplayTag& NeedTag) const
{
	if (HasNeed_Implementation(NeedTag))
	{
		return FNeedHandle(NeedTag, ComponentID);
	}
	return FNeedHandle();
}

TArray<FNeedHandle> UCharacterNeedsComponent::GetAllNeedHandles_Implementation() const
{
	TArray<FNeedHandle> Handles;
	for (const auto& Pair : Needs)
	{
		Handles.Add(FNeedHandle(Pair.Key, ComponentID));
	}
	return Handles;
}

float UCharacterNeedsComponent::GetCurrentMood_Implementation() const
{
	return MoodState.MoodValue;
}

bool UCharacterNeedsComponent::HasNeed_Implementation(const FGameplayTag& NeedTag) const
{
	return Needs.Contains(NeedTag);
}

ENeedThresholdLevel UCharacterNeedsComponent::GetNeedThreshold_Implementation(const FGameplayTag& NeedTag) const
{
	const FNeedData* Data = GetNeedData(NeedTag);
	return Data ? Data->State.CurrentThreshold : ENeedThresholdLevel::Normal;
}

bool UCharacterNeedsComponent::IsNeedSuppressed_Implementation(const FGameplayTag& NeedTag) const
{
	const FNeedData* Data = GetNeedData(NeedTag);
	return Data ? Data->State.bIsSuppressed : false;
}

bool UCharacterNeedsComponent::IsNeedFrozen_Implementation(const FGameplayTag& NeedTag) const
{
	const FNeedData* Data = GetNeedData(NeedTag);
	return Data ? Data->State.bIsFrozen : false;
}

// ========================================
// INeedsMutator INTERFACE
// ========================================

void UCharacterNeedsComponent::SetNeedValue_Implementation(const FGameplayTag& NeedTag, float Value)
{
	FNeedData* Data = GetNeedDataMutable(NeedTag);
	if (!Data)
	{
		return;
	}

	const float OldValue = Data->State.CurrentValue;
	Data->State.CurrentValue = FMath::Clamp(Value, 0.0f, 100.0f);

	if (!FMath::IsNearlyEqual(OldValue, Data->State.CurrentValue))
	{
		CheckThreshold(NeedTag, *Data);
		NotifyNeedValueChanged(NeedTag, OldValue, Data->State.CurrentValue);
		UpdateMood();
	}
}

void UCharacterNeedsComponent::ModifyNeedValue_Implementation(const FGameplayTag& NeedTag, float Delta)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterNeedsComponent::ModifyNeedValue_Implementation called! Tag: %s, Delta: %f, Owner: %s"), 
		*NeedTag.ToString(), Delta, *GetOwner()->GetName());
	
	FNeedData* Data = GetNeedDataMutable(NeedTag);
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("  -> Need data NOT FOUND for tag: %s"), *NeedTag.ToString());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("  -> Current value: %f, New value will be: %f"), 
		Data->State.CurrentValue, Data->State.CurrentValue + Delta);

	SetNeedValue_Implementation(NeedTag, Data->State.CurrentValue + Delta);
}

void UCharacterNeedsComponent::ApplyEffect_Implementation(UNeedEffectAsset* Effect, int32 StackCount)
{
	if (!Effect || !Effect->EffectTag.IsValid())
	{
		return;
	}

	// Check if can apply
	if (!Effect->CanApply(GameplayTags))
	{
		return;
	}

	// Check for existing effect
	FActiveNeedEffect* ExistingEffect = nullptr;
	for (FActiveNeedEffect& ActiveEffect : ActiveEffects)
	{
		if (ActiveEffect.EffectAsset == Effect)
		{
			ExistingEffect = &ActiveEffect;
			break;
		}
	}

	if (ExistingEffect)
	{
		// Stack existing effect
		if (Effect->bCanStack)
		{
			ExistingEffect->StackCount = FMath::Min(ExistingEffect->StackCount + StackCount, Effect->MaxStacks);
			
			if (Effect->bStackAddsDuration && Effect->EffectType == ENeedEffectType::OverTime)
			{
				ExistingEffect->RemainingDuration += Effect->Duration * StackCount;
			}
		}
		else
		{
			// Refresh duration for non-stacking effects
			if (Effect->EffectType == ENeedEffectType::OverTime)
			{
				ExistingEffect->RemainingDuration = Effect->Duration;
			}
		}
	}
	else
	{
		// Create new effect
		FActiveNeedEffect NewEffect;
		NewEffect.EffectAsset = Effect;
		NewEffect.InstanceID = NextInstanceID++;
		NewEffect.StackCount = StackCount;
		NewEffect.RemainingDuration = Effect->Duration;
		
		// Get application time safely
		if (CachedSubsystem && CachedSubsystem->Implements<UNeedsManager>())
		{
			NewEffect.ApplicationTime = INeedsManager::Execute_GetCurrentGameTimeHours(CachedSubsystem);
		}
		else
		{
			NewEffect.ApplicationTime = 0.0;
		}

		if (Effect->EffectType == ENeedEffectType::Instant)
		{
			// Apply instant effect
			for (const FNeedEffectModification& Mod : Effect->Modifications)
			{
				if (!Mod.TargetNeedTag.IsValid())
				{
					continue;
				}

				const float Value = Mod.CalculateValue(1.0f);
				const float PowerMultiplier = CalculateModifiedEffectPower(Mod.TargetNeedTag, 1.0f);
				ModifyNeedValue_Implementation(Mod.TargetNeedTag, Value * PowerMultiplier * StackCount);
			}
		}
		else
		{
			// Add to active effects
			ActiveEffects.Add(NewEffect);
			NotifyEffectApplied(NewEffect);
		}

		UpdateGameplayTags();
	}
}

void UCharacterNeedsComponent::AddModifier_Implementation(UNeedModifierAsset* Modifier)
{
	if (!Modifier || !Modifier->ModifierTag.IsValid())
	{
		return;
	}

	// Check if already exists
	for (const FActiveNeedModifier& ActiveMod : ActiveModifiers)
	{
		if (ActiveMod.ModifierAsset == Modifier)
		{
			return; // Already added
		}
	}

	// Create new modifier
	FActiveNeedModifier NewModifier;
	NewModifier.ModifierAsset = Modifier;
	NewModifier.InstanceID = NextInstanceID++;
	NewModifier.Priority = Modifier->Priority;
	NewModifier.bIsActive = Modifier->ShouldBeActive(GameplayTags, GetCurrentHour());

	ActiveModifiers.Add(NewModifier);

	// Sort by priority
	ActiveModifiers.Sort([](const FActiveNeedModifier& A, const FActiveNeedModifier& B)
	{
		return A.Priority < B.Priority;
	});

	NotifyModifierAdded(NewModifier);
	UpdateGameplayTags();
}

void UCharacterNeedsComponent::RemoveModifier_Implementation(int32 InstanceID)
{
	for (int32 i = ActiveModifiers.Num() - 1; i >= 0; --i)
	{
		if (ActiveModifiers[i].InstanceID == InstanceID)
		{
			const FActiveNeedModifier RemovedModifier = ActiveModifiers[i];
			ActiveModifiers.RemoveAt(i);
			NotifyModifierRemoved(RemovedModifier);
			UpdateGameplayTags();
			break;
		}
	}
}

void UCharacterNeedsComponent::SuppressNeed_Implementation(const FGameplayTag& NeedTag, bool bSuppress)
{
	FNeedData* Data = GetNeedDataMutable(NeedTag);
	if (Data)
	{
		Data->State.bIsSuppressed = bSuppress;
	}
}

void UCharacterNeedsComponent::FreezeNeed_Implementation(const FGameplayTag& NeedTag, bool bFreeze)
{
	FNeedData* Data = GetNeedDataMutable(NeedTag);
	if (Data)
	{
		Data->State.bIsFrozen = bFreeze;
	}
}

void UCharacterNeedsComponent::RegisterListener_Implementation(const TScriptInterface<INeedsListener>& Listener)
{
	if (Listener.GetObject() && !RegisteredListeners.Contains(Listener))
	{
		RegisteredListeners.Add(Listener);
	}
}

void UCharacterNeedsComponent::UnregisterListener_Implementation(const TScriptInterface<INeedsListener>& Listener)
{
	RegisteredListeners.Remove(Listener);
}

// ========================================
// BLUEPRINT API
// ========================================

float UCharacterNeedsComponent::BP_GetNeedValue(const FGameplayTag& NeedTag) const
{
	return GetNeedValue_Implementation(NeedTag);
}

void UCharacterNeedsComponent::BP_SetNeedValue(const FGameplayTag& NeedTag, float Value)
{
	SetNeedValue_Implementation(NeedTag, Value);
}

void UCharacterNeedsComponent::BP_ModifyNeedValue(const FGameplayTag& NeedTag, float Delta)
{
	ModifyNeedValue_Implementation(NeedTag, Delta);
}

void UCharacterNeedsComponent::BP_ApplyEffect(UNeedEffectAsset* Effect, int32 StackCount)
{
	ApplyEffect_Implementation(Effect, StackCount);
}

void UCharacterNeedsComponent::BP_AddModifier(UNeedModifierAsset* Modifier)
{
	AddModifier_Implementation(Modifier);
}

float UCharacterNeedsComponent::BP_GetCurrentMood() const
{
	return GetCurrentMood_Implementation();
}

TArray<FGameplayTag> UCharacterNeedsComponent::GetAllNeedTags() const
{
	TArray<FGameplayTag> Tags;
	Needs.GetKeys(Tags);
	return Tags;
}

// ========================================
// UPDATE (Called by Subsystem)
// ========================================

void UCharacterNeedsComponent::UpdateNeeds(float DeltaGameHours, ENeedLODLevel LODLevel)
{
	CurrentLODLevel = LODLevel;

	// Update modifiers (check conditions)
	UpdateModifiers();

	// Update each need
	for (auto& Pair : Needs)
	{
		UpdateNeed(Pair.Key, Pair.Value, DeltaGameHours);
	}

	// Update active effects
	UpdateEffects(DeltaGameHours);

	// Update mood
	UpdateMood();
}

void UCharacterNeedsComponent::UpdateNeed(const FGameplayTag& NeedTag, FNeedData& NeedData, float DeltaGameHours)
{
	if (!NeedData.Definition || NeedData.State.bIsFrozen)
	{
		return;
	}

	const float OldValue = NeedData.State.CurrentValue;

	// Apply decay
	if (!NeedData.State.bIsSuppressed)
	{
		ApplyDecay(NeedData, DeltaGameHours);
	}

	// Check for changes
	if (!FMath::IsNearlyEqual(OldValue, NeedData.State.CurrentValue, 0.01f))
	{
		CheckThreshold(NeedTag, NeedData);
		NotifyNeedValueChanged(NeedTag, OldValue, NeedData.State.CurrentValue);
	}
}

void UCharacterNeedsComponent::ApplyDecay(FNeedData& NeedData, float DeltaGameHours)
{
	const float BaseDecayRate = NeedData.Definition->DecayConfig.BaseDecayRate;
	const float ModifiedDecayRate = CalculateModifiedDecayRate(NeedData.Definition->NeedTag, BaseDecayRate);
	
	const float DecayAmount = NeedData.Definition->DecayConfig.CalculateDecay(DeltaGameHours, NeedData.State.CurrentValue);
	const float FinalDecay = (DecayAmount / BaseDecayRate) * ModifiedDecayRate;

	NeedData.State.CurrentValue -= FinalDecay;
	NeedData.State.ClampValue();
}

void UCharacterNeedsComponent::UpdateMood()
{
	if (!CachedSubsystem)
	{
		return;
	}

	UNeedsSettings* Settings = CachedSubsystem->GetSettings();
	if (!Settings || !Settings->bEnableMood)
	{
		return;
	}

	// Build need values map
	TMap<FGameplayTag, float> NeedValues;
	for (const auto& Pair : Needs)
	{
		NeedValues.Add(Pair.Key, Pair.Value.State.CurrentValue);
	}

	const float OldMood = MoodState.MoodValue;
	MoodState.MoodValue = Settings->CalculateMood(NeedValues);
	MoodState.ClampMood();

	if (MoodState.HasChangedSignificantly(Settings->MoodChangeThreshold))
	{
		NotifyMoodChanged(OldMood, MoodState.MoodValue);
		MoodState.UpdatePrevious();
	}
}

void UCharacterNeedsComponent::UpdateEffects(float DeltaGameHours)
{
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		FActiveNeedEffect& Effect = ActiveEffects[i];
		
		if (!Effect.EffectAsset)
		{
			ActiveEffects.RemoveAt(i);
			continue;
		}

		if (Effect.EffectAsset->EffectType == ENeedEffectType::OverTime)
		{
			// Apply effect per tick
			const float Progress = 1.0f - (Effect.RemainingDuration / Effect.EffectAsset->Duration);
			
			for (const FNeedEffectModification& Mod : Effect.EffectAsset->Modifications)
			{
				if (!Mod.TargetNeedTag.IsValid())
				{
					continue;
				}

				const float ValuePerHour = Mod.CalculateValue(Progress) / Effect.EffectAsset->Duration;
				const float PowerMultiplier = CalculateModifiedEffectPower(Mod.TargetNeedTag, 1.0f);
				ModifyNeedValue_Implementation(Mod.TargetNeedTag, ValuePerHour * PowerMultiplier * Effect.StackCount * DeltaGameHours);
			}

			// Update duration
			Effect.RemainingDuration -= DeltaGameHours;

			// Remove if expired
			if (Effect.RemainingDuration <= 0.0f)
			{
				const FActiveNeedEffect ExpiredEffect = Effect;
				ActiveEffects.RemoveAt(i);
				NotifyEffectExpired(ExpiredEffect);
				UpdateGameplayTags();
			}
		}
	}
}

void UCharacterNeedsComponent::UpdateModifiers()
{
	const int32 CurrentHour = GetCurrentHour();
	
	for (FActiveNeedModifier& Modifier : ActiveModifiers)
	{
		if (!Modifier.ModifierAsset)
		{
			continue;
		}

		const bool bShouldBeActive = Modifier.ModifierAsset->ShouldBeActive(GameplayTags, CurrentHour);
		if (bShouldBeActive != Modifier.bIsActive)
		{
			Modifier.bIsActive = bShouldBeActive;
			UpdateGameplayTags();
		}
	}
}

void UCharacterNeedsComponent::UpdateGameplayTags()
{
	GameplayTags.Reset();

	// Add tags from active effects
	for (const FActiveNeedEffect& Effect : ActiveEffects)
	{
		if (Effect.EffectAsset)
		{
			GameplayTags.AppendTags(Effect.EffectAsset->GrantedTags);
		}
	}

	// Add tags from active modifiers
	for (const FActiveNeedModifier& Modifier : ActiveModifiers)
	{
		if (Modifier.ModifierAsset && Modifier.bIsActive)
		{
			GameplayTags.AppendTags(Modifier.ModifierAsset->GrantedTags);
		}
	}
}

float UCharacterNeedsComponent::CalculateModifiedDecayRate(const FGameplayTag& NeedTag, float BaseDecayRate) const
{
	float ModifiedRate = BaseDecayRate;

	for (const FActiveNeedModifier& Modifier : ActiveModifiers)
	{
		if (!Modifier.ModifierAsset || !Modifier.bIsActive)
		{
			continue;
		}

		const FNeedBehaviorModification* BehaviorMod = Modifier.ModifierAsset->FindModification(NeedTag);
		if (BehaviorMod)
		{
			ModifiedRate = BehaviorMod->ModifyDecayRate(ModifiedRate);
		}
	}

	return ModifiedRate;
}

float UCharacterNeedsComponent::CalculateModifiedEffectPower(const FGameplayTag& NeedTag, float BasePower) const
{
	float ModifiedPower = BasePower;

	for (const FActiveNeedModifier& Modifier : ActiveModifiers)
	{
		if (!Modifier.ModifierAsset || !Modifier.bIsActive)
		{
			continue;
		}

		const FNeedBehaviorModification* BehaviorMod = Modifier.ModifierAsset->FindModification(NeedTag);
		if (BehaviorMod)
		{
			ModifiedPower = BehaviorMod->ModifyEffectPower(ModifiedPower);
		}
	}

	return ModifiedPower;
}

void UCharacterNeedsComponent::CheckThreshold(const FGameplayTag& NeedTag, FNeedData& NeedData)
{
	if (!NeedData.Definition)
	{
		return;
	}

	const ENeedThresholdLevel NewThreshold = NeedData.Definition->ThresholdConfig.EvaluateThreshold(
		NeedData.State.CurrentValue, NeedData.State.CurrentThreshold);

	if (NewThreshold != NeedData.State.CurrentThreshold)
	{
		const ENeedThresholdLevel OldThreshold = NeedData.State.CurrentThreshold;
		NeedData.State.CurrentThreshold = NewThreshold;
		NeedData.State.UpdatePreviousThreshold();
		NotifyNeedThresholdChanged(NeedTag, OldThreshold, NewThreshold);
	}
}

// ========================================
// NOTIFICATIONS
// ========================================

void UCharacterNeedsComponent::NotifyNeedValueChanged(const FGameplayTag& NeedTag, float OldValue, float NewValue)
{
	OnNeedValueChanged.Broadcast(NeedTag, OldValue, NewValue);

	for (const TScriptInterface<INeedsListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			INeedsListener::Execute_OnNeedValueChanged(Listener.GetObject(), 
				FNeedHandle(NeedTag, ComponentID), OldValue, NewValue);
		}
	}
}

void UCharacterNeedsComponent::NotifyNeedThresholdChanged(const FGameplayTag& NeedTag, ENeedThresholdLevel OldLevel, ENeedThresholdLevel NewLevel)
{
	OnNeedThresholdChanged.Broadcast(NeedTag, OldLevel, NewLevel);

	for (const TScriptInterface<INeedsListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			INeedsListener::Execute_OnNeedThresholdChanged(Listener.GetObject(), 
				FNeedHandle(NeedTag, ComponentID), OldLevel, NewLevel);
		}
	}
}

void UCharacterNeedsComponent::NotifyMoodChanged(float OldMood, float NewMood)
{
	OnMoodChanged.Broadcast(OldMood, NewMood);

	for (const TScriptInterface<INeedsListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			INeedsListener::Execute_OnMoodChanged(Listener.GetObject(), OldMood, NewMood);
		}
	}
}

void UCharacterNeedsComponent::NotifyEffectApplied(const FActiveNeedEffect& Effect)
{
	OnEffectApplied.Broadcast(Effect);

	for (const TScriptInterface<INeedsListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			INeedsListener::Execute_OnEffectApplied(Listener.GetObject(), Effect);
		}
	}
}

void UCharacterNeedsComponent::NotifyEffectExpired(const FActiveNeedEffect& Effect)
{
	OnEffectExpired.Broadcast(Effect);

	for (const TScriptInterface<INeedsListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			INeedsListener::Execute_OnEffectExpired(Listener.GetObject(), Effect);
		}
	}
}

void UCharacterNeedsComponent::NotifyModifierAdded(const FActiveNeedModifier& Modifier)
{
	OnModifierAdded.Broadcast(Modifier);

	for (const TScriptInterface<INeedsListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			INeedsListener::Execute_OnModifierAdded(Listener.GetObject(), Modifier);
		}
	}
}

void UCharacterNeedsComponent::NotifyModifierRemoved(const FActiveNeedModifier& Modifier)
{
	OnModifierRemoved.Broadcast(Modifier);

	for (const TScriptInterface<INeedsListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			INeedsListener::Execute_OnModifierRemoved(Listener.GetObject(), Modifier);
		}
	}
}

// ========================================
// HELPERS
// ========================================

const FNeedData* UCharacterNeedsComponent::GetNeedData(const FGameplayTag& NeedTag) const
{
	return Needs.Find(NeedTag);
}

FNeedData* UCharacterNeedsComponent::GetNeedDataMutable(const FGameplayTag& NeedTag)
{
	return Needs.Find(NeedTag);
}

const FNeedLODConfig& UCharacterNeedsComponent::GetLODConfig() const
{
	if (bUseCustomLOD)
	{
		return CustomLODConfig;
	}

	if (CachedSubsystem)
	{
		UNeedsSettings* Settings = CachedSubsystem->GetSettings();
		if (Settings)
		{
			return Settings->LODConfig;
		}
	}

	static FNeedLODConfig DefaultConfig;
	return DefaultConfig;
}

UNeedsSubsystem* UCharacterNeedsComponent::GetSubsystem() const
{
	if (CachedSubsystem)
	{
		return CachedSubsystem;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	return World->GetSubsystem<UNeedsSubsystem>();
}

int32 UCharacterNeedsComponent::GetCurrentHour() const
{
	if (CachedSubsystem && CachedSubsystem->Implements<UNeedsManager>())
	{
		const float GameTimeHours = INeedsManager::Execute_GetCurrentGameTimeHours(CachedSubsystem);
		return static_cast<int32>(GameTimeHours) % 24;
	}
	return 12; // Default noon
}
