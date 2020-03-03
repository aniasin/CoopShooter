// Fill out your copyright notice in the Description page of Project Settings.


#include "SCGameMode.h"
#include "CoopShooter/AI/CSTrackerBot.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ASCGameMode::ASCGameMode()
{
	NumberOfWaves = 1;
	NumberOfBots = 1;
	TimeBetwinBots = 1;
	TimeBetwinWaves = 1;
}

void ASCGameMode::StartWave()
{
	CurrentNumberOfWaves++;

	if (CurrentNumberOfWaves >= NumberOfWaves + 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(BotSpawnerTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(TimeBetwinWaveTimerHandle);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(BotSpawnerTimerHandle, this, &ASCGameMode::SpawnNextBot, TimeBetwinBots, true, 0);
	}
}

void ASCGameMode::EndWave()
{
	NumberOfBotsSpawned = 0;
	NumberOfBots++;

	GetWorld()->GetTimerManager().ClearTimer(BotSpawnerTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(TimeBetwinWaveTimerHandle, this, &ASCGameMode::CheckWaveState, TimeBetwinWaves, true, 0);
}

void ASCGameMode::SpawnNextBot()
{
	NumberOfBotsSpawned++;

	if (NumberOfBotsSpawned >= NumberOfBots)
	{
		EndWave();
	}

	UE_LOG(LogTemp, Log, TEXT("Current WaveNumbr = %s"), *FString::FromInt(CurrentNumberOfWaves))

	SPawnNewBot();
}

void ASCGameMode::CheckWaveState()
{
	UE_LOG(LogTemp, Warning, TEXT("Checking for next wave ..."))
	TArray<AActor*> TrackerBots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSTrackerBot::StaticClass(), TrackerBots);

	if (TrackerBots.Num() <= 0)
	{
		StartWave();
	}

}
