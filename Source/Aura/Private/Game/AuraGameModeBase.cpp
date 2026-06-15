// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadScreenSaveGame.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
    if (UGameplayStatics::DoesSaveGameExist(LoadSlot->LoadSlotName, SlotIndex)) {
        UGameplayStatics::DeleteGameInSlot(LoadSlot->LoadSlotName, SlotIndex);
    }
    USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
    ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
    LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
    LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
    LoadScreenSaveGame->SaveSlotStatus = Taken;

    UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);

}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
    USaveGame* SaveGameObject = nullptr;
    if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex)) {
        SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
    }
    else {
        SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
    }
    ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
    return LoadScreenSaveGame;
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
    if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex)) {
        UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
    }
}

void AAuraGameModeBase::TraveToMap(UMVVM_LoadSlot* Slot)
{
    const  FString SlotName = Slot->LoadSlotName;
    const int32 SlotIndex = Slot->SlotIndex;

    UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

void AAuraGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    Maps.Add(DefaultMapName, DefaultMap);
}
