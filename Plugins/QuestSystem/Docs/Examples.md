# Quest System Examples

## Example 1: Simple Fetch Quest

**Quest Name**: "Warm Bread for Elyn"  
**Type**: Tutorial, Collect/Deliver/Talk  
**Estimated Time**: 10 minutes

### Quest Flow

1. Player enters village between 06:00-10:00 (auto-start)
2. Talk to Baker
3. Collect 5 Wheat in the field
4. Deliver 1 Flour to Elyn
5. Report back to Baker
6. Receive rewards

### Configuration

```json
{
  "QuestId": "Q_WarmBreadForElyn",
  "Title": "Warm Bread for Elyn",
  "Description": "The baker needs help gathering wheat to make fresh bread for Elyn. Help him out and earn her gratitude!",
  "Meta": {
    "Categories": ["quest.tutorial", "quest.village"],
    "DifficultyLevel": 1,
    "IsRepeatable": false,
  "IsMainQuest": false,
    "Priority": 1,
    "EstimatedDuration": 10
  },
  
  "StartPolicy": {
    "PolicyType": "AutoOnCondition",
    "AutoStartCondition": {
      "EventType": "OnEnterArea",
      "AreaId": "Village",
      "TimeRangeStart": 6,
      "TimeRangeEnd": 10
    }
  },
  
  "Phases": [
    {
"PhaseId": "P_TalkToBaker",
 "Title": "Speak with the Baker",
      "Objectives": [
    {
      "ObjectiveId": "Obj_TalkBaker",
          "Title": "Talk to the Baker",
        "Style": "Talk",
          "Conditions": [
            {
              "EventType": "OnNpcTalked",
           "NpcId": "NPC_Baker",
   "Count": 1
  }
          ],
   "Counters": {
  "TargetCount": 1
          }
        }
   ],
      "Transition": {
      "NextPhaseId": "P_CollectWheat",
        "AutoAdvance": true
      }
    },
    {
      "PhaseId": "P_CollectWheat",
      "Title": "Gather Wheat",
      "Objectives": [
        {
  "ObjectiveId": "Obj_CollectWheat",
          "Title": "Collect Wheat",
          "Style": "Collect",
  "Conditions": [
            {
  "EventType": "OnItemAcquired",
     "ItemId": "Item_Wheat",
              "Count": 1
    }
    ],
   "Counters": {
 "TargetCount": 5
          },
    "TrackOnHUD": true
        }
      ],
      "Transition": {
        "NextPhaseId": "P_DeliverFlour",
        "AutoAdvance": true
      }
    },
    {
      "PhaseId": "P_DeliverFlour",
      "Title": "Deliver to Elyn",
      "Objectives": [
        {
       "ObjectiveId": "Obj_DeliverFlour",
          "Title": "Deliver Flour to Elyn",
       "Style": "Deliver",
          "Conditions": [
   {
      "EventType": "OnItemDelivered",
   "ItemId": "Item_Flour",
  "NpcId": "NPC_Elyn",
              "Count": 1
            }
          ],
        "Counters": {
     "TargetCount": 1
          }
        }
      ],
 "Transition": {
   "NextPhaseId": "P_ReportBack",
        "AutoAdvance": true
      }
    },
    {
      "PhaseId": "P_ReportBack",
      "Title": "Return to Baker",
      "Objectives": [
        {
          "ObjectiveId": "Obj_ReportBaker",
    "Title": "Report to the Baker",
     "Style": "Talk",
          "Conditions": [
   {
           "EventType": "OnDialogNodeReached",
           "NpcId": "NPC_Baker",
      "DialogNodeId": "Node_QuestComplete",
   "Count": 1
        }
   ],
          "Counters": {
 "TargetCount": 1
      }
        }
      ],
      "Transition": {
      "NextPhaseId": "",
        "AutoAdvance": true
      }
    }
  ],
  
  "RewardsOnComplete": {
    "Rewards": [
      {
 "RewardType": "GrantCurrency",
        "CurrencyAmount": 20
      },
  {
      "RewardType": "ModifyAffection",
        "NpcId": "NPC_Elyn",
        "AffectionDelta": 5.0
   },
      {
 "RewardType": "UnlockDialogBranch",
        "NpcId": "NPC_Elyn",
  "DialogBranchId": "Branch_Thanks"
      }
    ]
  },
  
  "FailurePolicy": {
    "FailureType": "TimeLimit",
    "FailAfterTimeOfDay": 22,
  "AllowRetry": true
  }
}
```

### Blueprint Implementation

```
// Auto-start is configured, but you can also start manually:
Start Quest (Quest ID: Q_WarmBreadForElyn)

// When player picks up wheat:
Emit Item Acquired (Item ID: Item_Wheat, Count: 1, Instigator: Player)

// When player gives flour to Elyn:
Emit Item Delivered (Item ID: Item_Flour, NPC ID: NPC_Elyn, Count: 1)

// Check quest state:
Is Quest Active (Quest ID: Q_WarmBreadForElyn) -> Bool
Get Objective Progress (Quest ID: Q_WarmBreadForElyn, Objective ID: Obj_CollectWheat) 
  -> Current: 3, Target: 5, Completed: false
```

---

## Example 2: Branching Mystery Quest

**Quest Name**: "Secret of the Old Oak"  
**Type**: Explore/Time/Branch  
**Estimated Time**: 15 minutes

### Quest Flow

1. Start from dialog with Tavern Keeper
2. Go to the old oak tree after 20:00 in clear weather
3. Find the hidden compartment
4. Make a moral choice:
   - **Path A**: Keep the charm (get item, lose reputation)
   - **Path B**: Return to guild (get reputation, no item)

### Configuration

```json
{
  "QuestId": "Q_SecretOfOldOak",
  "Title": "Secret of the Old Oak",
  "Description": "The tavern keeper whispered about a secret hidden in the old oak tree. But only at night, when the moon is clear, can it be found...",
  "Meta": {
    "Categories": ["quest.side", "quest.mystery"],
    "DifficultyLevel": 2,
    "IsRepeatable": false,
    "IsMainQuest": false,
    "Priority": 5
  },
  
  "Dependencies": [
    {
      "RequiredLevel": 3,
      "RequiredTags": ["Player.KnowsTavernKeeper"]
    }
  ],
  
  "StartPolicy": {
    "PolicyType": "OnDialog",
    "DialogNodeId": "Tavern_SecretHint"
  },
  
  "Phases": [
    {
  "PhaseId": "P_FindOak",
      "Title": "Find the Old Oak",
      "Objectives": [
        {
        "ObjectiveId": "Obj_ReachOak",
          "Title": "Go to the old oak tree at night",
          "Style": "Visit",
          "Conditions": [
    {
    "EventType": "OnEnterArea",
         "AreaId": "Area_OldOak",
        "Count": 1
            }
  ],
          "Gates": [
         {
    "RequireTimeOfDay": true,
      "RequiredTimeStart": 20,
     "RequiredTimeEnd": 24,
         "RequireWeather": true,
         "RequiredWeather": ["Weather.Clear", "Weather.PartlyCloudy"]
            }
      ],
          "Counters": {
        "TargetCount": 1
          }
        }
      ],
      "Transition": {
        "NextPhaseId": "P_SearchTree",
        "AutoAdvance": true
      }
    },
    {
      "PhaseId": "P_SearchTree",
      "Title": "Search the Tree",
      "Objectives": [
        {
          "ObjectiveId": "Obj_FindCompartment",
          "Title": "Find the hidden compartment",
          "Style": "Discover",
          "Conditions": [
            {
         "EventType": "OnInteract",
      "ItemId": "Trigger_OakCompartment",
    "Count": 1
            }
          ],
          "Counters": {
     "TargetCount": 1
          }
        }
      ],
  "Transition": {
 "NextPhaseId": "P_MakeChoice",
        "AutoAdvance": true
      }
    },
    {
      "PhaseId": "P_MakeChoice",
      "Title": "A Difficult Decision",
      "Description": "You found an ancient charm. Keep it for yourself, or return it to the guild?",
"Objectives": [
        {
      "ObjectiveId": "Obj_Choice",
          "Title": "Make your choice",
     "Style": "Custom",
  "Logic": "Or",
    "Conditions": [
            {
 "EventType": "OnDialogChoice",
              "DialogNodeId": "Choice_KeepCharm",
              "Count": 1
            },
   {
          "EventType": "OnDialogChoice",
     "DialogNodeId": "Choice_ReturnToGuild",
      "Count": 1
     }
          ],
          "Counters": {
         "TargetCount": 1
          }
      }
      ],
      "Transition": {
        "ConditionalBranches": {
      "Choice_KeepCharm": "P_KeepPath",
  "Choice_ReturnToGuild": "P_ReturnPath"
        },
        "AutoAdvance": true
      }
    },
    {
      "PhaseId": "P_KeepPath",
      "Title": "Keeper of Secrets",
      "Objectives": [
 {
    "ObjectiveId": "Obj_KeepComplete",
          "Title": "You kept the charm",
          "Style": "Custom",
          "Conditions": [
      {
            "EventType": "Custom",
         "EventTag": "Quest.Event.AutoComplete",
            "Count": 1
            }
          ],
     "Counters": {
            "TargetCount": 1
          },
   "Signals": {
            "OnStartRewards": {
    "Rewards": [
     {
         "RewardType": "GiveItem",
         "ItemId": "Item_AncientCharm",
       "Amount": 1
       },
           {
       "RewardType": "GrantTag",
       "GrantedTag": "Quest.Choice.SelfishPath"
      }
          ]
        }
          }
  }
      ],
      "Transition": {
      "NextPhaseId": "",
        "AutoAdvance": true
      }
    },
    {
      "PhaseId": "P_ReturnPath",
  "Title": "Honor Among Thieves",
      "Objectives": [
        {
      "ObjectiveId": "Obj_ReturnToGuild",
          "Title": "Return the charm to the guild",
          "Style": "Deliver",
          "Conditions": [
         {
"EventType": "OnItemDelivered",
    "ItemId": "Item_AncientCharm",
     "NpcId": "NPC_GuildMaster",
     "Count": 1
  }
          ],
       "Counters": {
            "TargetCount": 1
          }
        }
      ],
      "Transition": {
     "NextPhaseId": "",
     "AutoAdvance": true
      },
      "PhaseRewards": {
        "Rewards": [
   {
   "RewardType": "GrantCurrency",
      "CurrencyAmount": 100
      },
       {
            "RewardType": "GrantTag",
   "GrantedTag": "Player.Reputation.Guild",
"Amount": 10
 },
 {
            "RewardType": "GrantTag",
  "GrantedTag": "Quest.Choice.HonorablePath"
  }
    ]
   }
    }
  ],
  
  "FailurePolicy": {
    "FailureType": "OnCondition",
    "FailureCondition": {
      "EventType": "OnWeatherState",
      "WeatherTags": ["Weather.Rain", "Weather.Storm"]
    },
    "AllowRetry": true
}
}
```

### Blueprint Implementation

```
// Start from dialog:
On Dialog Choice (Choice ID: Tavern_SecretHint)
  -> Start Quest (Quest ID: Q_SecretOfOldOak)

// Check if conditions are met:
Branch:
  Condition: (Current Hour >= 20) AND (Is Weather Clear)
  True: Show Hint "The oak tree glows faintly in the moonlight..."
  False: Show Hint "Come back at night when the sky is clear..."

// Player finds compartment:
On Begin Overlap (Trigger_OakCompartment)
  -> Emit Custom Event (Event Tag: Quest.Event.Interact, String: Trigger_OakCompartment)

// Player makes choice:
On Dialog Choice (Choice ID: Choice_KeepCharm)
  -> Emit Dialog Choice (Dialog ID: OakSecret, Choice ID: Choice_KeepCharm)

On Dialog Choice (Choice ID: Choice_ReturnToGuild)
  -> Emit Dialog Choice (Dialog ID: OakSecret, Choice ID: Choice_ReturnToGuild)
```

---

## Example 3: Timed Escort Quest

**Quest Name**: "Escort Merchant to Market"  
**Type**: Escort/Combat  
**Time Limit**: 15 minutes real-time

### Configuration

```json
{
  "QuestId": "Q_EscortMerchant",
  "Title": "Escort Merchant to Market",
  "Description": "A traveling merchant needs protection on the road to the market. Bandits have been spotted recently!",
  "Meta": {
 "Categories": ["quest.side", "quest.combat"],
    "DifficultyLevel": 3,
    "IsRepeatable": true,
    "Priority": 3
  },
  
  "StartPolicy": {
  "PolicyType": "Manual"
  },
  
  "Phases": [
    {
      "PhaseId": "P_Escort",
   "Title": "Protect the Merchant",
      "RequireAllObjectives": true,
      "Objectives": [
        {
        "ObjectiveId": "Obj_ReachMarket",
          "Title": "Reach the market safely",
     "Style": "Escort",
     "Conditions": [
            {
              "EventType": "OnEnterArea",
          "AreaId": "Area_Market",
 "Count": 1
            }
          ],
          "Counters": {
       "TargetCount": 1
          }
        },
   {
          "ObjectiveId": "Obj_DefeatBandits",
 "Title": "Defeat bandits (Optional)",
          "Style": "Kill",
          "IsOptional": true,
          "Conditions": [
            {
 "EventType": "OnActorKilled",
              "ActorClass": "/Game/Enemies/BP_Bandit",
              "RequiredTags": ["Enemy.Bandit"],
              "Count": 1
         }
          ],
       "Counters": {
    "TargetCount": 3,
       "AllowOvercount": true
     },
        "Signals": {
   "OnCompleteRewards": {
         "Rewards": [
     {
    "RewardType": "GrantCurrency",
       "CurrencyAmount": 50
      }
    ]
            }
          }
}
      ],
      "Transition": {
      "NextPhaseId": "",
        "AutoAdvance": true
      }
    }
  ],
  
  "RewardsOnComplete": {
    "Rewards": [
      {
        "RewardType": "GrantCurrency",
        "CurrencyAmount": 150
      },
   {
        "RewardType": "GiveItem",
        "ItemId": "Item_MerchantToken",
     "Amount": 1
      }
    ]
  },
  
  "FailurePolicy": {
    "FailureType": "TimeLimit",
    "TimeLimit": 900.0,
    "AllowRetry": false,
 "ResetProgressOnFail": true
  }
}
```

---

## Integration Examples

### Example: Item Pickup Integration

```cpp
// In your item pickup actor
void AItemPickup::OnPickedUp(ACharacter* Character)
{
    // Add to inventory
    UInventoryComponent* Inventory = Character->FindComponentByClass<UInventoryComponent>();
    if (Inventory)
    {
   Inventory->AddItem(ItemId, 1);
    }
    
  // Emit quest event
    UQuestBlueprintLibrary::EmitItemAcquired(
        this, 
        ItemId, 
 1, 
  Character, 
  ItemTags
    );
    
// Destroy pickup
    Destroy();
}
```

### Example: NPC Dialog Integration

```cpp
// In your dialog system
void UDialogComponent::CompleteDialog(FName DialogId)
{
    // Process dialog completion
    ProcessDialogRewards();
    
    // Emit quest event
    UQuestBlueprintLibrary::EmitNpcTalked(
    this,
        NPCId,
        PlayerCharacter,
        FGameplayTagContainer()
    );
}

void UDialogComponent::OnChoiceSelected(FName DialogId, FName ChoiceId)
{
    // Process choice
    CurrentDialogNode = GetNodeForChoice(ChoiceId);
    
    // Emit quest event
    UQuestBlueprintLibrary::EmitDialogChoice(
 this,
        DialogId,
    ChoiceId,
        FGameplayTagContainer()
    );
}
```

### Example: Time of Day Integration

```cpp
// In your time management system
void ATimeManager::Tick(float DeltaTime)
{
    // Update time
    CurrentTime += DeltaTime * TimeScale;
    
    int32 NewHour = FMath::FloorToInt(CurrentTime);
    if (NewHour != LastHour)
    {
        LastHour = NewHour;
        OnHourChanged(NewHour);
    }
}

void ATimeManager::OnHourChanged(int32 Hour)
{
    // Notify systems
    OnTimeChangedDelegate.Broadcast(Hour);
    
    // Emit quest event
    if (UQuestSubsystem* QuestSystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
    {
 QuestSystem->GetEventBus()->EmitTimeReached(Hour, 0);
    }
}
```

---

## Common Patterns

### Pattern 1: Multi-Stage Collection

Collect different items across multiple locations:

```
Phase 1: Collect herbs (5x Sage, 3x Mint)
Phase 2: Collect minerals (2x Iron Ore)
Phase 3: Deliver all to alchemist
```

### Pattern 2: Investigation Chain

Talk to multiple NPCs to gather clues:

```
Phase 1: Talk to Guard
Phase 2: Talk to Witness
Phase 3: Examine Crime Scene
Phase 4: Confront Suspect
```

### Pattern 3: Timed Challenge

Complete objectives before time runs out:

```
Phase 1: Accept challenge (15 min timer starts)
Phase 2: Complete 3 sub-tasks
  - Obj 1: Kill 10 enemies
  - Obj 2: Collect 5 treasures
  - Obj 3: Return to start
```

### Pattern 4: Choice and Consequence

Player choice affects quest outcome:

```
Phase 1: Investigate situation
Phase 2: Choose side (A or B)
Phase 3a: Help Side A (leads to reward X)
Phase 3b: Help Side B (leads to reward Y)
```

---

## Testing Checklist

When creating quests, test:

- [ ] Quest starts correctly
- [ ] All objectives can be completed
- [ ] Progress saves and loads
- [ ] Failure conditions work
- [ ] Rewards are granted
- [ ] UI updates correctly
- [ ] Quest can be abandoned
- [ ] Dependencies are checked
- [ ] Gates block progress when appropriate
- [ ] Event emissions trigger properly
- [ ] No errors in log
- [ ] Quest validates without errors

---

## Performance Tips

1. **Batch Event Emissions**: Don't emit events every frame
2. **Use Gates**: Restrict event processing with gates instead of polling
3. **Lazy Load Assets**: Quest assets load on-demand
4. **Limit Active Quests**: Cap number of concurrent active quests (e.g., 20)
5. **Optimize Conditions**: Use specific event types, not generic Custom
6. **Profile**: Use Unreal Insights to measure event processing time

---

For more examples, see the Content folder: `/Plugins/QuestSystem/Content/Examples/`
