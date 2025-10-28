// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class UScheduleData;

/**
 * Asset type actions for ScheduleData assets
 * Defines how ScheduleData appears and behaves in the content browser
 */
class FAssetTypeActions_ScheduleData : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "ScheduleData", "Schedule Data"); }
	virtual FColor GetTypeColor() const override { return FColor(255, 196, 128); } // Orange
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; }
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

private:
	/** Action: Create a new schedule entry for this schedule */
	void ExecuteCreateEntry(TArray<TWeakObjectPtr<UScheduleData>> Objects);

	/** Action: Validate schedule data */
	void ExecuteValidateSchedule(TArray<TWeakObjectPtr<UScheduleData>> Objects);
};
