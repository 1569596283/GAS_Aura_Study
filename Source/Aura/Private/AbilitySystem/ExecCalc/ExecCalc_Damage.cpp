// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include <AbilitySystem/AuraAbilitySystemLibrary.h>
#include <Interaction/CombatInterface.h>
#include <AuraAbilityTypes.h>

struct AuraDamageStatics {
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);

    DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

    AuraDamageStatics() {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);

        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
    }
};

static const AuraDamageStatics& DamageStatics() {
    static AuraDamageStatics DStatics;
    return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
    RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
    RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectSpec& Spec, const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters& EvaluationParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDefs) const
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
    // 负面效果
    for (const TPair<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToDebuffs) {
        const FGameplayTag& DamageType = Pair.Key;
        const FGameplayTag& DebuffType = Pair.Value;
        const float TypeDamage = Spec.GetSetByCallerMagnitude(Pair.Key, false, -1.f);
        if (TypeDamage > -.5f) {
            // 判断是否添加负面效果
            const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);

            float TargetDebuffResistance = 0.f;
            const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageType];
            ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToDefs[ResistanceTag], EvaluationParameters, TargetDebuffResistance);
            TargetDebuffResistance = FMath::Max<float>(TargetDebuffResistance, 0.f);
            const float EffectiveDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100.f;
            const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
            if (bDebuff) {
                FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
                UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
                UAuraAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);

                const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
                const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
                const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);

                UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
                UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
                UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
            }
        }
    }
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
    const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_Armor, DamageStatics().ArmorDef);
    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);

    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
    TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);

    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    int32 SourcePlayerLevel = 1;
    if (SourceAvatar->Implements<UCombatInterface>()) {
        SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
    }
    int32 TargetPlayerLevel = 1;
    if (TargetAvatar->Implements<UCombatInterface>()) {
        TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
    }


    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    DetermineDebuff(Spec, ExecutionParams, EvaluationParameters, TagsToCaptureDefs);

    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
    // 伤害由调用者的幅度设定
    float Damage = 0;
    for (const TPair<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToResistances) {
        const FGameplayTag DamageTypeTag = Pair.Key;
        const FGameplayTag ResistanceTag = Pair.Value;

        checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
        const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTag];

        float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);

        if (DamageTypeValue <= 0.f) {
            continue;
        }

        float Resistance = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
        Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

        DamageTypeValue *= (100.f - Resistance) / 100.f;

        if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle)) {
            if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar)) {
                FDelegateHandle Handle = CombatInterface->GetOnDamageSignature().AddLambda([&](float DamageAmount) {
                    DamageTypeValue = DamageAmount;
                    CombatInterface->GetOnDamageSignature().Remove(Handle);
                    });
            }
        }
        UGameplayStatics::ApplyRadialDamageWithFalloff(TargetAvatar, DamageTypeValue, 0.f, UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle), UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle), UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle), 1.f, UDamageType::StaticClass(), TArray<AActor*>(), SourceAvatar, nullptr);

        Damage += DamageTypeValue;
    }

    // 捕获目标的格挡几率，判断是否成功格挡
    // 如果成功格挡，伤害减半

    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.0f);

    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

    UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

    Damage = bBlocked ? Damage / 2 : Damage;

    // 护甲穿透无视护甲
    float TargetArmor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
    TargetArmor = FMath::Max<float>(TargetArmor, 0.0f);

    float SourceArmorPenetration = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
    SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.0f);

    const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
    const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
    const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
    const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

    const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
    const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);
    // 护甲会减少受到的伤害
    Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;


    // 暴击
    float SourceCriticalHitChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
    SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
    float TargetCriticalHitResistance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
    TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);
    float SourceCriticalHitDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
    SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

    const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
    const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

    const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
    const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

    UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

    Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;

    const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
