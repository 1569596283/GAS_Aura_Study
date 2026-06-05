// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"
#include <AbilitySystemBlueprintLibrary.h>

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
    bAutoActivate = false;

}

void UPassiveNiagaraComponent::BeginPlay()
{
    if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))) {
        AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
    }
    else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner())) {
        CombatInterface->GetOnASCRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC) {
            if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC)) {
                AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
            }
            });
    }
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
    if (AbilityTag.MatchesTagExact(PassiveSpellTag)) {
        if (bActivate && !IsActive()) {
            Activate();
        }
        else {
            Deactivate();
        }
    }
}
