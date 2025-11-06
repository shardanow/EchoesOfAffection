
UCLASS()
class AMyPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Callback for dialogue start event
    UFUNCTION()
    void OnDialogueEnded(AActor* Player, AActor* NPC);

private:
  // Reference to subsystem
    TWeakObjectPtr<UDialogueSubsystem> DialogueSubsystem;
};


#include "Subsystems/DialogueSubsystem.h"

void AMyPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (UGameInstance* GI = GetGameInstance())
    {
        DialogueSubsystem = GI->GetSubsystem<UDialogueSubsystem>();
  
        if (DialogueSubsystem.IsValid())
        {
            DialogueSubsystem->OnAnyDialogueEnded.AddDynamic(this, &AMyPlayerCharacter::OnDialogueEnded);
        }
    }
}

void AMyPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (DialogueSubsystem.IsValid())
    {
      DialogueSubsystem->OnAnyDialogueEnded.RemoveDynamic(this, &AMyPlayerCharacter::OnDialogueEnded);
    }

    Super::EndPlay(EndPlayReason);
}

void AMyPlayerCharacter::OnDialogueEnded(AActor* Player, AActor* NPC)
{
    if (Player == this)
    {
        UE_LOG(LogTemp, Log, TEXT("Player dialogue ended with NPC: %s"), *NPC->GetName());
   
  }
}
