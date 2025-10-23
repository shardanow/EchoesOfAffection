// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/DialogueComponent.h"
#include "Interfaces/IDialogueParticipant.h"
#include "Interfaces/IDialogueService.h"
#include "Subsystems/DialogueSubsystem.h"
#include "Components/RelationshipComponent.h"
#include "Components/NPCMemoryComponent.h"
#include "Core/DialogueRunner.h"
#include "Core/DialogueContext.h"
#include "Data/DialogueDataAsset.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogueComponent, Log, All);

UDialogueComponent::UDialogueComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UDialogueComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cache Relationship Component
    AActor* Owner = GetOwner();
    if (Owner)
    {
        RelationshipComp = Owner->FindComponentByClass<URelationshipComponent>();
        MemoryComp = Owner->FindComponentByClass<UNPCMemoryComponent>();
    }
}

void UDialogueComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update cooldown
    if (DialogueCooldown > 0.0f)
    {
        DialogueCooldown -= DeltaTime;
    }
}

UTexture2D* UDialogueComponent::GetParticipantPortrait_Implementation() const
{
    return Portrait.LoadSynchronous();
}

bool UDialogueComponent::CanStartDialogue_Implementation(AActor* Initiator) const
{
    if (!bCanStartDialogue)
    {
        return false;
    }

    if (DialogueCooldown > 0.0f)
    {
        return false;
    }

    if (IsInDialogue())
    {
        return false;
    }

    // Check distance
    if (Initiator && GetOwner())
    {
        float Distance = FVector::Distance(GetOwner()->GetActorLocation(), Initiator->GetActorLocation());
        if (Distance > InteractionDistance)
        {
            return false;
        }
    }

    return true;
}

void UDialogueComponent::OnDialogueStarted_Implementation(UDialogueRunner* Runner)
{
    ActiveRunner = Runner;
    SetComponentTickEnabled(true);
    
    // Broadcast delegate (избегаем конфликта имен через явное указание)
    OnDialogueStarted.Broadcast(Runner);
    
    // Create memory of dialogue start if memory component exists
    if (MemoryComp && LastContext && LastContext->GetPlayer())
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("ActorName"), FText::FromString(LastContext->GetPlayer()->GetName()));
        
        MemoryComp->CreateMemory(
            EMemoryType::DialogueEvent,
            FText::Format(NSLOCTEXT("Dialogue", "StartedConversation", "Started conversation with {ActorName}"), Args),
            40.0f,
            EMemoryEmotion::Neutral,
            LastContext->GetPlayer()
        );
    }
}

void UDialogueComponent::OnDialogueEnded_Implementation()
{
    ActiveRunner = nullptr;
    SetComponentTickEnabled(false);
    
    // Broadcast delegate (избегаем конфликта имен через явное указание)
    OnDialogueEnded.Broadcast();
}

bool UDialogueComponent::StartDialogue(AActor* Initiator)
{
    if (!IDialogueParticipant::Execute_CanStartDialogue(this, Initiator))
    {
		UE_LOG(LogDialogueComponent, Warning, TEXT("StartDialogue - Cannot start dialogue now"));
        return false;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogDialogueComponent, Error, TEXT("StartDialogue - No valid World"));
        return false;
    }

    // Get DialogueService via interface instead of direct subsystem access
    IDialogueService* DialogueService = GetDialogueService();
    if (!DialogueService)
    {
        UE_LOG(LogDialogueComponent, Error, TEXT("StartDialogue - No DialogueService available"));
        return false;
    }

    // Create context
    UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
    Context->Initialize(); // v1.3.1: Initialize components
    Context->SetPlayer(Initiator);
    Context->SetNPC(GetOwner());

    // Select dialogue
    UDialogueDataAsset* SelectedDialogue = SelectDialogue(Context);
    if (!SelectedDialogue)
    {
        UE_LOG(LogDialogueComponent, Warning, TEXT("StartDialogue - No dialogue selected"));
        return false;
    }

    // Start dialogue through service interface
    UDialogueRunner* Runner = IDialogueService::Execute_StartDialogue(
        Cast<UObject>(DialogueService), 
        SelectedDialogue, 
        Initiator, 
        GetOwner()
    );

    if (Runner)
    {
        ActiveRunner = Runner;
      LastContext = Context;
        
        // Bind to end event
      Runner->OnDialogueEnded.AddDynamic(this, &UDialogueComponent::HandleDialogueEnded);
  
    // Bind to node changed event
    Runner->OnNodeEntered.AddDynamic(this, &UDialogueComponent::HandleNodeEntered);
     
        // Call interface method (через IDialogueParticipant)
        IDialogueParticipant::Execute_OnDialogueStarted(this, Runner);
        
        return true;
    }

    return false;
}

IDialogueService* UDialogueComponent::GetDialogueService() const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    // Try to get service from GameInstance subsystem
    UGameInstance* GameInstance = World->GetGameInstance();
    if (GameInstance)
    {
        UDialogueSubsystem* Subsystem = GameInstance->GetSubsystem<UDialogueSubsystem>();
        if (Subsystem && Subsystem->Implements<UDialogueService>())
        {
            return Cast<IDialogueService>(Subsystem);
        }
    }

    return nullptr;
}

UDialogueDataAsset* UDialogueComponent::SelectDialogue(const UDialogueSessionContext* Context)
{
    // Try conditional dialogues first
    for (const TPair<FName, TSoftObjectPtr<UDialogueDataAsset>>& Pair : ConditionalDialogues)
    {
        UDialogueDataAsset* ConditionalDialogue = Pair.Value.LoadSynchronous();
        if (ConditionalDialogue && RelationshipComp)
        {
            // Check if this dialogue's conditions are met
            float CurrentAffinity = RelationshipComp->GetAffinity();
            if (CurrentAffinity >= ConditionalDialogue->MinAffinityRequired)
            {
                return ConditionalDialogue;
            }
        }
    }

    // Fallback to default
    return DefaultDialogue.LoadSynchronous();
}

void UDialogueComponent::HandleDialogueEnded()
{
    if (ActiveRunner)
  {
        ActiveRunner->OnDialogueEnded.RemoveDynamic(this, &UDialogueComponent::HandleDialogueEnded);
   ActiveRunner->OnNodeEntered.RemoveDynamic(this, &UDialogueComponent::HandleNodeEntered);
        
        // Create memory of dialogue end if memory component exists
        if (MemoryComp && LastContext && LastContext->GetPlayer())
        {
      // Calculate importance based on duration
            FTimespan Duration = FDateTime::Now() - LastContext->GetState()->GetSessionStartTime();
     float Importance = FMath::Clamp(30.0f + Duration.GetTotalMinutes() * 5.0f, 30.0f, 80.0f);
  
            FFormatNamedArguments Args;
      Args.Add(TEXT("ActorName"), FText::FromString(LastContext->GetPlayer()->GetName()));
         
          MemoryComp->CreateMemory(
     EMemoryType::DialogueEvent,
       FText::Format(NSLOCTEXT("Dialogue", "HadConversation", "Had a conversation with {ActorName}"), Args),
   Importance,
              EMemoryEmotion::Neutral,
         LastContext->GetPlayer()
            );
    }
        
        ActiveRunner = nullptr;
   PreviousNode = nullptr;
    }

    // Call interface method
    IDialogueParticipant::Execute_OnDialogueEnded(this);
}

void UDialogueComponent::HandleNodeEntered(UDialogueNode* NewNode)
{
 // Broadcast node change event
 OnDialogueNodeChanged.Broadcast(NewNode, PreviousNode);
 
 // Update previous node
 PreviousNode = NewNode;
}
