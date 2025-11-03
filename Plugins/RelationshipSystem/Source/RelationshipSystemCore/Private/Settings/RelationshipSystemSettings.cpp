// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings/RelationshipSystemSettings.h"

#define LOCTEXT_NAMESPACE "RelationshipSystemSettings"

URelationshipSystemSettings::URelationshipSystemSettings()
	: bEnableAutomaticDecay(true)
	, DecayProcessingInterval(60.0f)
	, GameHoursPerRealSecond(1.0f)
	, bEnableDebugLogging(false)
	, bShowDebugInfo(false)
{
}

FName URelationshipSystemSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

#if WITH_EDITOR
FText URelationshipSystemSettings::GetSectionText() const
{
	return LOCTEXT("SectionText", "Relationship System");
}

FText URelationshipSystemSettings::GetSectionDescription() const
{
	return LOCTEXT("SectionDescription", "Configure the Relationship System plugin settings");
}
#endif

#undef LOCTEXT_NAMESPACE
