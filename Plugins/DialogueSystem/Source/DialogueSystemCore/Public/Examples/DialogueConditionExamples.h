// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * 
 */

#pragma once

#include "CoreMinimal.h"
#include "Conditions/DialogueConditionEvaluator.h"
#include "Core/DialogueContext.h"

/**
 */
class FDialogueConditionExamples
{
public:
    /**
     */
    static bool Example_HasItem()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        bool HasRose = Evaluator->EvaluateString(TEXT("hasItem(Rose)"), Context);
        
        bool HasGold = Evaluator->EvaluateString(TEXT("hasItem(Gold, 100)"), Context);
        
        return HasRose && HasGold;
    }

    /**
     */
    static bool Example_Affinity()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        bool GoodRelations = Evaluator->EvaluateString(
            TEXT("affinity[Lianne] >= 50"), 
            Context
        );
        
        return GoodRelations;
    }

    /**
     */
    static bool Example_ComplexCondition()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        Context->SetCustomVariable(FName(TEXT("QuestStage")), TEXT("2"));
        Context->SetCustomVariable(FName(TEXT("MetLianne")), TEXT("true"));
        
        FString QuestCondition = TEXT(
            "(variable[QuestStage] == 2 && memory(MetLianne)) || "
            "affinity[Lianne] >= 70"
        );
        
        bool CanShowQuest = Evaluator->EvaluateString(QuestCondition, Context);
        
        return CanShowQuest;
    }

    /**
     */
    static bool Example_RomanticScene()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        Context->AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Time.Evening"))));
        Context->SetCustomVariable(FName(TEXT("Location")), TEXT("Garden"));
        Context->SetCustomVariable(FName(TEXT("GaveGift")), TEXT("true"));
        
        FString RomanticCondition = TEXT(
            "(affinity[Lianne] >= 60 && time == Evening && "
            "variable[Location] == Garden && memory(GaveGift)) || "
            "affinity[Lianne] >= 85"
        );
        
        bool ShowRomanticScene = Evaluator->EvaluateString(RomanticCondition, Context);
        
        return ShowRomanticScene;
    }

    /**
     */
    static bool Example_NegationAndGrouping()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        Context->SetCustomVariable(FName(TEXT("Node_BadChoice_Visited")), TEXT("false"));
        Context->SetCustomVariable(FName(TEXT("Node_GoodChoice_Visited")), TEXT("true"));
        
        FString Condition = TEXT(
            "!visited(Node_BadChoice) && visited(Node_GoodChoice)"
        );
        
        bool Result = Evaluator->EvaluateString(Condition, Context);
        
        return Result;
    }

    /**
     */
    static UDialogueCondition* Example_DynamicCondition(
        const FName& NPCName, 
        float MinAffinity,
        const FName& RequiredItem)
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        
        FString DynamicCondition = FString::Printf(
            TEXT("affinity[%s] >= %f && hasItem(%s)"),
            *NPCName.ToString(),
            MinAffinity,
            *RequiredItem.ToString()
        );
        
        return Evaluator->ParseCondition(DynamicCondition);
    }

    /**
     */
    static bool Example_QuestProgress()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        Context->SetCustomVariable(FName(TEXT("MainQuest")), TEXT("3"));
        Context->SetCustomVariable(FName(TEXT("SideQuest_Lianne")), TEXT("2"));
        Context->SetCustomVariable(FName(TEXT("CompletedTutorial")), TEXT("true"));
        
        FString AdvancedQuestCondition = TEXT(
            "variable[MainQuest] == 3 && "
            "variable[SideQuest_Lianne] >= 2 && "
            "memory(CompletedTutorial)"
        );
        
        bool ShowAdvancedDialogue = Evaluator->EvaluateString(
            AdvancedQuestCondition, 
            Context
        );
        
        return ShowAdvancedDialogue;
    }

    /**
     */
    static bool Example_MerchantConditions()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        FString MerchantDiscount = TEXT(
            "(affinity[Merchant] >= 50 || hasItem(VIPCard)) && "
            "(hasItem(Gold, 1000) || hasItem(Gem, 10))"
        );
        
        bool GetDiscount = Evaluator->EvaluateString(MerchantDiscount, Context);
        
        return GetDiscount;
    }

    /**
     */
    static bool Example_TimeAndStoryConditions()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        Context->AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Time.Night"))));
        Context->AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Weather.Rainy"))));
        Context->SetCustomVariable(FName(TEXT("Chapter")), TEXT("5"));
        
        FString SpecialSceneCondition = TEXT(
            "time == Night && "
            "tag(Weather.Rainy) && "
            "variable[Chapter] >= 5 && "
            "affinity[Lianne] >= 70"
        );
        
        bool ShowSpecialScene = Evaluator->EvaluateString(
            SpecialSceneCondition, 
            Context
        );
        
        return ShowSpecialScene;
    }

    /**
     */
    static void Example_Caching()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        
        FString CommonCondition = TEXT("affinity[Lianne] >= 50 && hasItem(Rose)");
        
        UDialogueCondition* Cond1 = Evaluator->ParseCondition(CommonCondition);
        
        UDialogueCondition* Cond2 = Evaluator->ParseCondition(CommonCondition);
        
        check(Cond1 == Cond2);
    }

    /**
     */
    static void Example_DebuggingConditions()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        
        FString ComplexCondition = TEXT(
            "(affinity[Lianne] >= 50 && hasItem(Rose)) || "
            "visited(Node_SpecialEvent)"
        );
        
        UDialogueCondition* Condition = Evaluator->ParseCondition(ComplexCondition);
        
        if (Condition)
        {
            FString Description = Condition->GetDescription();
            UE_LOG(LogTemp, Log, TEXT("Condition: %s"), *Description);
        }
    }

    /**
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Examples")
    static bool BP_CheckRomanticSceneCondition(
        UDialogueConditionEvaluator* Evaluator,
        UDialogueSessionContext* Context)
    {
        if (!Evaluator || !Context)
        {
            return false;
        }

        FString Condition = TEXT(
            "affinity[Lianne] >= 60 && "
            "time == Evening && "
            "hasItem(Rose)"
        );

        return Evaluator->EvaluateString(Condition, Context);
    }
};
