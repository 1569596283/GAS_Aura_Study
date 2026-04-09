// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	// 敌人接口
	virtual void HighlighActor() override;
	virtual void UnHighlightActor() override;
	// 敌人接口

	// 战斗接口
	virtual int32 GetPlayerLevel() override;
	 
	// 战斗接口

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Character Class Defaults")
	int32 Level = 1;

};
