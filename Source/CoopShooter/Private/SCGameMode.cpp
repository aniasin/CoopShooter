// Fill out your copyright notice in the Description page of Project Settings.


#include "SCGameMode.h"
#include "CoopShooter/AI/CSTrackerBot.h"
#include "Kismet/GameplayStatics.h"
#include "CoopShooter/Public/CSGameStateBase.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "Engine/World.h"

ASCGameMode::ASCGameMode()
{
	GameStateClass = ACSGameStateBase::StaticClass();

	NumberOfWaves = 1;
	NumberOfBots = 1;
	TimeBetwinBots = 1;
	TimeBetwinWaves = 1;
	bIsGameOver = false;
}

void ASCGameMode::StartWave()
{
	if (bIsGameOver) { return; }
	CurrentNumberOfWaves++;

	if (CurrentNumberOfWaves >= NumberOfWaves + 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(BotSpawnerTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(TimeBetwinCheckWaveState);
	}
	else
	{
		SetWaveState(EWaveState::PreparingNextWave);
		GetWorld()->GetTimerManager().SetTimer(BotSpawnerTimerHandle, this, &ASCGameMode::SpawnNextBot, TimeBetwinBots, true, TimeBetwinWaves);
	}
}

void ASCGameMode::EndWave()
{
	NumberOfBotsSpawned = 0;
	NumberOfBots++;
	GetWorld()->GetTimerManager().ClearTimer(BotSpawnerTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(TimeBetwinCheckWaveState, this, &ASCGameMode::CheckWaveState, 1, true);
}

void ASCGameMode::SpawnNextBot()
{
	NumberOfBotsSpawned++;

	if (NumberOfBotsSpawned >= NumberOfBots)
	{
		EndWave();
	}
	else
	{
		SetWaveState(EWaveState::WaveInProgress);
		SPawnNewBot();
	}
}

void ASCGameMode::CheckWaveState()
{
	SetWaveState(EWaveState::WaitingToComplete);
	UE_LOG(LogTemp, Warning, TEXT("Checking for next wave ..."))
	TArray<AActor*> TrackerBots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSTrackerBot::StaticClass(), TrackerBots);

	if (TrackerBots.Num() <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeBetwinCheckWaveState);
		StartWave();
	}

}

void ASCGameMode::ActorKilled(AActor* Killer, AActor* Victim, AController* InstigatorController)
{
	ACSCharacter* Player = Cast<ACSCharacter>(Victim);
	if (Player)
	{
		TArray<AActor*>Players;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSCharacter::StaticClass(), Players);
		if (Players.Num() > 0)
		{
			// check if alive
			for (int32 i = 0; i < Players.Num(); i++)
			{
				ACSCharacter* CurrentPlayer = Cast<ACSCharacter>(Players[i]);
				if (CurrentPlayer)
				{
					AController* Controller = CurrentPlayer->GetController();
					if (Controller)
					{
						break;
					}
					else
					{
						SetWaveState(EWaveState::GameOver);
						GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
					}
				}
			}
		}
		else
		{
			SetWaveState(EWaveState::GameOver);
			GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		}
	}
}

void ASCGameMode::SetWaveState(EWaveState NewWaveState)
{
	ACSGameStateBase* MyGameState = GetGameState<ACSGameStateBase>();

	if (ensure(MyGameState))
	{
		MyGameState->SetWaveState(NewWaveState);
	}
}

