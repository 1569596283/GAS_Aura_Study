// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraAbilityTypes.h"
#include "AuraProjectile.generated.h"

class USphereComponent;
class UPrimitiveComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;


UCLASS()
class AURA_API AAuraProjectile : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AAuraProjectile();

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    FDamageEffectParams DamageEffectParams;

    UPROPERTY()
    TObjectPtr<USceneComponent> HomingTargetSceneComponent;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void Destroyed() override;

    UFUNCTION(BlueprintCallable)
    void OnHit();

    UFUNCTION()
    virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USphereComponent>Sphere;

    bool IsValidOverlap(AActor* OtherActor);

    bool bHit = false;

private:

    UPROPERTY(EditDefaultsOnly)
    float LifeSpan = 15.f;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UNiagaraSystem> ImpactEffect;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> ImpactSound;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> LoopingSound;

    UPROPERTY()
    TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
