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
 * ������� UI ������ �������
 * 
 * ��������:
 * - MVVM pattern (ViewModel ������ �� View)
 * - Event-driven (�������� �� ������� Runner'�)
 * - ������������� (typewriter, transitions)
 * - ��������� (Choice buttons - ��������� �������)
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

    /** ������ �������� ���������� */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> SpeakerPortrait;

    /** ��� ���������� */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> SpeakerName;

    /** ����� ������� (RichText ��� ��������������) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<URichTextBlock> DialogueText;

    /** ��������� ��� ������ ������ */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UVerticalBox> ChoicesContainer;

    /** ������ ������� (�����������) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UCanvasPanel> HistoryPanel;

    /** ������ ������ */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> EmotionIcon;

    /** ��������� "�������� input" */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> ContinueIndicator;

    //~ End Widget Bindings

    //~ Begin Configuration

    /** ����� ������� ��� ������ ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UChoiceWidget> ChoiceWidgetClass;

    /** �������� typewriter ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    bool bEnableTypewriter = true;

    /** ���������� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    bool bShowPortrait = true;

    /** ���������� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    bool bShowEmotions = true;

    //~ End Configuration

    //~ Begin Runtime State

    /** ������� Runner */
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

    /** ������ ��������� ������ ������ */
    UPROPERTY(Transient)
    TArray<TObjectPtr<class UChoiceWidget>> ChoiceWidgets;

    //~ End Runtime State

public:
    //~ Begin Public API

    /** ������� � Runner */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void BindToRunner(UDialogueRunner* Runner);

    /** ���������� �� Runner */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void UnbindFromRunner();

    /** ���������� typewriter */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void SkipTypewriter();

    /** ������� ������� �� ������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void SelectChoice(int32 Index);

    //~ End Public API

protected:
    //~ Begin Event Handlers

    /** Callback: ����� � ���� */
    UFUNCTION()
    void HandleNodeEntered(UDialogueNode* Node);

    /** Callback: ������ ������ */
    UFUNCTION()
    void HandleChoicesReady(const TArray<UDialogueNode*>& Choices);

    /** Callback: ������ �������� */
    UFUNCTION()
    void HandleDialogueEnded();

    //~ End Event Handlers

    //~ Begin Internal Logic

    /** �������� ������� */
    void UpdatePortrait(const UDialogueNode* Node, const UDialogueSessionContext* Context);

    /** �������� ��� */
    void UpdateSpeakerName(const UDialogueNode* Node);

    /** ��������� typewriter */
    void StartTypewriter(const FText& Text);

    /** �������� typewriter (tick) */
    void UpdateTypewriter(float DeltaTime);

    /** ������� ������ ������ */
    void CreateChoiceButtons(const TArray<UDialogueNode*>& Choices, const UDialogueSessionContext* Context);

    /** �������� ������ */
    void ClearChoiceButtons();

    /** �������� ������ */
    void UpdateEmotion(const FGameplayTag& EmotionTag);

    /** ������������� �������� ��������� */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void PlayIntroAnimation();

    /** ������������� �������� ����� */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void PlayOutroAnimation();

    //~ End Internal Logic
};
