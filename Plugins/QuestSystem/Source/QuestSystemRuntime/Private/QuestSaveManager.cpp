// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestSaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"

UQuestSaveManager::UQuestSaveManager()
{
}

void UQuestSaveManager::Initialize()
{
	// Log save directory info on initialization
	FString SaveDirectory = FPaths::ProjectSavedDir() / TEXT("SaveGames");
	FString FullSavePath = FPaths::ConvertRelativePathToFull(SaveDirectory);
	
	UE_LOG(LogTemp, Log, TEXT("========================================"));
	UE_LOG(LogTemp, Log, TEXT("QuestSaveManager: Initialized"));
	UE_LOG(LogTemp, Log, TEXT("Save Directory: %s"), *FullSavePath);
	UE_LOG(LogTemp, Log, TEXT("Save Slot Name: %s"), *DefaultSlotName);
	UE_LOG(LogTemp, Log, TEXT("Full Save Path: %s"), *GetSaveFilePath(DefaultSlotName));
	UE_LOG(LogTemp, Log, TEXT("========================================"));
}

void UQuestSaveManager::Shutdown()
{
	DisableAutoSave();
	UE_LOG(LogTemp, Log, TEXT("QuestSaveManager: Shutdown"));
}

// Helper function to get full save file path
FString UQuestSaveManager::GetSaveFilePath(const FString& SlotName) const
{
	FString SaveDirectory = FPaths::ProjectSavedDir() / TEXT("SaveGames");
	FString SavePath = SaveDirectory / (SlotName + TEXT(".sav"));
	return FPaths::ConvertRelativePathToFull(SavePath);
}

// === Save/Load (Async) ===

void UQuestSaveManager::SaveQuestProgressAsync(UQuestSaveGame* SaveGame, FOnSaveCompleteCallback OnComplete)
{
	if (!SaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestSaveManager: Cannot save - SaveGame is null"));
		OnComplete.ExecuteIfBound(false);
		return;
	}

	SaveGame->LastSaveTime = FDateTime::Now();
	
	FString SavePath = GetSaveFilePath(SaveGame->SaveSlotName);

	// Async save using UGameplayStatics
	// Note: UE5 doesn't have true async save yet, so we use AsyncTask
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, SaveGame, OnComplete, SavePath]()
	{
		bool bSuccess = UGameplayStatics::SaveGameToSlot(
			SaveGame, 
			SaveGame->SaveSlotName, 
			SaveGame->UserIndex
		);

		// Return to game thread for callback
		AsyncTask(ENamedThreads::GameThread, [OnComplete, bSuccess, SavePath]()
		{
			if (bSuccess)
			{
				UE_LOG(LogTemp, Log, TEXT("? QuestSaveManager: Save completed successfully"));
				UE_LOG(LogTemp, Log, TEXT("   File saved to: %s"), *SavePath);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("? QuestSaveManager: Save failed"));
				UE_LOG(LogTemp, Error, TEXT("   Attempted path: %s"), *SavePath);
			}

			OnComplete.ExecuteIfBound(bSuccess);
		});
	});
}

void UQuestSaveManager::LoadQuestProgressAsync(const FString& SlotName, int32 UserIndex, FOnLoadCompleteCallback OnComplete)
{
	FString SavePath = GetSaveFilePath(SlotName);
	
	// Async load
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, SlotName, UserIndex, OnComplete, SavePath]()
	{
		UQuestSaveGame* LoadedSave = Cast<UQuestSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex)
		);

		bool bSuccess = (LoadedSave != nullptr);

		// Return to game thread
		AsyncTask(ENamedThreads::GameThread, [this, OnComplete, bSuccess, LoadedSave, SavePath]()
		{
			if (bSuccess)
			{
				CurrentSaveGame = LoadedSave;
				UE_LOG(LogTemp, Log, TEXT("? QuestSaveManager: Loaded save game (%d quests)"), 
					CurrentSaveGame->QuestProgress.Num());
				UE_LOG(LogTemp, Log, TEXT("   Loaded from: %s"), *SavePath);
			}
			else
			{
				// Create new save game
				CurrentSaveGame = Cast<UQuestSaveGame>(
					UGameplayStatics::CreateSaveGameObject(UQuestSaveGame::StaticClass())
				);
				UE_LOG(LogTemp, Warning, TEXT("?? QuestSaveManager: No save file found, created new save game"));
				UE_LOG(LogTemp, Warning, TEXT("   Expected path: %s"), *SavePath);
			}

			OnComplete.ExecuteIfBound(bSuccess, CurrentSaveGame);
		});
	});
}

// === Save/Load (Sync - Deprecated) ===

bool UQuestSaveManager::SaveQuestProgress(UQuestSaveGame* SaveGame)
{
	if (!SaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestSaveManager: Cannot save - SaveGame is null"));
		return false;
	}

	SaveGame->LastSaveTime = FDateTime::Now();
	FString SavePath = GetSaveFilePath(SaveGame->SaveSlotName);

	bool bSuccess = UGameplayStatics::SaveGameToSlot(
		SaveGame, 
		SaveGame->SaveSlotName, 
		SaveGame->UserIndex
	);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("? QuestSaveManager: Saved quest progress"));
		UE_LOG(LogTemp, Log, TEXT("   Saved to: %s"), *SavePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("? QuestSaveManager: Failed to save quest progress"));
		UE_LOG(LogTemp, Error, TEXT("   Attempted path: %s"), *SavePath);
	}

	return bSuccess;
}

UQuestSaveGame* UQuestSaveManager::LoadQuestProgress(const FString& SlotName, int32 UserIndex)
{
	FString SavePath = GetSaveFilePath(SlotName);
	
	UQuestSaveGame* LoadedSave = Cast<UQuestSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex)
	);

	if (LoadedSave)
	{
		CurrentSaveGame = LoadedSave;
		UE_LOG(LogTemp, Log, TEXT("? QuestSaveManager: Loaded quest progress (%d quests)"), 
			CurrentSaveGame->QuestProgress.Num());
		UE_LOG(LogTemp, Log, TEXT("   Loaded from: %s"), *SavePath);
	}
	else
	{
		CurrentSaveGame = Cast<UQuestSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UQuestSaveGame::StaticClass())
		);
		UE_LOG(LogTemp, Warning, TEXT("?? QuestSaveManager: No save file found, created new save game"));
		UE_LOG(LogTemp, Warning, TEXT("   Expected path: %s"), *SavePath);
	}

	return CurrentSaveGame;
}

bool UQuestSaveManager::DeleteSaveSlot(const FString& SlotName, int32 UserIndex)
{
	FString SavePath = GetSaveFilePath(SlotName);
	bool bExists = UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
	
	if (!bExists)
	{
		UE_LOG(LogTemp, Warning, TEXT("?? QuestSaveManager: Save slot '%s' does not exist"), *SlotName);
		UE_LOG(LogTemp, Warning, TEXT("   Expected path: %s"), *SavePath);
		return false;
	}
	
	bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("? QuestSaveManager: Deleted save slot '%s'"), *SlotName);
		UE_LOG(LogTemp, Log, TEXT("   Deleted file: %s"), *SavePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("? QuestSaveManager: Failed to delete save slot '%s'"), *SlotName);
		UE_LOG(LogTemp, Error, TEXT("   Attempted path: %s"), *SavePath);
	}

	return bSuccess;
}

bool UQuestSaveManager::DoesSaveSlotExist(const FString& SlotName, int32 UserIndex) const
{
	bool bExists = UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
	FString SavePath = GetSaveFilePath(SlotName);
	
	UE_LOG(LogTemp, Log, TEXT("QuestSaveManager: Checking save slot '%s' - %s"), 
		*SlotName, bExists ? TEXT("EXISTS") : TEXT("NOT FOUND"));
	UE_LOG(LogTemp, Log, TEXT("   Path: %s"), *SavePath);
	
	return bExists;
}

// === Auto-Save ===

void UQuestSaveManager::EnableAutoSave(float IntervalSeconds)
{
	if (bAutoSaveEnabled)
	{
		DisableAutoSave();
	}

	AutoSaveInterval = IntervalSeconds;
	bAutoSaveEnabled = true;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(
			AutoSaveTimerHandle,
			this,
			&UQuestSaveManager::OnAutoSave,
			AutoSaveInterval,
			true  // Loop
		);

		UE_LOG(LogTemp, Log, TEXT("?? QuestSaveManager: Auto-save enabled (interval: %.1f seconds)"), AutoSaveInterval);
	}
}

void UQuestSaveManager::DisableAutoSave()
{
	if (!bAutoSaveEnabled)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World && AutoSaveTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(AutoSaveTimerHandle);
	}

	bAutoSaveEnabled = false;
	UE_LOG(LogTemp, Log, TEXT("?? QuestSaveManager: Auto-save disabled"));
}

void UQuestSaveManager::OnAutoSave()
{
	if (!CurrentSaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("?? QuestSaveManager: Auto-save skipped - no save game"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("?? QuestSaveManager: Auto-saving..."));

	// Use empty callback instead of lambda
	FOnSaveCompleteCallback SaveCallback;
	SaveQuestProgressAsync(CurrentSaveGame, SaveCallback);
}
