// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class UQuestAsset;

/**
 * Asset Type Actions for Quest Asset
 * Defines how Quest Assets appear and behave in the editor
 */
class FAssetTypeActions_QuestAsset : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_QuestAsset() = default;

	//~ Begin IAssetTypeActions Interface
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;
	//~ End IAssetTypeActions Interface

private:
	void ExecuteValidate(TArray<TWeakObjectPtr<UQuestAsset>> Objects);
	void ExecuteDuplicateWithNewId(TArray<TWeakObjectPtr<UQuestAsset>> Objects);
};
