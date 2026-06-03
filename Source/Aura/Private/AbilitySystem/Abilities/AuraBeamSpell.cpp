// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
    if (HitResult.bBlockingHit) {
        MouseHitLocation = HitResult.ImpactPoint;
        MouseHitActor = HitResult.GetActor();
    }
    else {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

void UAuraBeamSpell::StoreOwnerVariable()
{
    if (CurrentActorInfo) {
        OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
        OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
    }
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
    check(OwnerCharacter);
    if (OwnerCharacter->Implements<UCombatInterface>()) {
        if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter)) {
            TArray<AActor*> ActorsToIgnore;
            ActorsToIgnore.Add(OwnerCharacter);
            FHitResult HitResult;
            const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
            UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter, SocketLocation, BeamTargetLocation, 10.f, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

            if (HitResult.bBlockingHit) {
                MouseHitLocation = HitResult.ImpactPoint;
                MouseHitActor = HitResult.GetActor();
            }
        }
    }
    if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor)) {
        if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied)) {
            CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
        }
    }
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionTargets)
{
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
    ActorsToIgnore.Add(MouseHitActor);

    TArray<AActor*> OverlappingActors;
    UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(GetAvatarActorFromActorInfo(), OverlappingActors, ActorsToIgnore, 850.f, MouseHitActor->GetActorLocation());

    //int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
    int32 NumAdditionalTargets = MaxNumShockTargets;

    UAuraAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionTargets, MouseHitActor->GetActorLocation());

    for (AActor* Target : OutAdditionTargets) {
        if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target)) {
            if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied)) {
                CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
            }
        }
    }
}