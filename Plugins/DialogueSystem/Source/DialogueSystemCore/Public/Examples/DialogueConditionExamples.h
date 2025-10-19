// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * Примеры использования DSL парсера условий диалогов
 * 
 * Этот файл содержит практические примеры использования системы условий
 */

#pragma once

#include "CoreMinimal.h"
#include "Conditions/DialogueConditionEvaluator.h"
#include "Core/DialogueContext.h"

/**
 * Примеры простых условий
 */
class FDialogueConditionExamples
{
public:
    /**
     * Пример 1: Проверка наличия предмета
     */
    static bool Example_HasItem()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        // Простая проверка наличия предмета
        bool HasRose = Evaluator->EvaluateString(TEXT("hasItem(Rose)"), Context);
        
        // Проверка количества
        bool HasGold = Evaluator->EvaluateString(TEXT("hasItem(Gold, 100)"), Context);
        
        return HasRose && HasGold;
    }

    /**
     * Пример 2: Проверка отношений
     */
    static bool Example_Affinity()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        // Проверка минимального уровня отношений
        bool GoodRelations = Evaluator->EvaluateString(
            TEXT("affinity[Lianne] >= 50"), 
            Context
        );
        
        return GoodRelations;
    }

    /**
     * Пример 3: Сложное условие с логическими операторами
     */
    static bool Example_ComplexCondition()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        // Настройка контекста
        Context->SetCustomVariable(FName(TEXT("QuestStage")), TEXT("2"));
        Context->SetCustomVariable(FName(TEXT("MetLianne")), TEXT("true"));
        
        // Сложное условие для квестового диалога
        FString QuestCondition = TEXT(
            "(variable[QuestStage] == 2 && memory(MetLianne)) || "
            "affinity[Lianne] >= 70"
        );
        
        bool CanShowQuest = Evaluator->EvaluateString(QuestCondition, Context);
        
        return CanShowQuest;
    }

    /**
     * Пример 4: Романтическая сцена с несколькими условиями
     */
    static bool Example_RomanticScene()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        // Добавляем теги в контекст
        Context->AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Time.Evening"))));
        Context->SetCustomVariable(FName(TEXT("Location")), TEXT("Garden"));
        Context->SetCustomVariable(FName(TEXT("GaveGift")), TEXT("true"));
        
        // Условие для романтической сцены
        FString RomanticCondition = TEXT(
            "(affinity[Lianne] >= 60 && time == Evening && "
            "variable[Location] == Garden && memory(GaveGift)) || "
            "affinity[Lianne] >= 85"
        );
        
        bool ShowRomanticScene = Evaluator->EvaluateString(RomanticCondition, Context);
        
        return ShowRomanticScene;
    }

    /**
     * Пример 5: Использование отрицания и группировки
     */
    static bool Example_NegationAndGrouping()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        Context->SetCustomVariable(FName(TEXT("Node_BadChoice_Visited")), TEXT("false"));
        Context->SetCustomVariable(FName(TEXT("Node_GoodChoice_Visited")), TEXT("true"));
        
        // Проверка что игрок не сделал плохой выбор, но сделал хороший
        FString Condition = TEXT(
            "!visited(Node_BadChoice) && visited(Node_GoodChoice)"
        );
        
        bool Result = Evaluator->EvaluateString(Condition, Context);
        
        return Result;
    }

    /**
     * Пример 6: Динамическое построение условий
     */
    static UDialogueCondition* Example_DynamicCondition(
        const FName& NPCName, 
        float MinAffinity,
        const FName& RequiredItem)
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        
        // Строим условие динамически
        FString DynamicCondition = FString::Printf(
            TEXT("affinity[%s] >= %f && hasItem(%s)"),
            *NPCName.ToString(),
            MinAffinity,
            *RequiredItem.ToString()
        );
        
        return Evaluator->ParseCondition(DynamicCondition);
    }

    /**
     * Пример 7: Проверка прогресса квеста
     */
    static bool Example_QuestProgress()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        // Настройка квестовых переменных
        Context->SetCustomVariable(FName(TEXT("MainQuest")), TEXT("3"));
        Context->SetCustomVariable(FName(TEXT("SideQuest_Lianne")), TEXT("2"));
        Context->SetCustomVariable(FName(TEXT("CompletedTutorial")), TEXT("true"));
        
        // Условие для показа продвинутого диалога
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
     * Пример 8: Торговые условия
     */
    static bool Example_MerchantConditions()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        // Условие для скидки у торговца
        FString MerchantDiscount = TEXT(
            "(affinity[Merchant] >= 50 || hasItem(VIPCard)) && "
            "(hasItem(Gold, 1000) || hasItem(Gem, 10))"
        );
        
        bool GetDiscount = Evaluator->EvaluateString(MerchantDiscount, Context);
        
        return GetDiscount;
    }

    /**
     * Пример 9: Временные и сюжетные условия
     */
    static bool Example_TimeAndStoryConditions()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
        
        Context->AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Time.Night"))));
        Context->AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Weather.Rainy"))));
        Context->SetCustomVariable(FName(TEXT("Chapter")), TEXT("5"));
        
        // Условие для особой сцены
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
     * Пример 10: Использование кэширования
     */
    static void Example_Caching()
    {
        UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();
        
        // Часто используемое условие
        FString CommonCondition = TEXT("affinity[Lianne] >= 50 && hasItem(Rose)");
        
        // Первый раз парсится
        UDialogueCondition* Cond1 = Evaluator->ParseCondition(CommonCondition);
        
        // Второй раз берется из кэша (быстрее)
        UDialogueCondition* Cond2 = Evaluator->ParseCondition(CommonCondition);
        
        // Cond1 == Cond2, тот же объект из кэша
        check(Cond1 == Cond2);
    }

    /**
     * Пример 11: Отладка условий
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
            // Получить читаемое описание условия для отладки
            FString Description = Condition->GetDescription();
            UE_LOG(LogTemp, Log, TEXT("Condition: %s"), *Description);
            // Выведет: "((Affinity[Lianne] >= 50.0 AND Has Rose) OR Memory[Node_SpecialEvent_Visited] = true)"
        }
    }

    /**
     * Пример 12: Интеграция с Blueprint
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

        // Blueprint-friendly пример
        FString Condition = TEXT(
            "affinity[Lianne] >= 60 && "
            "time == Evening && "
            "hasItem(Rose)"
        );

        return Evaluator->EvaluateString(Condition, Context);
    }
};
