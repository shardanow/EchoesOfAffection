// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

/**
 * Details customization for Quest Asset
 * Provides custom UI in the details panel
 */
class FQuestAssetDetails : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	//~ Begin IDetailCustomization Interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	//~ End IDetailCustomization Interface

private:
	FReply OnValidateClicked();
	FReply OnGenerateQuestIdClicked();

	TWeakObjectPtr<class UQuestAsset> QuestAssetPtr;
};
