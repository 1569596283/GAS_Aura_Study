// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& Info : AttributeInformation) {
		if (Info.AttributeTag.MatchesTagExact(AttributeTag)) {
			return Info;
		}
	}

	if (bLogNotFound) {
		UE_LOG(LogTemp, Error, TEXT("没有找到属性标签[%s]"), *AttributeTag.ToString());
	}

	return FAuraAttributeInfo();
}
