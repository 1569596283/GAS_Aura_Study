// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UDamageTextComponent;
class UInputMappingContext;
class UInputAction;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;
class UNiagaraSystem;
class AMagicCircle;

struct FInputActionValue;

enum class ETargetingStatus : uint8 {
    TargetingEnemy,
    TargetingNoEnemy,
    NotTargeting
};

/**
 *
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAuraPlayerController();
    virtual void PlayerTick(float DeltaTime) override;

    UFUNCTION(Client, Reliable)
    void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

    UFUNCTION(BlueprintCallable)
    void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);
    UFUNCTION(BlueprintCallable)
    void HideMagicCircle();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> AuraContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ShiftAction;

    void ShiftPressed() { bShiftKeyDown = true; };
    void ShiftReleased() { bShiftKeyDown = false; };
    bool bShiftKeyDown = false;

    void Move(const  FInputActionValue& InputActionValue);

    void CursorTrace();
    TObjectPtr<AActor> LastActor;
    TObjectPtr<AActor> ThisActor;
    FHitResult CursorHit;
    void HighlightActor(AActor* InActor);
    void UnHighlightActor(AActor* InActor);

    void AbilityInputTagPressed(FGameplayTag InputTag);
    void AbilityInputTagReleased(FGameplayTag InputTag);
    void AbilityInputTagHeld(FGameplayTag InputTag);

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UAuraInputConfig> InputConfig;

    UPROPERTY()
    TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

    UAuraAbilitySystemComponent* GetASC();

    FVector CachedDestination = FVector::ZeroVector;
    float FllowTime = 0.f;
    float ShortPressThreshold = 0.5f;
    bool bAutoRunning = false;
    ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;

    UPROPERTY(EditDefaultsOnly)
    float AutoRunAcceptanceRadius = 50.f;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USplineComponent> Spline;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

    void AutoRun();

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AMagicCircle> MagicCircleClass;

    UPROPERTY()
    TObjectPtr<AMagicCircle> MagicCircle;

    void UpdateMagicCircleLocation();
};
