// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DialogueVariant.generated.h"

/**
 * Type-safe variant for dialogue variables
 * Replaces stringly-typed FString storage
 * 
 * Benefits:
 * - Type safety at compile time
 * - Better debugging
 * - Automatic type conversion
 * - Memory efficient
 * 
 * v1.3.1 - Architecture improvement
 */
UENUM(BlueprintType)
enum class EDialogueVariantType : uint8
{
	None    UMETA(DisplayName = "None"),
	Bool        UMETA(DisplayName = "Boolean"),
	Int         UMETA(DisplayName = "Integer"),
	Float       UMETA(DisplayName = "Float"),
	String      UMETA(DisplayName = "String"),
	Name        UMETA(DisplayName = "Name"),
	Object      UMETA(DisplayName = "Object"),
	Tag    UMETA(DisplayName = "Gameplay Tag")
};

/**
 * Type-safe variant value
 * Can hold different types and convert between them
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueVariant
{
	GENERATED_BODY()

	// Constructors
	FDialogueVariant();
	explicit FDialogueVariant(bool InValue);
	explicit FDialogueVariant(int32 InValue);
	explicit FDialogueVariant(float InValue);
	explicit FDialogueVariant(const FString& InValue);
	explicit FDialogueVariant(FName InValue);
	explicit FDialogueVariant(UObject* InValue);
	explicit FDialogueVariant(const FGameplayTag& InValue);

	// Type queries
	EDialogueVariantType GetType() const { return Type; }
	bool IsNone() const { return Type == EDialogueVariantType::None; }
	bool IsBool() const { return Type == EDialogueVariantType::Bool; }
	bool IsInt() const { return Type == EDialogueVariantType::Int; }
	bool IsFloat() const { return Type == EDialogueVariantType::Float; }
	bool IsString() const { return Type == EDialogueVariantType::String; }
	bool IsName() const { return Type == EDialogueVariantType::Name; }
	bool IsObject() const { return Type == EDialogueVariantType::Object; }
	bool IsTag() const { return Type == EDialogueVariantType::Tag; }

	// Getters (with default fallbacks)
	bool GetBool(bool DefaultValue = false) const;
	int32 GetInt(int32 DefaultValue = 0) const;
	float GetFloat(float DefaultValue = 0.0f) const;
	FString GetString(const FString& DefaultValue = TEXT("")) const;
	FName GetName(FName DefaultValue = NAME_None) const;
	UObject* GetObject(UObject* DefaultValue = nullptr) const;
	FGameplayTag GetTag(const FGameplayTag& DefaultValue = FGameplayTag()) const;

	// Conversion to string (for legacy compatibility)
	FString ToString() const;

	// Static factory methods
	static FDialogueVariant MakeBool(bool Value) { return FDialogueVariant(Value); }
	static FDialogueVariant MakeInt(int32 Value) { return FDialogueVariant(Value); }
	static FDialogueVariant MakeFloat(float Value) { return FDialogueVariant(Value); }
	static FDialogueVariant MakeString(const FString& Value) { return FDialogueVariant(Value); }
	static FDialogueVariant MakeName(FName Value) { return FDialogueVariant(Value); }
	static FDialogueVariant MakeObject(UObject* Value) { return FDialogueVariant(Value); }
	static FDialogueVariant MakeTag(const FGameplayTag& Value) { return FDialogueVariant(Value); }
	static FDialogueVariant FromString(const FString& TypedString);

	// Comparison operators
	bool operator==(const FDialogueVariant& Other) const;
	bool operator!=(const FDialogueVariant& Other) const { return !(*this == Other); }

private:
	UPROPERTY(SaveGame)
	EDialogueVariantType Type;

	// Numeric values (union would save memory but Blueprint doesn't support it)
	UPROPERTY(SaveGame)
	bool BoolValue;

	UPROPERTY(SaveGame)
	int32 IntValue;

	UPROPERTY(SaveGame)
	float FloatValue;

	// Reference types
	UPROPERTY(SaveGame)
	FString StringValue;

	UPROPERTY(SaveGame)
	FName NameValue;

	UPROPERTY(SaveGame)
	TWeakObjectPtr<UObject> ObjectValue;

	UPROPERTY(SaveGame)
	FGameplayTag TagValue;

	// Helper: Convert from string (for legacy support)
	void SetFromString(const FString& Value, EDialogueVariantType TargetType);
};

/**
 * Blueprint library for working with variants
 */
UCLASS()
class DIALOGUESYSTEMCORE_API UDialogueVariantLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Type checks
	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static bool IsVariantBool(const FDialogueVariant& Variant) { return Variant.IsBool(); }

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static bool IsVariantInt(const FDialogueVariant& Variant) { return Variant.IsInt(); }

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static bool IsVariantFloat(const FDialogueVariant& Variant) { return Variant.IsFloat(); }

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static bool IsVariantString(const FDialogueVariant& Variant) { return Variant.IsString(); }

	// Getters
	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static bool GetVariantBool(const FDialogueVariant& Variant, bool DefaultValue = false)
	{
		return Variant.GetBool(DefaultValue);
	}

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static int32 GetVariantInt(const FDialogueVariant& Variant, int32 DefaultValue = 0)
	{
		return Variant.GetInt(DefaultValue);
	}

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static float GetVariantFloat(const FDialogueVariant& Variant, float DefaultValue = 0.0f)
	{
		return Variant.GetFloat(DefaultValue);
	}

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static FString GetVariantString(const FDialogueVariant& Variant, const FString& DefaultValue = TEXT(""))
	{
		return Variant.GetString(DefaultValue);
	}

	// Factory methods
	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant", meta = (DisplayName = "Make Variant (Bool)"))
	static FDialogueVariant MakeVariantBool(bool Value)
	{
		return FDialogueVariant::MakeBool(Value);
	}

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant", meta = (DisplayName = "Make Variant (Int)"))
	static FDialogueVariant MakeVariantInt(int32 Value)
	{
		return FDialogueVariant::MakeInt(Value);
	}

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant", meta = (DisplayName = "Make Variant (Float)"))
	static FDialogueVariant MakeVariantFloat(float Value)
	{
		return FDialogueVariant::MakeFloat(Value);
	}

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant", meta = (DisplayName = "Make Variant (String)"))
	static FDialogueVariant MakeVariantString(const FString& Value)
	{
		return FDialogueVariant::MakeString(Value);
	}

	// Conversion
	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static FString VariantToString(const FDialogueVariant& Variant)
	{
		return Variant.ToString();
	}

	UFUNCTION(BlueprintPure, Category = "Dialogue|Variant")
	static FDialogueVariant StringToVariant(const FString& TypedString)
	{
		return FDialogueVariant::FromString(TypedString);
	}
};
