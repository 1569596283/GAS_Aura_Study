// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ArcaneShards.h"

FString UArcaneShards::GetDescription(int32 Level)
{

    const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);
    if (Level == 1) {
        return FString::Printf(TEXT("<Title>ARCANE SHARDS</>\n\n"
            // Level
            "<Small>Level: </><Level>%d</>\n"
            // ManaCost
            "<Small>ManaCost: </><ManaCost>%.1f </>\n"
            // Cooldown
            "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
            // Damage
            "<Default>Summon a shard of arcane enery, causing radial aracne damage of </><Damage>%d </>"
            "<Default>at the shard origin.</>"),
            Level, ManaCost, Cooldown, ScaledDamage);
    }
    else {
        return FString::Printf(TEXT("<Title>ARCANE SHARDS</>\n\n"
            // Level
            "<Small>Level: </><Level>%d</>\n"
            // ManaCost
            "<Small>ManaCost: </><ManaCost>%.1f </>\n"
            // Cooldown
            "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
            // Number, Damage
            "<Default>Summon %d shards of arcane enery, causing radial aracne damage of </><Damage>%d </>"
            "<Default>at the shard origins.</>"),
            Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShards), ScaledDamage);
    }
}

FString UArcaneShards::GetNextLevelDescription(int32 Level)
{
    const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
    const float ManaCost = FMath::Abs(GetManaCost(Level));
    const float Cooldown = GetCooldown(Level);
    return FString::Printf(TEXT("<Title>ARCANE SHARDS</>\n\n"
        // Level
        "<Small>Level: </><Level>%d</>\n"
        // ManaCost
        "<Small>ManaCost: </><ManaCost>%.1f </>\n"
        // Cooldown
        "<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
        // Number, Damage
        "<Default>Summon %d shards of arcane enery, causing radial aracne damage of </><Damage>%d </>"
        "<Default>at the shard origins.</>"),
        Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShards), ScaledDamage);
}