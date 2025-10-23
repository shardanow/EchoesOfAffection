// В вашем Player Character .h файле

UCLASS()
class AMyPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Callback для окончания диалога
    UFUNCTION()
    void OnDialogueEnded(AActor* Player, AActor* NPC);

private:
  // Ссылка на subsystem
    TWeakObjectPtr<UDialogueSubsystem> DialogueSubsystem;
};

// В вашем Player Character .cpp файле

#include "Subsystems/DialogueSubsystem.h"

void AMyPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Получаем DialogueSubsystem
    if (UGameInstance* GI = GetGameInstance())
    {
        DialogueSubsystem = GI->GetSubsystem<UDialogueSubsystem>();
  
        if (DialogueSubsystem.IsValid())
        {
            // Подписываемся на глобальное событие окончания диалога
            DialogueSubsystem->OnAnyDialogueEnded.AddDynamic(this, &AMyPlayerCharacter::OnDialogueEnded);
        }
    }
}

void AMyPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Отписываемся от события
    if (DialogueSubsystem.IsValid())
    {
      DialogueSubsystem->OnAnyDialogueEnded.RemoveDynamic(this, &AMyPlayerCharacter::OnDialogueEnded);
    }

    Super::EndPlay(EndPlayReason);
}

void AMyPlayerCharacter::OnDialogueEnded(AActor* Player, AActor* NPC)
{
    // Проверяем, что диалог был с ЭТИМ игроком
    if (Player == this)
    {
        UE_LOG(LogTemp, Log, TEXT("Player dialogue ended with NPC: %s"), *NPC->GetName());
   
        // ВАШ КОД ЗДЕСЬ
        // Например, разблокировка UI, восстановление управления и т.д.
  }
}
