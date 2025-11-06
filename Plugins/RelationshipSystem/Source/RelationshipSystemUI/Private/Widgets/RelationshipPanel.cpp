// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/RelationshipPanel.h"
#include "Widgets/RelationshipDimensionBar.h"
#include "Components/NPCRelationshipComponent.h"
#include "Components/VerticalBox.h"
#include "Subsystems/RelationshipSubsystem.h"
#include "DataAssets/RelationshipDatabase.h"
#include "DataAssets/RelationshipDimension.h"
#include "DataAssets/RelationshipState.h"

// Conditional include for GameEventBus
#if __has_include("GameEventBusSubsystem.h")
	#include "GameEventBusSubsystem.h"
	#define GAMEEVENTBUS_AVAILABLE 1
#else
	#define GAMEEVENTBUS_AVAILABLE 0
#endif

FGameplayTag URelationshipPanel::GetCurrentState() const
{
	if (!Subject || !Target)
	{
		return FGameplayTag();
	}

	// ? FIX: Use GameInstance->GetSubsystem for GameInstanceSubsystem
	UWorld* World = GetWorld();
	if (!World)
	{
		return FGameplayTag();
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return FGameplayTag();
	}

	URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>();
	if (!Subsystem)
	{
		return FGameplayTag();
	}

	return Subsystem->GetCurrentState(Subject, Target);
}

FText URelationshipPanel::GetStateDisplayName() const
{
	FGameplayTag StateTag = GetCurrentState();
	
	if (!StateTag.IsValid())
	{
		return FText::FromString(TEXT("No State"));
	}

	// ? FIX: Use GameInstance subsystem
	UWorld* World = GetWorld();
	if (World)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>())
			{
				if (URelationshipDatabase* Database = Subsystem->GetDatabase())
				{
					if (URelationshipState* State = Database->FindState(StateTag))
					{
						return State->GetDisplayName();
					}
				}
			}
		}
	}

	// Fallback to tag name
	return FText::FromString(StateTag.ToString());
}

void URelationshipPanel::NativeConstruct()
{
	Super::NativeConstruct();

	// Subscribe to events
	SubscribeToEvents();

	// Hide by default if auto-hide enabled
	if (bAutoHideWhenNoDialogue)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void URelationshipPanel::NativeDestruct()
{
	UnsubscribeFromEvents();

	Super::NativeDestruct();
}

void URelationshipPanel::NativeRefreshDisplay()
{
	Super::NativeRefreshDisplay();

	// ? FIX: Ensure relationship exists BEFORE creating dimension bars!
	if (Subject && Target)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>())
				{
					// ? Check if relationship exists
					if (!Subsystem->HasRelationship(Subject, Target))
					{
						UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel: Relationship doesn't exist! Creating it now..."));
						UE_LOG(LogTemp, Warning, TEXT("  Subject: %s"), *GetNameSafe(Subject));
						UE_LOG(LogTemp, Warning, TEXT("  Target: %s"), *GetNameSafe(Target));
						
						// ? Create relationship with default profiles
						// This is safe - Subsystem will handle profile loading
						Subsystem->CreateRelationship(Subject, Target, nullptr, nullptr);
						
						UE_LOG(LogTemp, Log, TEXT("RelationshipPanel: Relationship created successfully!"));
					}
				}
			}
		}
	}

	// Recreate dimension bars
	CreateDimensionBars();

	// Refresh all dimension bars
	for (URelationshipDimensionBar* Bar : SpawnedDimensionBars)
	{
		if (Bar)
		{
			Bar->RefreshDisplay();
		}
	}
}

void URelationshipPanel::CreateDimensionBars()
{
	ClearDimensionBars();

	UE_LOG(LogTemp, Log, TEXT("RelationshipPanel::CreateDimensionBars - START"));
	UE_LOG(LogTemp, Log, TEXT("  DimensionContainer: %s"), DimensionContainer ? TEXT("Valid") : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  DimensionBarClass: %s"), DimensionBarClass ? TEXT("Valid") : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  Subject: %s"), *GetNameSafe(Subject));
	UE_LOG(LogTemp, Log, TEXT("  Target: %s"), *GetNameSafe(Target));

	if (!DimensionContainer || !DimensionBarClass || !Subject || !Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel::CreateDimensionBars - ABORTED: Missing required components"));
		return;
	}

	// ? FIX: Use GameInstance->GetSubsystem for GameInstanceSubsystem
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel::CreateDimensionBars - ABORTED: No World"));
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel::CreateDimensionBars - ABORTED: No GameInstance"));
		return;
	}

	URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel::CreateDimensionBars - ABORTED: No RelationshipSubsystem"));
		return;
	}

	URelationshipDatabase* Database = Subsystem->GetDatabase();
	if (!Database)
	{
		UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel::CreateDimensionBars - ABORTED: No Database"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("RelationshipPanel::CreateDimensionBars - Database found: %s"), *GetNameSafe(Database));

	// Get all dimensions from database
	TArray<URelationshipDimension*> AllDimensions = Database->GetAllDimensions();

	UE_LOG(LogTemp, Log, TEXT("RelationshipPanel::CreateDimensionBars - Total dimensions in database: %d"), AllDimensions.Num());
	UE_LOG(LogTemp, Log, TEXT("  bShowAllDimensions: %s"), bShowAllDimensions ? TEXT("TRUE") : TEXT("FALSE"));

	int32 CreatedBars = 0;
	int32 SkippedBars = 0;

	for (URelationshipDimension* Dimension : AllDimensions)
	{
		if (!Dimension)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Dimension[NULL] - SKIPPED (null dimension)"));
			continue;
		}

		FGameplayTag DimTag = Dimension->GetAssetTag();
		
		UE_LOG(LogTemp, Log, TEXT("  Processing dimension: %s"), *DimTag.ToString());

		// Check if dimension has value (if filtering)
		if (!bShowAllDimensions)
		{
			// ? FIX: Use Subsystem directly
			float Value = Subsystem->GetDimensionValue(Subject, Target, DimTag);
			UE_LOG(LogTemp, Log, TEXT("    Current value: %.2f"), Value);
			
			if (FMath::IsNearlyZero(Value))
			{
				UE_LOG(LogTemp, Log, TEXT("    SKIPPED (zero value, bShowAllDimensions=false)"));
				SkippedBars++;
				continue; // Skip zero dimensions
			}
		}

		// Create dimension bar widget
		URelationshipDimensionBar* DimBar = CreateWidget<URelationshipDimensionBar>(this, DimensionBarClass);
		if (DimBar)
		{
			DimBar->DimensionTag = DimTag;
			DimBar->SetActors(Subject, Target);
			DimBar->bAutoRefresh = bAutoRefresh;
			DimBar->RefreshInterval = RefreshInterval;

			DimensionContainer->AddChildToVerticalBox(DimBar);
			SpawnedDimensionBars.Add(DimBar);
			
			CreatedBars++;
			UE_LOG(LogTemp, Log, TEXT("    ? CREATED dimension bar for '%s'"), *DimTag.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("    ? FAILED to create widget for '%s'"), *DimTag.ToString());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("RelationshipPanel::CreateDimensionBars - FINISHED"));
	UE_LOG(LogTemp, Log, TEXT("  Created bars: %d"), CreatedBars);
	UE_LOG(LogTemp, Log, TEXT("  Skipped bars: %d"), SkippedBars);
	UE_LOG(LogTemp, Log, TEXT("  Total spawned: %d"), SpawnedDimensionBars.Num());
}

void URelationshipPanel::ClearDimensionBars()
{
	if (DimensionContainer)
	{
		DimensionContainer->ClearChildren();
	}

	SpawnedDimensionBars.Empty();
}

void URelationshipPanel::SubscribeToEvents()
{
#if GAMEEVENTBUS_AVAILABLE
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UGameEventBusSubsystem* EventBus = GameInstance->GetSubsystem<UGameEventBusSubsystem>();
		if (!EventBus)
		{
			UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel: GameEventBus not available"));
			return;
		}

		// Subscribe to dialogue events
		if (bListenToDialogueEvents)
		{
			// FIXED: Use correct event tags from DialogueSubsystem
			FGameplayTag DialogueStartedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started"), false);
			FGameplayTag DialogueEndedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Completed"), false);
			
			if (DialogueStartedTag.IsValid())
			{
				// Use lambda instead of member function to avoid FGameEventPayload in header
				TWeakObjectPtr<URelationshipPanel> WeakThis(this);
				DialogueStartedHandle = EventBus->SubscribeToEventNative(
					DialogueStartedTag,
					FGameEventNativeDelegate::FDelegate::CreateLambda([WeakThis](const FGameEventPayload& Payload)
					{
						if (URelationshipPanel* Panel = WeakThis.Get())
						{
							AActor* Player = Payload.InstigatorActor.Get();
							AActor* NPC = Payload.TargetActor.Get();
							
							UE_LOG(LogTemp, Log, TEXT("RelationshipPanel: Dialogue.Started - Player=%s, NPC=%s"),
								*GetNameSafe(Player), *GetNameSafe(NPC));
							
							if (Player && NPC)
							{
								// FIXED: Show NPC's relationship TO Player (not Player's relationship to NPC)
								// Relationships are stored as Subject -> Target, so we want NPC -> Player
								Panel->SetActors(NPC, Player);
								if (Panel->bAutoHideWhenNoDialogue)
								{
									Panel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
								}
								Panel->RefreshDisplay();
								
								UE_LOG(LogTemp, Log, TEXT("RelationshipPanel: SetActors(NPC=%s, Player=%s) called, visibility changed"),
									*GetNameSafe(NPC), *GetNameSafe(Player));
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel: Invalid actors - Player=%s, NPC=%s"),
									Player ? TEXT("Valid") : TEXT("NULL"),
									NPC ? TEXT("Valid") : TEXT("NULL"));
							}
						}
					})
				);
			}

			if (DialogueEndedTag.IsValid())
			{
				TWeakObjectPtr<URelationshipPanel> WeakThis(this);
				DialogueEndedHandle = EventBus->SubscribeToEventNative(
					DialogueEndedTag,
					FGameEventNativeDelegate::FDelegate::CreateLambda([WeakThis](const FGameEventPayload& Payload)
					{
						if (URelationshipPanel* Panel = WeakThis.Get())
						{
							if (Panel->bAutoHideWhenNoDialogue)
							{
								Panel->SetVisibility(ESlateVisibility::Collapsed);
								UE_LOG(LogTemp, Log, TEXT("RelationshipPanel: Dialogue ended, panel hidden"));
							}
						}
					})
				);
			}
		}

		// Subscribe to relationship changes
		FGameplayTag RelationshipChangedTag = FGameplayTag::RequestGameplayTag(FName("Relationship.DimensionChanged"), false);
		if (RelationshipChangedTag.IsValid())
		{
			TWeakObjectPtr<URelationshipPanel> WeakThis(this);
			RelationshipChangedHandle = EventBus->SubscribeToEventNative(
				RelationshipChangedTag,
				FGameEventNativeDelegate::FDelegate::CreateLambda([WeakThis](const FGameEventPayload& Payload)
				{
					if (URelationshipPanel* Panel = WeakThis.Get())
					{
						AActor* EventSubject = Payload.InstigatorActor.Get();
						AActor* EventTarget = Payload.TargetActor.Get();
						
						UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel: Relationship.DimensionChanged event received!"));
						UE_LOG(LogTemp, Warning, TEXT("  EventSubject: %s"), *GetNameSafe(EventSubject));
						UE_LOG(LogTemp, Warning, TEXT("  EventTarget: %s"), *GetNameSafe(EventTarget));
						UE_LOG(LogTemp, Warning, TEXT("  Panel->Subject: %s"), *GetNameSafe(Panel->Subject));
						UE_LOG(LogTemp, Warning, TEXT("  Panel->Target: %s"), *GetNameSafe(Panel->Target));
						UE_LOG(LogTemp, Warning, TEXT("  StringParam (Dimension): %s"), *Payload.StringParam.ToString());
						UE_LOG(LogTemp, Warning, TEXT("  FloatParam (Value): %.2f"), Payload.FloatParam);
						
						if (Panel->Subject == EventSubject && Panel->Target == EventTarget)
						{
							UE_LOG(LogTemp, Warning, TEXT("  ? MATCH! This event is for us!"));
							
							FName ChangedDimension = Payload.StringParam;
							bool bRefreshed = false;
							
							UE_LOG(LogTemp, Warning, TEXT("  Checking %d dimension bars..."), Panel->SpawnedDimensionBars.Num());
							
							for (URelationshipDimensionBar* Bar : Panel->SpawnedDimensionBars)
							{
								if (Bar)
								{
									FName BarDimensionName = Bar->DimensionTag.GetTagName();
									UE_LOG(LogTemp, Warning, TEXT("    Bar dimension: %s (vs %s)"), 
										*BarDimensionName.ToString(), *ChangedDimension.ToString());
									
									if (BarDimensionName == ChangedDimension)
									{
										UE_LOG(LogTemp, Warning, TEXT("    ? REFRESHING dimension bar: %s"), *ChangedDimension.ToString());
										Bar->RefreshDisplay();
										bRefreshed = true;
										break;
									}
								}
							}
							
							if (!bRefreshed)
							{
								UE_LOG(LogTemp, Warning, TEXT("  ?? No matching bar found, refreshing entire panel"));
								Panel->RefreshDisplay();
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("  ? Dimension bar refreshed successfully!"));
							}
						}
						else
						{
							UE_LOG(LogTemp, Verbose, TEXT("  ? Event not for us (different actors)"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("RelationshipPanel: WeakThis invalid!"));
					}
				})
			);
			
			UE_LOG(LogTemp, Log, TEXT("RelationshipPanel: Subscribed to Relationship.DimensionChanged"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("RelationshipPanel: 'Relationship.DimensionChanged' GameplayTag NOT FOUND!"));
			UE_LOG(LogTemp, Error, TEXT("  Add it to DefaultGameplayTags.ini:"));
			UE_LOG(LogTemp, Error, TEXT("  +GameplayTagList=(Tag=\"Relationship.DimensionChanged\",DevComment=\"Emitted when relationship dimension value changes\")"));
		}

		// Subscribe to time events (for decay)
		if (bListenToTimeEvents)
		{
			FGameplayTag TimeHourChangedTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.HourChanged"), false);
			if (TimeHourChangedTag.IsValid())
			{
				TWeakObjectPtr<URelationshipPanel> WeakThis(this);
				TimeHourChangedHandle = EventBus->SubscribeToEventNative(
					TimeHourChangedTag,
					FGameEventNativeDelegate::FDelegate::CreateLambda([WeakThis](const FGameEventPayload& Payload)
					{
						if (URelationshipPanel* Panel = WeakThis.Get())
						{
							if (Panel->Subject && Panel->Target && Panel->GetVisibility() != ESlateVisibility::Collapsed)
							{
								Panel->RefreshDisplay();
							}
						}
					})
				);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("? RelationshipPanel: Subscribed to GameEventBus events"));
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("?? RelationshipPanel: GameEventBus not available, using manual updates only"));
#endif
}

void URelationshipPanel::UnsubscribeFromEvents()
{
	// Remove old member function handlers - implementation stays the same
	#if GAMEEVENTBUS_AVAILABLE
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UGameEventBusSubsystem* EventBus = GameInstance->GetSubsystem<UGameEventBusSubsystem>();
		if (!EventBus)
		{
			return;
		}

		// Unsubscribe from all events
		if (DialogueStartedHandle.IsValid())
		{
			// FIXED: Use correct event tag
			FGameplayTag DialogueStartedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started"), false);
			if (DialogueStartedTag.IsValid())
			{
				EventBus->UnsubscribeNative(DialogueStartedTag, DialogueStartedHandle);
			}
			DialogueStartedHandle.Reset();
		}

		if (DialogueEndedHandle.IsValid())
		{
			// FIXED: Use correct event tag
			FGameplayTag DialogueEndedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Completed"), false);
			if (DialogueEndedTag.IsValid())
			{
				EventBus->UnsubscribeNative(DialogueEndedTag, DialogueEndedHandle);
			}
			DialogueEndedHandle.Reset();
		}

		if (RelationshipChangedHandle.IsValid())
		{
			FGameplayTag RelationshipChangedTag = FGameplayTag::RequestGameplayTag(FName("Relationship.DimensionChanged"), false);
			if (RelationshipChangedTag.IsValid())
			{
				EventBus->UnsubscribeNative(RelationshipChangedTag, RelationshipChangedHandle);
			}
			RelationshipChangedHandle.Reset();
		}

		if (TimeHourChangedHandle.IsValid())
		{
			FGameplayTag TimeHourChangedTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.HourChanged"), false);
			if (TimeHourChangedTag.IsValid())
			{
				EventBus->UnsubscribeNative(TimeHourChangedTag, TimeHourChangedHandle);
			}
			TimeHourChangedHandle.Reset();
		}

		UE_LOG(LogTemp, Verbose, TEXT("RelationshipPanel: Unsubscribed from GameEventBus events"));
	}
#endif
}
