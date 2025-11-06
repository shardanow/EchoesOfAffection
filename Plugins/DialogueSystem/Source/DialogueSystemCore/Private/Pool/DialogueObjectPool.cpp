// Copyright Epic Games, Inc. All Rights Reserved.

#include "Pool/DialogueObjectPool.h"
#include "Core/DialogueRunner.h"

void UDialogueObjectPool::Initialize(UClass* InClass, int32 InInitialSize, int32 InMaxSize)
{
    if (!InClass)
    {
        UE_LOG(LogTemp, Error, TEXT("UDialogueObjectPool::Initialize - InClass is null"));
        return;
    }

    PooledClass = InClass;
    MaxPoolSize = InMaxSize;

    // Pre-create initial objects
    PreWarm(InInitialSize);
}

UObject* UDialogueObjectPool::Acquire()
{
    if (!PooledClass)
    {
        UE_LOG(LogTemp, Error, TEXT("UDialogueObjectPool::Acquire - Pool not initialized"));
        return nullptr;
    }

    UObject* Object = nullptr;

    // Try to reuse from available pool
    if (AvailableObjects.Num() > 0)
    {
        Object = AvailableObjects.Pop();
    }
    else
    {
        // Create new if pool is empty
        Object = CreatePooledObject();
    }

    if (Object)
    {
        ObjectsInUse.Add(Object);
    }

    return Object;
}

void UDialogueObjectPool::Release(UObject* Object)
{
    if (!Object)
    {
        return;
    }

    // Remove from in-use set
    if (!ObjectsInUse.Remove(Object))
    {
        UE_LOG(LogTemp, Warning, TEXT("UDialogueObjectPool::Release - Object was not from this pool"));
        return;
    }

    // Reset object state
    ResetPooledObject(Object);

    // Check if we should keep it or destroy it
    if (MaxPoolSize > 0 && AvailableObjects.Num() >= MaxPoolSize)
    {
        // Pool is full, let GC collect it
        Object->ConditionalBeginDestroy();
    }
    else
    {
        // Return to pool
        AvailableObjects.Add(Object);
    }
}

void UDialogueObjectPool::GetPoolStats(int32& OutAvailable, int32& OutInUse, int32& OutTotal) const
{
    OutAvailable = AvailableObjects.Num();
    OutInUse = ObjectsInUse.Num();
    OutTotal = TotalCreated;
}

void UDialogueObjectPool::PreWarm(int32 Count)
{
    if (!PooledClass)
    {
        return;
    }

    for (int32 i = 0; i < Count; ++i)
    {
        UObject* Object = CreatePooledObject();
        if (Object)
        {
            AvailableObjects.Add(Object);
        }
    }
}

void UDialogueObjectPool::Clear()
{
    // Destroy all objects
    for (UObject* Object : AvailableObjects)
    {
        if (Object)
        {
            Object->ConditionalBeginDestroy();
        }
    }

    AvailableObjects.Empty();
    ObjectsInUse.Empty();
    TotalCreated = 0;
}

void UDialogueObjectPool::Trim()
{
    // Keep only half of available objects
    int32 TargetSize = FMath::Max(1, AvailableObjects.Num() / 2);
    
    while (AvailableObjects.Num() > TargetSize)
    {
        UObject* Object = AvailableObjects.Pop();
        if (Object)
        {
            Object->ConditionalBeginDestroy();
        }
    }
}

UObject* UDialogueObjectPool::CreatePooledObject()
{
    if (!PooledClass)
    {
        return nullptr;
    }
    UObject* NewObj = ::NewObject<UObject>(this, PooledClass);
    if (NewObj)
    {
        TotalCreated++;
    }

    return NewObj;
}

void UDialogueObjectPool::ResetPooledObject(UObject* Object)
{
    // Base implementation - can be overridden
    // Most objects don't need special reset logic
}

// ============================================================================
// UDialogueRunnerPool
// ============================================================================

void UDialogueRunnerPool::ResetPooledObject(UObject* Object)
{
    Super::ResetPooledObject(Object);

    // DialogueRunner-specific reset
    UDialogueRunner* Runner = Cast<UDialogueRunner>(Object);
    if (Runner)
    {
        // End dialogue if still active
        if (Runner->IsActive())
        {
            Runner->EndDialogue();
        }

        // Clear internal state
        // Runner will be reset in next StartDialogue call
    }
}
