// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

/**
 * Detail customization for ScheduleEntryData
 * Provides enhanced editor UI for schedule entries
 */
class FScheduleEntryDataCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	/** Cached detail builder */
	IDetailLayoutBuilder* CachedDetailBuilder = nullptr;
};
