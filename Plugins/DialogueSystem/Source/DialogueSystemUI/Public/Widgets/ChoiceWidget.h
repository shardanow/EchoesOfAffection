// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChoiceWidget.generated.h"

// Forward declarations
class UDialogueNode;
class UButton;
class UTextBlock;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceSelectedSignature, int32, ChoiceIndex);

/**
 * ������ ��� ����� ������ ������
 * ���������� �����, ������ �����������, ������
 */
UCLASS(Abstract)
class DIALOGUESYSTEMUI_API UChoiceWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    //~ Begin UUserWidget Interface
    virtual void NativeConstruct() override;
    //~ End UUserWidget Interface

    //~ Begin Events

    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoiceSelectedSignature OnChoiceSelected;

    //~ End Events

protected:
    //~ Begin Widget Bindings

    /** ������ */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> ChoiceButton;

    /** ����� ������ */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> ChoiceText;

    /** ������ ����������� (�����������) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> ConsequenceIcon;

    /** ����� ������ (1, 2, 3...) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> HotkeyText;

    //~ End Widget Bindings

    //~ Begin Data

    /** ��������� ���� */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueNode> AssociatedNode;

    /** ������ */
    UPROPERTY(Transient)
    int32 ChoiceIndex = -1;

    //~ End Data

public:
    //~ Begin Public API

    /** ���������������� ������ � ������� ������ */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void InitializeChoice(UDialogueNode* Node, int32 Index);

    /** Set choice data (used by DialogueWidget) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void SetChoiceData(UDialogueNode* Node, int32 Index);

    /** �������� ������ */
    UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
    int32 GetChoiceIndex() const { return ChoiceIndex; }

    /** �������� ���� */
    UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
    UDialogueNode* GetNode() const { return AssociatedNode; }

    //~ End Public API

protected:
    /** Callback: ������ ������ */
    UFUNCTION()
    void HandleButtonClicked();

    /** �������� ������ */
    void UpdateVisuals();

    /** Blueprint event ��� ������������ */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void OnChoiceInitialized(UDialogueNode* Node, int32 Index);
};
