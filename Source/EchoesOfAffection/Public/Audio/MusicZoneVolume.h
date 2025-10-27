// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Audio/MusicManager.h"
#include "Sound/SoundBase.h"
#include "MusicZoneVolume.generated.h"

/**
 * Trigger box that plays specific music when the player enters
 * Simple, clean, and works perfectly with UE editor
 */
UCLASS(Blueprintable)
class ECHOESOFAFFECTION_API AMusicZoneVolume : public ATriggerBox
{
    GENERATED_BODY()

public:
    AMusicZoneVolume();

    //~ Begin AActor Interface
    virtual void BeginPlay() override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
    //~ End AActor Interface

    /** Get the zone music */
    UFUNCTION(BlueprintPure, Category = "Music Zone")
    USoundBase* GetZoneMusic() const { return ZoneMusic; }

    /** Set the zone music */
    UFUNCTION(BlueprintCallable, Category = "Music Zone")
    void SetZoneMusic(USoundBase* NewMusic) { ZoneMusic = NewMusic; }

    /** Enable/disable this zone */
    UFUNCTION(BlueprintCallable, Category = "Music Zone")
    void SetZoneActive(bool bActive) { bIsActive = bActive; }

    /** Check if zone is active */
    UFUNCTION(BlueprintPure, Category = "Music Zone")
    bool IsZoneActive() const { return bIsActive; }

protected:
    /** Music to play in this zone */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
    TObjectPtr<USoundBase> ZoneMusic;

    /** Priority of this zone (higher = more important, range: 0-100) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone", meta = (ClampMin = "0", ClampMax = "100"))
    int32 Priority = 10;

    /** Type of transition when entering this zone */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
    EMusicTransitionType TransitionType = EMusicTransitionType::Crossfade;

    /** Duration of crossfade in seconds */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone", meta = (EditCondition = "TransitionType == EMusicTransitionType::Crossfade", ClampMin = "0.1", ClampMax = "10.0"))
    float CrossfadeDuration = 2.0f;

    /** Volume multiplier for this zone's music */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float VolumeMultiplier = 1.0f;

    /** Is this zone currently active */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
  bool bIsActive = true;

    /** Only trigger for player pawns */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone")
    bool bPlayerOnly = true;

 /** Debug: Show zone info on screen when player enters */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music Zone|Debug")
    bool bShowDebugInfo = false;

    /** Called when player enters the zone */
    UFUNCTION(BlueprintNativeEvent, Category = "Music Zone")
    void OnPlayerEntered(APlayerController* PlayerController);
    virtual void OnPlayerEntered_Implementation(APlayerController* PlayerController);

  /** Called when player exits the zone */
    UFUNCTION(BlueprintNativeEvent, Category = "Music Zone")
void OnPlayerExited(APlayerController* PlayerController);
    virtual void OnPlayerExited_Implementation(APlayerController* PlayerController);

private:
    /** Handle player entering zone */
    void HandlePlayerEntered(APlayerController* PlayerController);

    /** Handle player exiting zone */
    void HandlePlayerExited(APlayerController* PlayerController);

    /** Show debug info on screen */
  void ShowDebugInfo(const FString& Message, const FColor& Color);

    // ? Ќќ¬ќ≈: ”никальный ID зоны в MusicManager
    int32 RegisteredZoneID = -1;

#if WITH_EDITOR
    /** Called when property changes in editor */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
