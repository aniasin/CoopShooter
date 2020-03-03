// Fill out your copyright notice in the Description page of Project Settings.


#include "CSGameStateBase.h"
#include "Net/UnrealNetwork.h"

void ACSGameStateBase::SetWaveState(EWaveState NewWaveState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		EWaveState OldState = WaveState;
		WaveState = NewWaveState;
		OnRep_WaveState(OldState);
	}
}

void ACSGameStateBase::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void ACSGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACSGameStateBase, WaveState);
}

