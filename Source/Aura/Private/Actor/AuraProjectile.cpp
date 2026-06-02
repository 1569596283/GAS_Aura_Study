// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "Aura/Aura.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

// Sets default values
AAuraProjectile::AAuraProjectile()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
    SetRootComponent(Sphere);
    Sphere->SetCollisionObjectType(ECC_Projectile);
    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
    Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->InitialSpeed = 550.f;
    ProjectileMovement->MaxSpeed = 550.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AAuraProjectile::BeginPlay()
{
    Super::BeginPlay();
    SetLifeSpan(LifeSpan);
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

    LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::Destroyed()
{
    if (LoopingSoundComponent) {
        LoopingSoundComponent->Stop();
        LoopingSoundComponent->DestroyComponent();
    }
    if (!bHit && !HasAuthority())
        OnHit();

    Super::Destroyed();
}

void AAuraProjectile::OnHit()
{
    UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
    if (LoopingSoundComponent) {
        LoopingSoundComponent->Stop();
        LoopingSoundComponent->DestroyComponent();
    }
    bHit = true;
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
    if (SourceAvatarActor == OtherActor)
        return;

    if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor))
        return;

    if (!bHit)
        OnHit();

    if (HasAuthority()) {
        if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor)) {
            const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
            DamageEffectParams.DeathImpulse = DeathImpulse;
            const bool bKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;
            if (bKnockback) {
                FRotator Rotation = GetActorRotation();
                Rotation.Pitch = 45.f;
                const FVector KnockbackDirection = Rotation.Vector();
                const FVector KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackFroceMagnitude;
                DamageEffectParams.KnockbackForce = KnockbackForce;
            }
            else {
                DamageEffectParams.KnockbackForce = FVector::ZeroVector;
            }
            DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
            UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
        }

        Destroy();
    }
    else {
        bHit = true;
    }

}

