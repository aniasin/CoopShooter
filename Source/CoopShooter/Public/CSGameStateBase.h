// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CSGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	PreparingNextWave,
	WaveInProgress,
	WaitingToComplete,
	GameOver,
};

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API ACSGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(ReplicatedUsing = OnRep_WaveState, BlueprintReadOnly, Category = "GameState")
		EWaveState WaveState;

	void SetWaveState(EWaveState NewWaveState);
	
protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);
};
