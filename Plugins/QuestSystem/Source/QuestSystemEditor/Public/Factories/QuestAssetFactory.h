// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "QuestAssetFactory.generated.h"

/**
 * Factory for creating new Quest Asset
 */
UCLASS()
class QUESTSYSTEMEDITOR_API UQuestAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UQuestAssetFactory();

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override { return true; }
	//~ End UFactory Interface
};
