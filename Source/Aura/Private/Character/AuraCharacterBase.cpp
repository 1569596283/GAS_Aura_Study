// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"

#include "Aura/Aura.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "AuraGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <AbilitySystem/Debuff/DebuffNiagaraComponent.h>
#include <Net/UnrealNetwork.h>

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
    BurnDebuffComponent->SetupAttachment(GetRootComponent());
    BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;
    StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");
    StunDebuffComponent->SetupAttachment(GetRootComponent());
    StunDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetCapsuleComponent()->SetGenerateOverlapEvents(false);
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
    GetMesh()->SetGenerateOverlapEvents(true);

    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
    Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
    Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
    DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
    DOREPLIFETIME(AAuraCharacterBase, bIsBeingShocked);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
    return HitReactMontage;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
    Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
    MulticasHandleDeath(DeathImpulse);
}

void AAuraCharacterBase::MulticasHandleDeath_Implementation(const FVector& DeathImpulse)
{
    UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

    Weapon->SetSimulatePhysics(true);
    Weapon->SetEnableGravity(true);
    Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

    GetMesh()->SetEnableGravity(true);
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Dissolve();
    bDead = true;
    OnDeath.Broadcast(this);
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    bIsStunned = NewCount > 0;
    GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::OnRep_Stunned()
{

}

void AAuraCharacterBase::OnRep_Burned()
{
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
    Super::BeginPlay();

}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const  FGameplayTag& MontageTag)
{
    // TODO 根据标签返回插槽位置
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
    if (MontageTag.MatchesTagExact(GameplayTags.Combat_Socket_Weapon) && IsValid(Weapon)) {
        return Weapon->GetSocketLocation(WeaponTipSocketName);
    }
    if (MontageTag.MatchesTagExact(GameplayTags.Combat_Socket_LeftHand)) {
        return GetMesh()->GetSocketLocation(LeftHandSocketName);
    }
    if (MontageTag.MatchesTagExact(GameplayTags.Combat_Socket_RightHand)) {
        return GetMesh()->GetSocketLocation(RightHandSocketName);
    }
    if (MontageTag.MatchesTagExact(GameplayTags.Combat_Socket_Tail)) {
        return GetMesh()->GetSocketLocation(TailSocketName);
    }

    return FVector();
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
    return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
    return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
    return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
    return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTageedMontageByTag_Implementation(const FGameplayTag& MonatgeTag)
{
    for (FTaggedMontage TaggedMontage : AttackMontages) {
        if (TaggedMontage.MontageTag == MonatgeTag) {
            return TaggedMontage;
        }
    }
    return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
    return MinionCount;
}

void AAuraCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
    MinionCount += Amount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
    return CharacterClass;
}

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate()
{
    return OnAscRegistered;
}

FOnDeath& AAuraCharacterBase::GetOnDeathDelegate()
{
    return OnDeath;
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
    return Weapon;
}

bool AAuraCharacterBase::IsBeingShocked_Implementation() const
{
    return bIsBeingShocked;
}

void AAuraCharacterBase::SetIsBeingShocked_Implementation(bool bInShock)
{
    bIsBeingShocked = bInShock;
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float level) const
{
    check(IsValid(GetAbilitySystemComponent()));
    check(GameplayEffectClass);
    FGameplayEffectContextHandle Contexthandle = GetAbilitySystemComponent()->MakeEffectContext();
    Contexthandle.AddSourceObject(this);
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, level, Contexthandle);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
    ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
    ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
    ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
    UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);

    if (!HasAuthority())
        return;

    AuraASC->AddCharacterAbilities(StartupAbilities);
    AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::Dissolve()
{
    if (IsValid(DissolveMaterialInstance)) {
        UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
        GetMesh()->SetMaterial(0, DynamicMatInst);
        StartDissolveTimeline(DynamicMatInst);
    }
    if (IsValid(WeaponDissolveMaterialInstance)) {
        UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
        Weapon->SetMaterial(0, DynamicMatInst);
        StartWeaponDissolveTimeline(DynamicMatInst);
    }
}
