# ?? Scalability & Future Enhancements Roadmap

> **Дата:** 2025-01-23  
> **Версия:** v1.3.1+  
> **Цель:** Конкретные рекомендации для масштабирования до крупной игры

---

## ?? Executive Summary

Данный документ содержит **конкретные, реализуемые рекомендации** для масштабирования Dialogue System от текущего состояния (10-20 NPC) до **крупной игры** (100+ NPC, множество одновременных диалогов, AI-генерация, мультиплеер).

---

## ?? Цели Масштабирования

### Краткосрочные (v1.4-1.5) - 3-6 месяцев
- ? Поддержка 50-100 NPC одновременно
- ? AI-интеграция (sentiment analysis + generation)
- ? Event system refactoring
- ? Advanced memory queries

### Среднесрочные (v1.6-2.0) - 6-12 месяцев
- ? Поддержка 200+ NPC
- ? Multiplayer dialogue sessions
- ? ECS-based architecture для NPC
- ? Visual dialogue editor

### Долгосрочные (v2.0+) - 12+ месяцев
- ? Поддержка 500+ NPC (open world scale)
- ? Full AI conversation simulation
- ? Cross-platform save/load (cloud sync)
- ? Modding support

---

## 1. Масштабирование для Множества NPC

### Проблема: Current Architecture

```cpp
// Текущий подход: Each NPC has full component instances
ANPC
??? UDialogueComponent (16KB)
??? URelationshipComponent (8KB)
??? UNPCMemoryComponent (50KB+ depending on memories)

Total per NPC: ~74KB
100 NPCs: ~7.4MB
500 NPCs: ~37MB (too much!)
```

### 1.1. Shared Data Architecture

**Решение:** Разделить static (shared) и dynamic (per-instance) данные.

#### Implementation

```cpp
// 1. Create NPC Profile DataAsset (shared between instances)
UCLASS()
class UNPCProfileAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Unique profile ID */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ProfileId;
    
    /** Display name */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;
    
    /** Available dialogues */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TSoftObjectPtr<UDialogueDataAsset>> AvailableDialogues;
    
    /** Personality preset */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
    UPersonalityPreset* Personality;
    
    /** Default relationship values */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRelationshipDefaults DefaultRelationship;

    /** Memory settings */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMemorySettings MemorySettings;
    
    /** Visual data */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> Portrait;
    
    /** Audio data */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<USoundBase> VoiceSet;
};

// 2. Create lightweight instance data
USTRUCT(BlueprintType)
struct FNPCInstanceData
{
    GENERATED_BODY()
    
    /** Reference to profile */
    UPROPERTY()
  FName ProfileId;
    
    /** Current relationship values (only deltas from default) */
    UPROPERTY()
    FRelationshipState CurrentRelationship;
    
    /** Active memories (only unique to this instance) */
    UPROPERTY()
    TArray<FNPCMemoryEntry> UniqueMemories;
    
  /** Active dialogue session (if any) */
    UPROPERTY()
    FGuid ActiveDialogueSession;
    
    /** Last interaction timestamp */
    UPROPERTY()
    FDateTime LastInteraction;
};

// 3. NPC Manager Subsystem (centralized management)
UCLASS()
class UNPCManagerSubsystem : public UGameInstanceSubsystem
{
  GENERATED_BODY()

public:
    /** Get or create NPC instance data */
    UFUNCTION(BlueprintCallable)
    FNPCInstanceData* GetNPCData(AActor* NPC);
    
    /** Get NPC profile (cached) */
    UFUNCTION(BlueprintCallable)
    UNPCProfileAsset* GetNPCProfile(FName ProfileId);
    
    /** Tick all NPC data (decay, etc.) */
    virtual void Tick(float DeltaTime) override;
    
protected:
    /** All NPC instance data (sparse array) */
    UPROPERTY()
    TMap<TWeakObjectPtr<AActor>, FNPCInstanceData> NPCInstances;
    
    /** Cached profiles */
    UPROPERTY()
    TMap<FName, UNPCProfileAsset*> CachedProfiles;
    
    /** Batch update settings */
    UPROPERTY(Config)
    float BatchUpdateInterval = 1.0f;
    
float TimeSinceLastBatchUpdate = 0.0f;
};

// 4. Updated NPC Actor (minimal memory footprint)
UCLASS()
class ANPC : public ACharacter, public IDialogueParticipant
{
    GENERATED_BODY()

public:
    /** Profile reference (shared) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UNPCProfileAsset> Profile;
    
    /** Get instance data from manager */
    FNPCInstanceData* GetInstanceData() const
    {
        UNPCManagerSubsystem* Manager = GetWorld()->GetGameInstance()
        ->GetSubsystem<UNPCManagerSubsystem>();
        return Manager ? Manager->GetNPCData(const_cast<ANPC*>(this)) : nullptr;
    }
    
    /** Get profile (cached in manager) */
    UNPCProfileAsset* GetProfile() const
    {
     UNPCManagerSubsystem* Manager = GetWorld()->GetGameInstance()
         ->GetSubsystem<UNPCManagerSubsystem>();
        return Manager ? Manager->GetNPCProfile(ProfileId) : nullptr;
    }
    
protected:
/** Cached profile ID for quick lookups */
    UPROPERTY()
    FName ProfileId;
};
```

#### Benefits

```
Memory Savings:
????????????????????????????????????????

Before (per NPC):
- UDialogueComponent: 16KB
- URelationshipComponent: 8KB
- UNPCMemoryComponent: 50KB
Total: 74KB

After (per NPC):
- FNPCInstanceData: 2KB (only deltas)
- Profile reference: 8 bytes
Total: ~2KB

Savings: 97% reduction!

100 NPCs:
Before: 7.4MB
After: 200KB + shared profiles (~500KB)
Total: ~700KB (90% savings!)

500 NPCs:
Before: 37MB
After: 1MB + shared profiles (~500KB)
Total: ~1.5MB (96% savings!)
```

---

### 1.2. Batch Processing System

**Цель:** Обновлять множество NPC эффективно.

```cpp
// 1. Batch Update System
UCLASS()
class UNPCBatchUpdateSystem : public UTickableGameObject
{
public:
    /** Register NPC for batch updates */
    void RegisterNPC(FNPCInstanceData* Data);
    
    /** Unregister NPC */
 void UnregisterNPC(FNPCInstanceData* Data);
    
    /** Tick implementation */
    virtual void Tick(float DeltaTime) override
    {
     TimeSinceLastUpdate += DeltaTime;
    
      if (TimeSinceLastUpdate >= UpdateInterval)
    {
            BatchUpdateRelationships(TimeSinceLastUpdate);
            BatchUpdateMemories(TimeSinceLastUpdate);

          TimeSinceLastUpdate = 0.0f;
        }
    }
    
protected:
    void BatchUpdateRelationships(float DeltaTime)
    {
        // Parallel processing
  ParallelFor(RegisteredNPCs.Num(), [&](int32 Index)
    {
     FNPCInstanceData* Data = RegisteredNPCs[Index];
   
     // Decay affinity
 Data->CurrentRelationship.Affinity = FMath::FInterpTo(
    Data->CurrentRelationship.Affinity,
   Data->CurrentRelationship.AffinityDecayTarget,
      DeltaTime,
        Data->CurrentRelationship.AffinityDecayRate
            );
       
    // Decay other stats...
        });
    }
    
    void BatchUpdateMemories(float DeltaTime)
    {
        // Process memory decay in batches
        const int32 BatchSize = 100;
        
        for (int32 Batch = 0; Batch < RegisteredNPCs.Num(); Batch += BatchSize)
        {
int32 End = FMath::Min(Batch + BatchSize, RegisteredNPCs.Num());
          
         ParallelFor(End - Batch, [&](int32 LocalIndex)
     {
          int32 Index = Batch + LocalIndex;
  FNPCInstanceData* Data = RegisteredNPCs[Index];
      
        ProcessMemoryDecay(Data, DeltaTime);
         });
 }
    }
    
    void ProcessMemoryDecay(FNPCInstanceData* Data, float DeltaTime)
    {
        for (FNPCMemoryEntry& Memory : Data->UniqueMemories)
        {
    // Calculate age
  float AgeDays = (FDateTime::Now() - Memory.Timestamp).GetTotalDays();
    
            // Apply decay
      float DecayAmount = BaseDecayRate * DeltaTime;
          
   // Emotional memories decay slower
            if (IsEmotionalMemory(Memory.Emotion))
            {
     DecayAmount *= EmotionalMemoryBonus;
          }
          
     Memory.Freshness = FMath::Max(0.0f, Memory.Freshness - DecayAmount);

            // Remove if too old and unimportant
   if (Memory.Freshness < MinImportanceThreshold)
 {
          // Mark for removal
      }
        }
        
      // Remove marked memories
      Data->UniqueMemories.RemoveAll([](const FNPCMemoryEntry& M) {
   return M.Freshness < MinImportanceThreshold;
  });
    }
    
    TArray<FNPCInstanceData*> RegisteredNPCs;
    float UpdateInterval = 1.0f;
    float TimeSinceLastUpdate = 0.0f;
};
```

#### Performance Impact

```
Single-threaded (current):
- 100 NPCs ? 1ms per update = 100ms per tick
- At 60 FPS: 6 seconds per frame! (unplayable)

Batch + Parallel:
- 100 NPCs / 8 threads = 12.5 NPCs per thread
- 12.5 ? 1ms = 12.5ms per batch
- Total: ~15ms (including overhead)
- At 60 FPS: <1ms per frame ?

500 NPCs:
- Single-threaded: 500ms (impossible)
- Batch + Parallel: ~75ms (playable at 30 FPS)
```

---

### 1.3. Spatial Partitioning для NPC

**Цель:** Обновлять только близких к игроку NPC.

```cpp
// 1. NPC Spatial Grid
UCLASS()
class UNPCSpatialGrid : public UObject
{
public:
    /** Initialize grid */
void Initialize(FVector WorldMin, FVector WorldMax, int32 GridSize)
    {
   CellSize = (WorldMax - WorldMin) / GridSize;
        GridMin = WorldMin;
        Cells.SetNum(GridSize * GridSize * GridSize);
    }
    
    /** Add NPC to grid */
    void AddNPC(AActor* NPC)
    {
        FIntVector CellIndex = GetCellIndex(NPC->GetActorLocation());
        Cells[GetCellArrayIndex(CellIndex)].Add(NPC);
    }
    
    /** Remove NPC from grid */
    void RemoveNPC(AActor* NPC)
    {
   // Remove from all cells (slow, but rare operation)
        for (FNPCCell& Cell : Cells)
        {
            Cell.Remove(NPC);
        }
    }
    
    /** Update NPC position */
    void UpdateNPC(AActor* NPC, FVector OldLocation, FVector NewLocation)
    {
FIntVector OldCell = GetCellIndex(OldLocation);
     FIntVector NewCell = GetCellIndex(NewLocation);
        
        if (OldCell != NewCell)
        {
            // Move between cells
     Cells[GetCellArrayIndex(OldCell)].Remove(NPC);
      Cells[GetCellArrayIndex(NewCell)].Add(NPC);
     }
    }
    
    /** Get NPCs near player */
    TArray<AActor*> GetNPCsNearPlayer(FVector PlayerLocation, float Radius)
    {
        TArray<AActor*> Result;
        
        // Calculate search area
        FIntVector MinCell = GetCellIndex(PlayerLocation - FVector(Radius));
        FIntVector MaxCell = GetCellIndex(PlayerLocation + FVector(Radius));
 
  // Iterate cells
  for (int32 X = MinCell.X; X <= MaxCell.X; ++X)
        {
   for (int32 Y = MinCell.Y; Y <= MaxCell.Y; ++Y)
            {
    for (int32 Z = MinCell.Z; Z <= MaxCell.Z; ++Z)
 {
             FIntVector CellIndex(X, Y, Z);
    int32 ArrayIndex = GetCellArrayIndex(CellIndex);
   
 if (Cells.IsValidIndex(ArrayIndex))
            {
       for (AActor* NPC : Cells[ArrayIndex])
           {
     if (FVector::Dist(NPC->GetActorLocation(), PlayerLocation) <= Radius)
   {
       Result.Add(NPC);
     }
          }
    }
       }
     }
        }
        
  return Result;
  }
    
protected:
    FIntVector GetCellIndex(FVector WorldLocation) const
    {
        FVector Relative = (WorldLocation - GridMin) / CellSize;
    return FIntVector(
            FMath::FloorToInt(Relative.X),
            FMath::FloorToInt(Relative.Y),
            FMath::FloorToInt(Relative.Z)
      );
    }
    
    int32 GetCellArrayIndex(FIntVector CellIndex) const
    {
        return CellIndex.X + CellIndex.Y * GridSize + CellIndex.Z * GridSize * GridSize;
    }
    
    struct FNPCCell
    {
        TArray<TWeakObjectPtr<AActor>> NPCs;
        
        void Add(AActor* NPC) { NPCs.AddUnique(NPC); }
        void Remove(AActor* NPC) { NPCs.Remove(NPC); }
    };
    
    TArray<FNPCCell> Cells;
FVector GridMin;
    FVector CellSize;
    int32 GridSize;
};

// 2. Update only nearby NPCs
void UNPCManagerSubsystem::Tick(float DeltaTime)
{
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;
    
    // Get NPCs near player (e.g., 5000 units)
    TArray<AActor*> NearbyNPCs = SpatialGrid->GetNPCsNearPlayer(
   Player->GetActorLocation(),
        5000.0f
    );
    
  // Update only nearby NPCs
    for (AActor* NPC : NearbyNPCs)
    {
        FNPCInstanceData* Data = GetNPCData(NPC);
  if (Data)
        {
            UpdateNPCData(Data, DeltaTime);
        }
    }

    // Periodically update distant NPCs (less frequently)
    if (FMath::FRand() < 0.01f) // 1% chance per tick
    {
        UpdateRandomDistantNPC(DeltaTime);
    }
}
```

#### Performance Impact

```
Without Spatial Partitioning:
- Check all 500 NPCs every tick
- 500 checks ? 0.1ms = 50ms

With Spatial Partitioning:
- Check only ~20 nearby NPCs
- 20 checks ? 0.1ms = 2ms
- 96% reduction!

Spatial grid overhead:
- Build grid: ~1ms (one-time)
- Update NPC position: ~0.01ms per NPC
- Query grid: ~0.5ms
- Total: <1ms
```

---

## 2. AI Integration

### 2.1. Sentiment Analysis (v1.5)

**Цель:** Анализировать эмоциональный тон выборов игрока.

```cpp
// 1. Sentiment Analyzer Interface
UINTERFACE()
class USentimentAnalyzerInterface : public UInterface
{
    GENERATED_BODY()
};

class ISentimentAnalyzerInterface
{
    GENERATED_BODY()
    
public:
    /** Analyze text sentiment */
    UFUNCTION(BlueprintNativeEvent)
    void AnalyzeSentiment(
        const FString& Text,
   const FOnSentimentAnalyzed& Callback
    );
};

// 2. OpenAI Sentiment Analyzer
UCLASS()
class UOpenAISentimentAnalyzer : public UObject, public ISentimentAnalyzerInterface
{
    GENERATED_BODY()
    
public:
    virtual void AnalyzeSentiment_Implementation(
        const FString& Text,
      const FOnSentimentAnalyzed& Callback
    ) override
    {
        // Build simple prompt
        FString Prompt = FString::Printf(TEXT(
            "Analyze the sentiment of this text and respond with one word: "
            "Positive, Negative, Neutral, Aggressive, Friendly, Sarcastic, or Sad.\n\n"
            "Text: \"%s\"\n\n"
            "Sentiment:"
        ), *Text);
        
  // Send request
        SendAIRequest(Prompt, [Callback](const FString& Response)
        {
            ESentiment Sentiment = ParseSentiment(Response);
  
          // Execute callback on game thread
          AsyncTask(ENamedThreads::GameThread, [Callback, Sentiment]()
   {
 Callback.ExecuteIfBound(Sentiment, true);
            });
        });
    }
    
protected:
    ESentiment ParseSentiment(const FString& Response)
    {
        FString Trimmed = Response.TrimStartAndEnd().ToLower();
        
        if (Trimmed.Contains("positive") || Trimmed.Contains("friendly"))
    return ESentiment::Positive;
        else if (Trimmed.Contains("negative") || Trimmed.Contains("sad"))
            return ESentiment::Negative;
        else if (Trimmed.Contains("aggressive"))
return ESentiment::Aggressive;
        else if (Trimmed.Contains("sarcastic"))
            return ESentiment::Sarcastic;
        else
            return ESentiment::Neutral;
    }
};

// 3. Integration with DialogueRunner
void UDialogueRunner::SelectChoice(int32 ChoiceIndex)
{
    // Get selected choice
FDialogueChoice Choice = CurrentNode->Choices[ChoiceIndex];
    
    // Analyze sentiment (if AI enabled)
    if (AIProvider && bEnableSentimentAnalysis)
    {
    FOnSentimentAnalyzed Callback;
        Callback.BindUObject(this, &UDialogueRunner::HandleSentimentAnalyzed, Choice);
        
        AIProvider->AnalyzeSentiment(Choice.Text.ToString(), Callback);
    }
    else
    {
        // Process normally
        ProcessChoice(Choice);
    }
}

void UDialogueRunner::HandleSentimentAnalyzed(
    ESentiment Sentiment,
 bool bSuccess,
    FDialogueChoice Choice
)
{
    if (bSuccess)
    {
        // Apply sentiment-based relationship changes
        AActor* NPC = Context->GetNPC();
        URelationshipComponent* RelComp = NPC->FindComponentByClass<URelationshipComponent>();
        
        if (RelComp)
      {
            switch (Sentiment)
   {
      case ESentiment::Positive:
         case ESentiment::Friendly:
  RelComp->ModifyAffinity(5.0f);
         break;
           
     case ESentiment::Negative:
  case ESentiment::Sad:
       RelComp->ModifyAffinity(-5.0f);
        break;
  
   case ESentiment::Aggressive:
         RelComp->ModifyFear(10.0f);
            RelComp->ModifyAffinity(-10.0f);
        break;
                    
      case ESentiment::Sarcastic:
    // Depends on NPC personality
    if (RelComp->GetPersonality().AppreciatesSarcasm)
         RelComp->ModifyAffinity(5.0f);
     else
       RelComp->ModifyAffinity(-5.0f);
            break;
          }
        }
        
   // Create memory with sentiment
        UNPCMemoryComponent* MemComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
    if (MemComp)
        {
       EMemoryEmotion MemoryEmotion = ConvertSentimentToEmotion(Sentiment);
     
            MemComp->CreateMemory(
        EMemoryType::PlayerChoice,
          FText::Format(LOCTEXT("PlayerSaid", "Player said: {0}"), Choice.Text),
     CalculateImportance(Sentiment),
       MemoryEmotion,
Context->GetPlayer()
        );
 }
    }
    
 // Continue with choice processing
    ProcessChoice(Choice);
}
```

#### Benefits

```
Without Sentiment Analysis:
- Designer manually sets affinity changes
- Limited granularity
- Can't react to tone

With Sentiment Analysis:
- Automatic emotional reaction
- Contextual relationship changes
- NPC personality affects reaction
- More immersive interactions
```

---

### 2.2. Dynamic Response Generation (v1.5)

**Цель:** Генерировать уникальные ответы NPC на основе контекста.

```cpp
// 1. Response Generator
UCLASS()
class UAIResponseGenerator : public UObject
{
public:
    /** Generate NPC response */
    void GenerateResponse(
        UDialogueSessionContext* Context,
        const FString& PlayerMessage,
   const FOnResponseGenerated& Callback
    )
    {
      // Build rich context
        FString ContextData = BuildContextData(Context);
      
        // Build prompt
        FString Prompt = FString::Printf(TEXT(
   "You are %s, an NPC in a fantasy RPG.\n\n"
  "Your personality:\n%s\n\n"
            "Current context:\n%s\n\n"
            "Recent conversation:\n%s\n\n"
 "Player just said: \"%s\"\n\n"
         "Respond in character (1-2 sentences, natural dialogue):"
        ),
            *GetNPCName(Context),
            *GetPersonalityDescription(Context),
          *ContextData,
            *GetConversationHistory(Context, 5),
            *PlayerMessage
        );
    
    // Send AI request
        SendAIRequest(Prompt, [Callback, Context](const FString& Response)
        {
      // Parse response
     FText ResponseText = FText::FromString(Response.TrimStartAndEnd());
            
       // Execute callback on game thread
            AsyncTask(ENamedThreads::GameThread, [Callback, ResponseText]()
     {
    Callback.ExecuteIfBound(ResponseText, true);
            });
        });
    }
    
protected:
    FString BuildContextData(UDialogueSessionContext* Context) const
    {
        TArray<FString> Parts;
        
  // Relationship
        AActor* NPC = Context->GetNPC();
      if (URelationshipComponent* RelComp = NPC->FindComponentByClass<URelationshipComponent>())
{
  Parts.Add(FString::Printf(TEXT("Relationship with player: %s (Affinity: %.0f)"),
*RelComp->GetRelationshipRank().ToString(),
      RelComp->GetAffinity()
            ));
 }
     
        // Recent memories
        if (UNPCMemoryComponent* MemComp = NPC->FindComponentByClass<UNPCMemoryComponent>())
        {
            TArray<FNPCMemoryEntry> Recent = MemComp->GetRecentMemories(3);
            for (const FNPCMemoryEntry& Memory : Recent)
   {
     Parts.Add(FString::Printf(TEXT("I remember: %s"),
         *Memory.Description.ToString()
                ));
      }
        }
        
        // Active quests
 // ... add quest context
        
        // World state
        // ... add relevant world info
        
  return FString::Join(Parts, TEXT("\n"));
    }
    
    FString GetPersonalityDescription(UDialogueSessionContext* Context) const
    {
        AActor* NPC = Context->GetNPC();
      URelationshipComponent* RelComp = NPC->FindComponentByClass<URelationshipComponent>();
        
   if (RelComp)
   {
     const FNPCPersonality& Personality = RelComp->GetPersonality();
  
            return FString::Printf(TEXT(
      "- Mood: %s\n"
            "- Openness: %.1f/10\n"
                "- Friendliness: %.1f/10\n"
       "- Patience: %.1f/10"
),
         *UEnum::GetValueAsString(Personality.Mood),
                Personality.Openness,
         Personality.Friendliness,
        Personality.Patience
     );
 }
        
      return TEXT("Neutral personality");
    }
};

// 2. Integration with DialogueRunner
void UDialogueRunner::ProcessDynamicNode(UDialogueNode* Node)
{
    if (Node->bUseDynamicResponse && AIResponseGenerator)
    {
     // Get last player message
        FString PlayerMessage = GetLastPlayerChoice();

 // Generate response
        FOnResponseGenerated Callback;
        Callback.BindUObject(this, &UDialogueRunner::HandleResponseGenerated, Node);
        
        AIResponseGenerator->GenerateResponse(
   Context,
        PlayerMessage,
       Callback
        );
        
        // Show "thinking" indicator in UI
        OnAIThinking.Broadcast();
    }
    else
    {
        // Use static text
        ProcessStaticNode(Node);
    }
}

void UDialogueRunner::HandleResponseGenerated(
    const FText& Response,
    bool bSuccess,
    UDialogueNode* Node
)
{
    // Hide "thinking" indicator
    OnAIThinkingComplete.Broadcast();
    
    if (bSuccess)
    {
        // Update node with generated response
        Node->SpeakerText = Response;
      
     // Process node normally
  ProcessNode(Node);
    }
    else
    {
    // Fallback to static text or error handling
 UE_LOG(LogDialogue, Warning, TEXT("AI response generation failed, using fallback"));
  ProcessStaticNode(Node);
    }
}
```

#### Usage Example

```
Designer creates hybrid dialogue:

NODE GreetingDynamic
  TYPE DynamicResponse
  FALLBACK_TEXT "Hello there!"
  
  CHOICE "Tell me about yourself"
    ? AI generates unique response based on:
      - NPC personality
      - Relationship with player
      - Recent memories
      - World state
  
  CHOICE "Goodbye"
    GOTO End

Result:
- First playthrough: "Ah, a new face! I'm the village blacksmith..."
- Second playthrough (different relationship): "You again? Still haven't paid for that sword..."
- Third playthrough (after quest): "My friend! Thanks again for helping..."
```

---

### 2.3. Conversation Memory (Long-term Context)

**Цель:** AI запоминает предыдущие разговоры для консистентности.

```cpp
// 1. Conversation Memory Store
UCLASS()
class UConversationMemoryStore : public USaveGame
{
    GENERATED_BODY()
    
public:
    /** Store conversation summary */
    void StoreConversation(
 FName NPCId,
        FName DialogueId,
        const FString& Summary,
      const TArray<FString>& KeyPoints
    )
    {
        FConversationMemory Memory;
        Memory.NPCId = NPCId;
    Memory.DialogueId = DialogueId;
        Memory.Summary = Summary;
   Memory.KeyPoints = KeyPoints;
     Memory.Timestamp = FDateTime::Now();
        
        ConversationHistory.Add(Memory);
  
        // Keep only recent N conversations
        if (ConversationHistory.Num() > MaxConversations)
        {
            ConversationHistory.RemoveAt(0);
    }
     
        // Auto-save
 UGameplayStatics::SaveGameToSlot(this, TEXT("ConversationMemory"), 0);
    }
    
  /** Get conversation history with NPC */
    TArray<FConversationMemory> GetConversationHistory(FName NPCId) const
    {
        return ConversationHistory.FilterByPredicate([NPCId](const FConversationMemory& M) {
            return M.NPCId == NPCId;
        });
    }
 
    /** Build context string for AI */
    FString BuildContextString(FName NPCId, int32 MaxConversations = 5) const
    {
      TArray<FConversationMemory> History = GetConversationHistory(NPCId);
 
      // Take most recent
        int32 StartIndex = FMath::Max(0, History.Num() - MaxConversations);
 TArray<FString> Parts;
  
        for (int32 i = StartIndex; i < History.Num(); ++i)
     {
            const FConversationMemory& Memory = History[i];
            
 Parts.Add(FString::Printf(TEXT("Previous conversation (%s):\nSummary: %s\nKey points: %s"),
          *Memory.Timestamp.ToString(),
      *Memory.Summary,
        *FString::Join(Memory.KeyPoints, TEXT(", "))
            ));
        }
        
 return FString::Join(Parts, TEXT("\n\n"));
    }
    
protected:
    UPROPERTY()
    TArray<FConversationMemory> ConversationHistory;
    
    UPROPERTY()
    int32 MaxConversations = 100;
};

// 2. Summarize conversation at end
void UDialogueRunner::EndDialogue()
{
    // ... existing code ...
 
    // If AI enabled, summarize conversation
    if (AIProvider && bEnableConversationMemory)
    {
        SummarizeConversation();
    }
}

void UDialogueRunner::SummarizeConversation()
{
    // Build conversation text
    TArray<FString> ConversationLines;
    for (const FDialogueHistoryEntry& Entry : Context->GetState()->GetConversationHistory())
    {
   ConversationLines.Add(FString::Printf(TEXT("%s: %s"),
            *Entry.SpeakerName.ToString(),
  *Entry.DialogueText.ToString()
        ));
    }
    
    FString ConversationText = FString::Join(ConversationLines, TEXT("\n"));
    
    // Build summary prompt
  FString Prompt = FString::Printf(TEXT(
        "Summarize this conversation in 2-3 sentences and extract 3-5 key points:\n\n"
   "%s\n\n"
      "Summary:\n"
     "Key points:\n"
  ), *ConversationText);
    
    // Send AI request
  SendAIRequest(Prompt, [this](const FString& Response)
    {
        // Parse summary and key points
        TArray<FString> Lines;
        Response.ParseIntoArrayLines(Lines);
        
        FString Summary;
        TArray<FString> KeyPoints;

        bool bInKeyPoints = false;
        for (const FString& Line : Lines)
        {
       if (Line.Contains("Key points:"))
     {
                bInKeyPoints = true;
            }
            else if (bInKeyPoints)
 {
                if (!Line.IsEmpty())
    {
     KeyPoints.Add(Line.TrimStartAndEnd());
         }
       }
            else if (!Line.IsEmpty() && !Line.Contains("Summary:"))
 {
     Summary += Line + TEXT(" ");
            }
        }
        
 // Store in memory
        UConversationMemoryStore* MemoryStore = Cast<UConversationMemoryStore>(
            UGameplayStatics::LoadGameFromSlot(TEXT("ConversationMemory"), 0)
    );
        
        if (!MemoryStore)
      {
            MemoryStore = NewObject<UConversationMemoryStore>();
 }
        
        MemoryStore->StoreConversation(
   GetNPCId(),
            CurrentDialogueId,
 Summary,
            KeyPoints
        );
    });
}
```

#### Benefits

```
Without Conversation Memory:
- AI generates responses from scratch
- May contradict previous conversations
- No continuity between sessions

With Conversation Memory:
- AI references past conversations
- Consistent character behavior
- Long-term narrative continuity
- Example: "Last time you promised to help, why didn't you?"
```

---

## 3. Event System Refactoring (v1.4)

### Проблема: Дублирующиеся события

```cpp
// Current state: Too many overlapping delegates

// DialogueRunner
FOnDialogueStartedSignature OnDialogueStarted;
FOnDialogueEndedSignature OnDialogueEnded;
FOnNodeChangedSignature OnNodeChanged;
// ... 10+ more

// DialogueComponent
FOnDialogueStartedSignature OnDialogueStarted; // duplicate!
FOnDialogueEndedSignature OnDialogueEnded; // duplicate!
// ...

// DialogueSubsystem
FOnAnyDialogueStartedSignature OnAnyDialogueStarted; // another duplicate!
FOnAnyDialogueEndedSignature OnAnyDialogueEnded; // another duplicate!
// ...
```

### Решение: Centralized Event Bus

```cpp
// 1. Dialogue Event Bus
UCLASS()
class UDialogueEventBus : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    //??????????????????????????????????????????????????????????????????????
  // Session Events
    //??????????????????????????????????????????????????????????????????????
    
    /** Dialogue session started */
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FDialogueStarted, 
        UDialogueRunner* /* Runner */,
    AActor* /* Player */,
   AActor* /* NPC */
    );
    FDialogueStarted OnDialogueStarted;
  
    /** Dialogue session ended */
    DECLARE_MULTICAST_DELEGATE_TwoParams(FDialogueEnded,
      AActor* /* Player */,
 AActor* /* NPC */
    );
    FDialogueEnded OnDialogueEnded;
    
    //??????????????????????????????????????????????????????????????????????
    // Node Events
    //??????????????????????????????????????????????????????????????????????
    
    /** Node changed */
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FNodeChanged,
        UDialogueNode* /* NewNode */,
        UDialogueNode* /* OldNode */,
      UDialogueRunner* /* Runner */
  );
    FNodeChanged OnNodeChanged;
    
    /** Choices available */
    DECLARE_MULTICAST_DELEGATE_TwoParams(FChoicesAvailable,
        const TArray<FDialogueChoice>& /* Choices */,
        UDialogueRunner* /* Runner */
    );
    FChoicesAvailable OnChoicesAvailable;
    
    /** Choice selected */
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FChoiceSelected,
  int32 /* ChoiceIndex */,
        const FDialogueChoice& /* Choice */,
        UDialogueRunner* /* Runner */
    );
    FChoiceSelected OnChoiceSelected;
    
    //??????????????????????????????????????????????????????????????????????
    // State Events
    //??????????????????????????????????????????????????????????????????????

    /** State changed */
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FStateChanged,
        EDialogueState /* OldState */,
        EDialogueState /* NewState */,
        UDialogueRunner* /* Runner */
  );
    FStateChanged OnStateChanged;
    
    //??????????????????????????????????????????????????????????????????????
    // Command Events
    //??????????????????????????????????????????????????????????????????????
    
    /** Command executed */
    DECLARE_MULTICAST_DELEGATE_TwoParams(FCommandExecuted,
        UDialogueCommand* /* Command */,
        bool /* bSuccess */
    );
    FCommandExecuted OnCommandExecuted;
    
    /** Command undone */
    DECLARE_MULTICAST_DELEGATE_OneParam(FCommandUndone,
        UDialogueCommand* /* Command */
  );
    FCommandUndone OnCommandUndone;
    
    //??????????????????????????????????????????????????????????????????????
    // Effect/Condition Events
    //??????????????????????????????????????????????????????????????????????
    
    /** Effect applied */
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FEffectApplied,
        const FDialogueEffect& /* Effect */,
      UDialogueSessionContext* /* Context */,
      bool /* bSuccess */
    );
    FEffectApplied OnEffectApplied;
    
    /** Condition evaluated */
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FConditionEvaluated,
        const FDialogueCondition& /* Condition */,
    UDialogueSessionContext* /* Context */,
      bool /* Result */
    );
    FConditionEvaluated OnConditionEvaluated;
    
    //??????????????????????????????????????????????????????????????????????
    // Utility
    //??????????????????????????????????????????????????????????????????????
    
    /** Get global event bus */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Events", meta = (WorldContext = "WorldContextObject"))
    static UDialogueEventBus* Get(UObject* WorldContextObject)
    {
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
{
            if (UGameInstance* GI = World->GetGameInstance())
    {
         return GI->GetSubsystem<UDialogueEventBus>();
            }
        }
        return nullptr;
    }
};

// 2. Update DialogueRunner to use Event Bus
void UDialogueRunner::StartDialogue()
{
    // ... existing code ...
    
    // Broadcast via Event Bus instead of local delegate
    UDialogueEventBus* EventBus = UDialogueEventBus::Get(this);
    if (EventBus)
    {
        EventBus->OnDialogueStarted.Broadcast(this, Context->GetPlayer(), Context->GetNPC());
    }
    
    // DEPRECATED: Keep for backward compatibility (v1.4)
    OnDialogueStarted.Broadcast(this);
}

// 3. UI subscribes to Event Bus
void UDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Subscribe to global events
    UDialogueEventBus* EventBus = UDialogueEventBus::Get(this);
    if (EventBus)
    {
      EventBus->OnChoicesAvailable.AddUObject(this, &UDialogueWidget::HandleChoicesAvailable);
        EventBus->OnNodeChanged.AddUObject(this, &UDialogueWidget::HandleNodeChanged);
        EventBus->OnDialogueEnded.AddUObject(this, &UDialogueWidget::HandleDialogueEnded);
    }
}

void UDialogueWidget::BeginDestroy()
{
    // Unsubscribe
    UDialogueEventBus* EventBus = UDialogueEventBus::Get(this);
    if (EventBus)
    {
        EventBus->OnChoicesAvailable.RemoveAll(this);
      EventBus->OnNodeChanged.RemoveAll(this);
        EventBus->OnDialogueEnded.RemoveAll(this);
    }
    
    Super::BeginDestroy();
}
```

#### Benefits

```
Before:
- 30+ duplicate delegates across classes
- Difficult to track all subscribers
- Memory overhead from duplicates
- Hard to add cross-system listeners

After:
- Single source of truth
- Easy to add global listeners
- Cleaner architecture
- Better debugging (can log all events in one place)

Memory Savings:
- Each delegate: ~32 bytes
- 30 duplicates ? 32 bytes = ~1KB per dialogue
- With 10 active dialogues: ~10KB saved
```

---

## 4. Visual Dialogue Editor (v1.6)

### Цель: Drag-and-drop editor для дизайнеров

```cpp
// 1. Dialogue Graph Node
UCLASS()
class UDialogueGraphNode : public UEdGraphNode
{
    GENERATED_BODY()
    
public:
    /** Underlying dialogue node data */
    UPROPERTY()
    UDialogueNode* DialogueNode;
    
    /** Visual position */
    FVector2D NodePosition;
  
    /** Connected nodes */
    UPROPERTY()
    TArray<UDialogueGraphNode*> ChildNodes;
  
    /** Parent nodes */
    UPROPERTY()
    TArray<UDialogueGraphNode*> ParentNodes;
};

// 2. Dialogue Graph
UCLASS()
class UDialogueGraph : public UEdGraph
{
    GENERATED_BODY()
    
public:
    /** Root node */
  UPROPERTY()
    UDialogueGraphNode* RootNode;
    
    /** All nodes */
    UPROPERTY()
    TArray<UDialogueGraphNode*> AllNodes;
    
    /** Associated dialogue asset */
    UPROPERTY()
    UDialogueDataAsset* DialogueAsset;
    
    /** Compile graph to dialogue asset */
    void CompileToAsset();
};

// 3. Editor Implementation (UE5 Editor Module)
class FDialogueEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
    // Register asset actions
        IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    
        TSharedPtr<IAssetTypeActions> Action = MakeShareable(new FAssetTypeActions_DialogueDataAsset());
        AssetTools.RegisterAssetTypeActions(Action.ToSharedRef());
    }
};

// 4. Graph Editor Widget
class SDialogueGraphEditor : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SDialogueGraphEditor) {}
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs, UDialogueGraph* InGraph)
    {
        Graph = InGraph;
      
        // Create graph editor
        SGraphEditor::FGraphEditorEvents Events;
        Events.OnNodeDoubleClicked = FOnNodeDoubleClicked::CreateSP(this, &SDialogueGraphEditor::OnNodeDoubleClicked);
        Events.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &SDialogueGraphEditor::OnSelectionChanged);
      
 GraphEditor = SNew(SGraphEditor)
   .GraphToEdit(Graph)
            .GraphEvents(Events)
            .ShowGraphStateOverlay(false);
        
        ChildSlot
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
    .FillHeight(1.0f)
   [
    GraphEditor.ToSharedRef()
    ]
            + SVerticalBox::Slot()
         .AutoHeight()
[
      // Toolbar with compile, save, test buttons
       CreateToolbar()
   ]
   ];
    }
    
protected:
    void OnNodeDoubleClicked(UEdGraphNode* Node)
    {
        // Open node details editor
        ShowNodeDetailsPanel(Cast<UDialogueGraphNode>(Node));
    }
    
    void OnSelectionChanged(const TSet<UObject*>& Selection)
    {
        // Update details panel
  }
    
TSharedRef<SWidget> CreateToolbar()
    {
   FToolBarBuilder ToolbarBuilder(TSharedPtr<FUICommandList>(), FMultiBoxCustomization::None);
        
        ToolbarBuilder.AddToolBarButton(
      FUIAction(FExecuteAction::CreateSP(this, &SDialogueGraphEditor::CompileGraph)),
            NAME_None,
      LOCTEXT("Compile", "Compile"),
    LOCTEXT("CompileTooltip", "Compile graph to dialogue asset"),
  FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Build")
  );
        
        ToolbarBuilder.AddToolBarButton(
            FUIAction(FExecuteAction::CreateSP(this, &SDialogueGraphEditor::TestDialogue)),
      NAME_None,
          LOCTEXT("Test", "Test"),
   LOCTEXT("TestTooltip", "Test dialogue in-editor"),
  FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Simulate")
        );
  
        return ToolbarBuilder.MakeWidget();
    }
    
    void CompileGraph()
    {
 if (Graph)
        {
 Graph->CompileToAsset();
            
            // Show success notification
          FNotificationInfo Info(LOCTEXT("CompileSuccess", "Dialogue compiled successfully!"));
    Info.ExpireDuration = 3.0f;
      FSlateNotificationManager::Get().AddNotification(Info);
        }
    }
    
    void TestDialogue()
    {
        // Open test window
        // ... implementation
 }
    
 TSharedPtr<SGraphEditor> GraphEditor;
    UDialogueGraph* Graph;
};
```

#### Features

```
Visual Editor Features:
????????????????????????????????????????

Node Types:
- Dialogue Node (speech bubble icon)
- Branch Node (diamond icon)
- End Node (stop sign icon)
- Event Node (lightning bolt icon)

Connections:
- Drag from output pin to input pin
- Colored by condition (green = always, yellow = conditional)
- Labels show choice text

Node Details Panel:
- Speaker dropdown (Player/NPC)
- Text editor with rich text support
- Condition list (drag-and-drop)
- Effect list (drag-and-drop)
- Preview button

Toolbar:
- Compile (F7)
- Save (Ctrl+S)
- Test (F5)
- Import from DSL
- Export to DSL
- Find (Ctrl+F)
- Beautify (auto-arrange nodes)

Features:
- Undo/Redo (Ctrl+Z/Y)
- Copy/Paste nodes (Ctrl+C/V)
- Comment boxes
- Breakpoints for debugging
- Search/Replace text
- Validation errors displayed inline
- Auto-save every 5 minutes
```

---

## 5. Conclusion

### Implementation Priority

#### High Priority (v1.4) - 1-2 months
1. ? Event System Refactoring
2. ? Shared Data Architecture (для 50-100 NPC)
3. ? Batch Processing System

#### Medium Priority (v1.5) - 2-4 months
4. ? Sentiment Analysis Integration
5. ? Dynamic Response Generation
6. ? Conversation Memory

#### Low Priority (v1.6-2.0) - 4-12 months
7. ? Visual Dialogue Editor
8. ? ECS Architecture (для 500+ NPC)
9. ? Multiplayer Support

### Estimated Impact

```
Performance Improvements:
????????????????????????????????????????
Current (v1.3.1):
- 10-20 NPC: Smooth (60 FPS)
- 50 NPC: Playable (45 FPS)
- 100 NPC: Stuttering (25 FPS)

After v1.4 (Event System + Batching):
- 10-20 NPC: Smooth (60 FPS)
- 50 NPC: Smooth (60 FPS)
- 100 NPC: Playable (50 FPS)

After v1.5 (Shared Data + Spatial Partitioning):
- 10-20 NPC: Smooth (60 FPS)
- 50 NPC: Smooth (60 FPS)
- 100 NPC: Smooth (60 FPS)
- 200 NPC: Playable (45 FPS)

After v2.0 (ECS):
- 10-20 NPC: Smooth (60 FPS)
- 50 NPC: Smooth (60 FPS)
- 100 NPC: Smooth (60 FPS)
- 200 NPC: Smooth (60 FPS)
- 500 NPC: Playable (30-45 FPS)
```

### Memory Savings

```
Current (v1.3.1):
- 100 NPC ? 74KB = 7.4MB

After v1.5 (Shared Data):
- 100 NPC ? 2KB + profiles = ~700KB
- Savings: 90%

After v2.0 (ECS):
- 500 NPC ? 0.5KB + systems = ~1.5MB
- Savings: 96% vs current for same NPC count
```

---

**Документ подготовлен:** 2025-01-23  
**Версия:** 1.0  
**Статус:** ? Ready for Implementation
