// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueVariant.h"

//==============================================================================
// FDialogueVariant Implementation
//==============================================================================

FDialogueVariant::FDialogueVariant()
	: Type(EDialogueVariantType::None)
	, BoolValue(false)
	, IntValue(0)
	, FloatValue(0.0f)
	, NameValue(NAME_None)
{
}

FDialogueVariant::FDialogueVariant(bool InValue)
	: Type(EDialogueVariantType::Bool)
	, BoolValue(InValue)
	, IntValue(0)
	, FloatValue(0.0f)
	, NameValue(NAME_None)
{
}

FDialogueVariant::FDialogueVariant(int32 InValue)
	: Type(EDialogueVariantType::Int)
	, BoolValue(false)
	, IntValue(InValue)
	, FloatValue(0.0f)
	, NameValue(NAME_None)
{
}

FDialogueVariant::FDialogueVariant(float InValue)
	: Type(EDialogueVariantType::Float)
	, BoolValue(false)
	, IntValue(0)
	, FloatValue(InValue)
	, NameValue(NAME_None)
{
}

FDialogueVariant::FDialogueVariant(const FString& InValue)
	: Type(EDialogueVariantType::String)
	, BoolValue(false)
	, IntValue(0)
	, FloatValue(0.0f)
	, StringValue(InValue)
	, NameValue(NAME_None)
{
}

FDialogueVariant::FDialogueVariant(FName InValue)
	: Type(EDialogueVariantType::Name)
	, BoolValue(false)
	, IntValue(0)
	, FloatValue(0.0f)
	, NameValue(InValue)
{
}

FDialogueVariant::FDialogueVariant(UObject* InValue)
	: Type(EDialogueVariantType::Object)
	, BoolValue(false)
	, IntValue(0)
	, FloatValue(0.0f)
	, NameValue(NAME_None)
	, ObjectValue(InValue)
{
}

FDialogueVariant::FDialogueVariant(const FGameplayTag& InValue)
	: Type(EDialogueVariantType::Tag)
	, BoolValue(false)
	, IntValue(0)
	, FloatValue(0.0f)
	, NameValue(NAME_None)
	, TagValue(InValue)
{
}

//==============================================================================
// Getters with Type Conversion
//==============================================================================

bool FDialogueVariant::GetBool(bool DefaultValue) const
{
	switch (Type)
	{
	case EDialogueVariantType::Bool:
		return BoolValue;
	case EDialogueVariantType::Int:
		return IntValue != 0;
	case EDialogueVariantType::Float:
		return !FMath::IsNearlyZero(FloatValue);
	case EDialogueVariantType::String:
		return StringValue.ToBool();
	default:
		return DefaultValue;
	}
}

int32 FDialogueVariant::GetInt(int32 DefaultValue) const
{
	switch (Type)
	{
	case EDialogueVariantType::Int:
		return IntValue;
	case EDialogueVariantType::Bool:
		return BoolValue ? 1 : 0;
	case EDialogueVariantType::Float:
		return FMath::RoundToInt(FloatValue);
	case EDialogueVariantType::String:
		return FCString::Atoi(*StringValue);
	default:
		return DefaultValue;
	}
}

float FDialogueVariant::GetFloat(float DefaultValue) const
{
	switch (Type)
	{
	case EDialogueVariantType::Float:
		return FloatValue;
	case EDialogueVariantType::Int:
		return static_cast<float>(IntValue);
	case EDialogueVariantType::Bool:
		return BoolValue ? 1.0f : 0.0f;
	case EDialogueVariantType::String:
		return FCString::Atof(*StringValue);
	default:
		return DefaultValue;
	}
}

FString FDialogueVariant::GetString(const FString& DefaultValue) const
{
	switch (Type)
	{
	case EDialogueVariantType::String:
		return StringValue;
	case EDialogueVariantType::Bool:
		return BoolValue ? TEXT("true") : TEXT("false");
	case EDialogueVariantType::Int:
		return FString::FromInt(IntValue);
	case EDialogueVariantType::Float:
		return FString::SanitizeFloat(FloatValue);
	case EDialogueVariantType::Name:
		return NameValue.ToString();
	case EDialogueVariantType::Object:
		return ObjectValue.IsValid() ? ObjectValue->GetName() : TEXT("None");
	case EDialogueVariantType::Tag:
		return TagValue.ToString();
	default:
		return DefaultValue;
	}
}

FName FDialogueVariant::GetName(FName DefaultValue) const
{
	switch (Type)
	{
	case EDialogueVariantType::Name:
		return NameValue;
	case EDialogueVariantType::String:
		return FName(*StringValue);
	default:
		return DefaultValue;
	}
}

UObject* FDialogueVariant::GetObject(UObject* DefaultValue) const
{
	if (Type == EDialogueVariantType::Object)
	{
		return ObjectValue.Get();
	}
	return DefaultValue;
}

FGameplayTag FDialogueVariant::GetTag(const FGameplayTag& DefaultValue) const
{
	if (Type == EDialogueVariantType::Tag)
	{
		return TagValue;
	}
	return DefaultValue;
}

//==============================================================================
// String Conversion
//==============================================================================

FString FDialogueVariant::ToString() const
{
	// Format: "Type:Value"
	// Examples:
	//   "Bool:true"
	//   "Int:42"
	//   "Float:3.14"
	//   "String:Hello World"

	FString TypeStr;
	switch (Type)
	{
	case EDialogueVariantType::Bool: TypeStr = TEXT("Bool"); break;
	case EDialogueVariantType::Int: TypeStr = TEXT("Int"); break;
	case EDialogueVariantType::Float: TypeStr = TEXT("Float"); break;
	case EDialogueVariantType::String: TypeStr = TEXT("String"); break;
	case EDialogueVariantType::Name: TypeStr = TEXT("Name"); break;
	case EDialogueVariantType::Object: TypeStr = TEXT("Object"); break;
	case EDialogueVariantType::Tag: TypeStr = TEXT("Tag"); break;
	default: TypeStr = TEXT("None"); break;
	}

	return FString::Printf(TEXT("%s:%s"), *TypeStr, *GetString());
}

FDialogueVariant FDialogueVariant::FromString(const FString& TypedString)
{
	// Parse "Type:Value" format
	FString TypeStr, ValueStr;
	if (!TypedString.Split(TEXT(":"), &TypeStr, &ValueStr))
	{
		// No type specified, assume string
		return FDialogueVariant(TypedString);
	}

	// Determine type
	if (TypeStr == TEXT("Bool"))
	{
		bool Value = ValueStr.ToBool();
		return FDialogueVariant(Value);
	}
	else if (TypeStr == TEXT("Int"))
	{
		int32 Value = FCString::Atoi(*ValueStr);
		return FDialogueVariant(Value);
	}
	else if (TypeStr == TEXT("Float"))
	{
		float Value = FCString::Atof(*ValueStr);
		return FDialogueVariant(Value);
	}
	else if (TypeStr == TEXT("Name"))
	{
		FName Value = FName(*ValueStr);
		return FDialogueVariant(Value);
	}
	else if (TypeStr == TEXT("Tag"))
	{
		FGameplayTag Value = FGameplayTag::RequestGameplayTag(FName(*ValueStr), false);
		return FDialogueVariant(Value);
	}
	else // String or unknown
	{
		return FDialogueVariant(ValueStr);
	}
}

//==============================================================================
// Comparison
//==============================================================================

bool FDialogueVariant::operator==(const FDialogueVariant& Other) const
{
	if (Type != Other.Type)
	{
		return false;
	}

	switch (Type)
	{
	case EDialogueVariantType::Bool:
		return BoolValue == Other.BoolValue;
	case EDialogueVariantType::Int:
		return IntValue == Other.IntValue;
	case EDialogueVariantType::Float:
		return FMath::IsNearlyEqual(FloatValue, Other.FloatValue);
	case EDialogueVariantType::String:
		return StringValue.Equals(Other.StringValue);
	case EDialogueVariantType::Name:
		return NameValue == Other.NameValue;
	case EDialogueVariantType::Object:
		return ObjectValue == Other.ObjectValue;
	case EDialogueVariantType::Tag:
		return TagValue == Other.TagValue;
	default:
		return true; // Both None
	}
}
