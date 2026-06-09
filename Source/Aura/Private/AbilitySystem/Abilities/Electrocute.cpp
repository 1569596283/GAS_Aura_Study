// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Electrocute.h"

FString UElectrocute::GetDescription(int32 Level)
{

    const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);
    if (Level == 1) {
        return FString::Printf(TEXT("<Title>ELECTROCUTE</>\n\n"
            // Level
            "<Small>Level: </><Level>%d</>\n"
            // ManaCost
            "<Small>ManaCost: </><ManaCost>%.1f </>\n"
            // Cooldown
            "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
            // Damage
            "<Default>Emits a beam of lightning, connecting with the target, repeatedly causing </><Damage>%d </>"
            "<Default>lightning damage with a chance to stun</>"),
            Level, ManaCost, Cooldown, ScaledDamage);
    }
    else {
        return FString::Printf(TEXT("<Title>ELECTROCUTE</>\n\n"
            // Level
            "<Small>Level: </><Level>%d</>\n"
            // ManaCost
            "<Small>ManaCost: </><ManaCost>%.1f </>\n"
            // Cooldown
            "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
            // Number, Damage
            "<Default>Emits a beam of lightning, propagating to %d additional targets nearby, causing </><Damage>%d </>"
            "<Default>lightning damage with a chance to stun</>"),
            Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShockTargets), ScaledDamage);
    }
}

FString UElectrocute::GetNextLevelDescription(int32 Level)
{
    const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);

    return FString::Printf(TEXT("<Title>NEXT LEVEL</>\n\n"
        // Level
        "<Small>Level: </><Level>%d</>\n"
        // ManaCost
        "<Small>ManaCost: </><ManaCost>%.1f </>\n"
        // Cooldown
        "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
        // Number, Damage
        "<Default>Emits a beam of lightning, propagating to %d additional targets nearby, causing </><Damage>%d </>"
        "<Default>lightning damage with a chance to stun</>"),
        Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShockTargets), ScaledDamage);
}