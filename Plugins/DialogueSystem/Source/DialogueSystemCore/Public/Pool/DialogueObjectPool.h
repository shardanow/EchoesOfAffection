// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueObjectPool.generated.h"

/**
 * Generic object pool for UObjects
 * Reduces allocation overhead by reusing objects
 * 
 * Performance benefits:
 * - Reduces GC pressure
 * - Minimizes allocation time
 * - Better cache locality
 * - Predictable memory usage
 * 
 * Thread-safety: Not thread-safe, use from Game Thread only
 */
UCLASS()
class DIALOGUESYSTEMCORE_API UDialogueObjectPool : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Initialize the pool with a specific object class
     * @param InClass The class of objects to pool
     * @param InInitialSize Initial pool size
     * @param InMaxSize Maximum pool size (0 = unlimited)
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Pool")
    void Initialize(UClass* InClass, int32 InInitialSize = 5, int32 InMaxSize = 20);

    /**
     * Acquire an object from the pool
     * If pool is empty, creates a new object
     * @return Object instance ready for use
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Pool")
    UObject* Acquire();

    /**
     * Return an object to the pool
     * Object will be reset before being reused
     * @param Object The object to return
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Pool")
    void Release(UObject* Object);

    /**
     * Get current pool statistics
     * @param OutAvailable Number of available objects
     * @param OutInUse Number of objects in use
     * @param OutTotal Total objects created
     */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Pool")
    void GetPoolStats(int32& OutAvailable, int32& OutInUse, int32& OutTotal) const;

    /**
     * Pre-warm the pool by creating objects in advance
     * @param Count Number of objects to create
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Pool")
    void PreWarm(int32 Count);

    /**
     * Clear the pool and destroy all cached objects
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Pool")
    void Clear();

    /**
     * Trim excess objects from the pool
     * Keeps only the minimum required
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Pool")
    void Trim();

protected:
    /** The class of pooled objects */
    UPROPERTY(Transient)
    TObjectPtr<UClass> PooledClass;

    /** Available objects ready for reuse */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UObject>> AvailableObjects;

    /** Objects currently in use */
    UPROPERTY(Transient)
    TSet<TObjectPtr<UObject>> ObjectsInUse;

    /** Maximum pool size (0 = unlimited) */
    UPROPERTY(Transient)
    int32 MaxPoolSize = 20;

    /** Total objects created (for statistics) */
    UPROPERTY(Transient)
    int32 TotalCreated = 0;

    /**
     * Create a new pooled object
     * @return New object instance
     */
    UObject* CreatePooledObject();

    /**
     * Reset object to default state before reusing
     * Override this for custom reset logic
     * @param Object The object to reset
     */
    virtual void ResetPooledObject(UObject* Object);
};

/**
 * Specialized pool for DialogueRunners
 * Handles DialogueRunner-specific reset logic
 */
UCLASS()
class DIALOGUESYSTEMCORE_API UDialogueRunnerPool : public UDialogueObjectPool
{
    GENERATED_BODY()

protected:
    virtual void ResetPooledObject(UObject* Object) override;
};
