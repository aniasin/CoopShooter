// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SCGameMode.generated.h"


enum class EWaveState : uint8;
/**
 * 
 */
UCLASS()
class COOPSHOOTER_API ASCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASCGameMode();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void ActorKilled(AActor* Killer, AActor* Victim, AController* InstigatorController);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
		void K2_ActorKilled(AActor* Killer, AActor* Victim, AController* InstigatorController);

protected:

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void StartWave();
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
		void SPawnNewBot();

	void EndWave();
	void SpawnNextBot();
	void PrepareNextWave();

	void RestartDeadPlayers();

	FTimerHandle ResPawnPlayer_TimerHandle;

	void CheckWaveState();

	void SetWaveState(EWaveState NewWaveState);

	FTimerHandle BotSpawnerTimerHandle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ClampMin = 1), Category = "Waves Settings")
	float TimeBetwinBots;

	FTimerHandle TimeBetwinCheckWaveState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ClampMin = 1), Category = "Waves Settings")
	float TimeBetwinWaves;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ClampMin = 1), Category = "Waves Settings")
		int32 NumberOfBots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ClampMin = 1), Category = "Waves Settings")
		int32 NumberOfWaves;

	void CheckGameOver();
	void GameOver();

private:

	bool bIsGameOver;

	int32 NumberOfBotsSpawned;
	int32 CurrentNumberOfWaves;
};
