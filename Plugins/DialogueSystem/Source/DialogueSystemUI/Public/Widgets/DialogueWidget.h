// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "DialogueWidget.generated.h"

// Forward declarations
class UDialogueRunner;
class UDialogueNode;
class UDialogueSessionContext;
class UTextBlock;
class UImage;
class UVerticalBox;
class UCanvasPanel;
class URichTextBlock;

/**
 * Главный UI виджет диалога
 * 
 * Принципы:
 * - MVVM pattern (ViewModel отделён от View)
 * - Event-driven (подписка на события Runner'а)
 * - Анимированный (typewriter, transitions)
 * - Модульный (Choice buttons - отдельные виджеты)
 */
UCLASS(Abstract)
class DIALOGUESYSTEMUI_API UDialogueWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    //~ Begin UUserWidget Interface
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    //~ End UUserWidget Interface

protected:
    //~ Begin Widget Bindings (meta = (BindWidget))

    /** Панель портрета говорящего */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> SpeakerPortrait;

    /** Имя говорящего */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> SpeakerName;

    /** Текст реплики (RichText для форматирования) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<URichTextBlock> DialogueText;

    /** Контейнер для кнопок выбора */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UVerticalBox> ChoicesContainer;

    /** Панель истории (опционально) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UCanvasPanel> HistoryPanel;

    /** Иконка эмоции */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> EmotionIcon;

    /** Индикатор "ожидание input" */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> ContinueIndicator;

    //~ End Widget Bindings

    //~ Begin Configuration

    /** Класс виджета для кнопки выбора */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UChoiceWidget> ChoiceWidgetClass;

    /** Включить typewriter эффект */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    bool bEnableTypewriter = true;

    /** Показывать портрет */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    bool bShowPortrait = true;

    /** Показывать эмоции */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    bool bShowEmotions = true;

    //~ End Configuration

    //~ Begin Runtime State

    /** Текущий Runner */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueRunner> CurrentRunner;

    /** Typewriter state */
    UPROPERTY(Transient)
    FString FullText;

    UPROPERTY(Transient)
    FString VisibleText;

    UPROPERTY(Transient)
    int32 CurrentCharIndex = 0;

    UPROPERTY(Transient)
    bool bIsTyping = false;

    /** Список созданных кнопок выбора */
    UPROPERTY(Transient)
    TArray<TObjectPtr<class UChoiceWidget>> ChoiceWidgets;

    //~ End Runtime State

public:
    //~ Begin Public API

    /** Связать с Runner */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void BindToRunner(UDialogueRunner* Runner);

    /** Отвязаться от Runner */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void UnbindFromRunner();

    /** Пропустить typewriter */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void SkipTypewriter();

    /** Выбрать вариант по индексу */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void SelectChoice(int32 Index);

    //~ End Public API

protected:
    //~ Begin Event Handlers

    /** Callback: вошли в узел */
    UFUNCTION()
    void HandleNodeEntered(UDialogueNode* Node);

    /** Callback: готовы выборы */
    UFUNCTION()
    void HandleChoicesReady(const TArray<UDialogueNode*>& Choices);

    /** Callback: диалог завершён */
    UFUNCTION()
    void HandleDialogueEnded();

    //~ End Event Handlers

    //~ Begin Internal Logic

    /** Обновить портрет */
    void UpdatePortrait(const UDialogueNode* Node, const UDialogueSessionContext* Context);

    /** Обновить имя */
    void UpdateSpeakerName(const UDialogueNode* Node);

    /** Запустить typewriter */
    void StartTypewriter(const FText& Text);

    /** Обновить typewriter (tick) */
    void UpdateTypewriter(float DeltaTime);

    /** Создать кнопки выбора */
    void CreateChoiceButtons(const TArray<UDialogueNode*>& Choices, const UDialogueSessionContext* Context);

    /** Очистить кнопки */
    void ClearChoiceButtons();

    /** Обновить эмоцию */
    void UpdateEmotion(const FGameplayTag& EmotionTag);

    /** Воспроизвести анимацию появления */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void PlayIntroAnimation();

    /** Воспроизвести анимацию ухода */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void PlayOutroAnimation();

    //~ End Internal Logic
};
