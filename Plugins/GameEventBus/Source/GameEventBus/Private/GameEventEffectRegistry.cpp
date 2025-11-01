// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameEventEffectRegistry.h"
#include "GameEventBusSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogEffectRegistry, Log, All);

void UGameEventEffectRegistry::RegisterEffect(const FGameplayTag& EffectTag, TSubclassOf<UObject> EffectClass, FName SourceSystem)
{
	if (!EffectTag.IsValid())
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("RegisterEffect: Invalid EffectTag!"));
		return;
	}

	if (!EffectClass)
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("RegisterEffect: Invalid EffectClass for tag '%s'"), *EffectTag.ToString());
		return;
	}

	// Check if class implements interface
	if (!EffectClass->ImplementsInterface(UGenericGameEffect::StaticClass()))
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("RegisterEffect: Class '%s' does not implement IGenericGameEffect interface"), 
			*EffectClass->GetName());
		return;
	}

	FEffectRegistration Registration;
	Registration.EffectTag = EffectTag;
	Registration.EffectClass = EffectClass;
	Registration.SourceSystem = SourceSystem;

	RegisteredEffects.Add(EffectTag, Registration);

	UE_LOG(LogEffectRegistry, Log, TEXT("Registered effect '%s' from system '%s'"), 
		*EffectTag.ToString(), 
		*SourceSystem.ToString());
}

void UGameEventEffectRegistry::RegisterCondition(const FGameplayTag& ConditionTag, TSubclassOf<UObject> EvaluatorClass, FName SourceSystem)
{
	if (!ConditionTag.IsValid())
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("RegisterCondition: Invalid ConditionTag!"));
		return;
	}

	if (!EvaluatorClass)
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("RegisterCondition: Invalid EvaluatorClass for tag '%s'"), *ConditionTag.ToString());
		return;
	}

	// Check if class implements interface
	if (!EvaluatorClass->ImplementsInterface(UGenericConditionEvaluator::StaticClass()))
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("RegisterCondition: Class '%s' does not implement IGenericConditionEvaluator interface"), 
			*EvaluatorClass->GetName());
		return;
	}

	FConditionRegistration Registration;
	Registration.ConditionTag = ConditionTag;
	Registration.EvaluatorClass = EvaluatorClass;
	Registration.SourceSystem = SourceSystem;

	RegisteredConditions.Add(ConditionTag, Registration);

	UE_LOG(LogEffectRegistry, Log, TEXT("Registered condition '%s' from system '%s'"), 
		*ConditionTag.ToString(), 
		*SourceSystem.ToString());
}

bool UGameEventEffectRegistry::ExecuteEffect(const FGameplayTag& EffectTag, UObject* WorldContext, const FGameEventPayload& Payload)
{
	FEffectRegistration* Registration = RegisteredEffects.Find(EffectTag);
	if (!Registration)
	{
		UE_LOG(LogEffectRegistry, Warning, TEXT("ExecuteEffect: Effect '%s' not registered"), *EffectTag.ToString());
		return false;
	}

	UObject* EffectInstance = GetOrCreateEffectInstance(*Registration);
	if (!EffectInstance)
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("ExecuteEffect: Failed to create instance for '%s'"), *EffectTag.ToString());
		return false;
	}

	IGenericGameEffect* EffectInterface = Cast<IGenericGameEffect>(EffectInstance);
	if (!EffectInterface)
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("ExecuteEffect: Instance does not implement IGenericGameEffect for '%s'"), *EffectTag.ToString());
		return false;
	}

	bool bSuccess = EffectInterface->Execute_ExecuteEffect(EffectInstance, WorldContext, Payload);
	
	UE_LOG(LogEffectRegistry, Verbose, TEXT("Executed effect '%s': %s"), 
		*EffectTag.ToString(), 
		bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));

	return bSuccess;
}

bool UGameEventEffectRegistry::EvaluateCondition(const FGameplayTag& ConditionTag, UObject* WorldContext, const FGameEventPayload& Payload) const
{
	const FConditionRegistration* Registration = RegisteredConditions.Find(ConditionTag);
	if (!Registration)
	{
		UE_LOG(LogEffectRegistry, Warning, TEXT("EvaluateCondition: Condition '%s' not registered"), *ConditionTag.ToString());
		return false;
	}

	// Need mutable access for instance creation - use const_cast (safe here)
	UObject* EvaluatorInstance = const_cast<UGameEventEffectRegistry*>(this)->GetOrCreateConditionInstance(
		const_cast<FConditionRegistration&>(*Registration)
	);

	if (!EvaluatorInstance)
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("EvaluateCondition: Failed to create instance for '%s'"), *ConditionTag.ToString());
		return false;
	}

	IGenericConditionEvaluator* ConditionInterface = Cast<IGenericConditionEvaluator>(EvaluatorInstance);
	if (!ConditionInterface)
	{
		UE_LOG(LogEffectRegistry, Error, TEXT("EvaluateCondition: Instance does not implement IGenericConditionEvaluator for '%s'"), 
			*ConditionTag.ToString());
		return false;
	}

	bool bResult = ConditionInterface->Execute_EvaluateCondition(EvaluatorInstance, WorldContext, Payload);
	
	UE_LOG(LogEffectRegistry, VeryVerbose, TEXT("Evaluated condition '%s': %s"), 
		*ConditionTag.ToString(), 
		bResult ? TEXT("TRUE") : TEXT("FALSE"));

	return bResult;
}

bool UGameEventEffectRegistry::IsEffectRegistered(const FGameplayTag& EffectTag) const
{
	return RegisteredEffects.Contains(EffectTag);
}

bool UGameEventEffectRegistry::IsConditionRegistered(const FGameplayTag& ConditionTag) const
{
	return RegisteredConditions.Contains(ConditionTag);
}

TArray<FGameplayTag> UGameEventEffectRegistry::GetAllRegisteredEffects() const
{
	TArray<FGameplayTag> Tags;
	RegisteredEffects.GenerateKeyArray(Tags);
	return Tags;
}

TArray<FGameplayTag> UGameEventEffectRegistry::GetAllRegisteredConditions() const
{
	TArray<FGameplayTag> Tags;
	RegisteredConditions.GenerateKeyArray(Tags);
	return Tags;
}

void UGameEventEffectRegistry::UnregisterEffect(const FGameplayTag& EffectTag)
{
	if (RegisteredEffects.Remove(EffectTag) > 0)
	{
		UE_LOG(LogEffectRegistry, Log, TEXT("Unregistered effect '%s'"), *EffectTag.ToString());
	}
}

void UGameEventEffectRegistry::UnregisterCondition(const FGameplayTag& ConditionTag)
{
	if (RegisteredConditions.Remove(ConditionTag) > 0)
	{
		UE_LOG(LogEffectRegistry, Log, TEXT("Unregistered condition '%s'"), *ConditionTag.ToString());
	}
}

void UGameEventEffectRegistry::ClearAllRegistrations()
{
	RegisteredEffects.Empty();
	RegisteredConditions.Empty();
	UE_LOG(LogEffectRegistry, Log, TEXT("Cleared all registrations"));
}

UObject* UGameEventEffectRegistry::GetOrCreateEffectInstance(FEffectRegistration& Registration)
{
	// Return cached instance if available
	if (Registration.CachedInstance)
	{
		return Registration.CachedInstance;
	}

	// Create new instance
	if (Registration.EffectClass)
	{
		Registration.CachedInstance = NewObject<UObject>(this, Registration.EffectClass);
		return Registration.CachedInstance;
	}

	return nullptr;
}

UObject* UGameEventEffectRegistry::GetOrCreateConditionInstance(FConditionRegistration& Registration)
{
	// Return cached instance if available
	if (Registration.CachedInstance)
	{
		return Registration.CachedInstance;
	}

	// Create new instance
	if (Registration.EvaluatorClass)
	{
		Registration.CachedInstance = NewObject<UObject>(this, Registration.EvaluatorClass);
		return Registration.CachedInstance;
	}

	return nullptr;
}
