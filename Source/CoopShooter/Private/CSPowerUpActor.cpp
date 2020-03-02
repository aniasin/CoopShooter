// Fill out your copyright notice in the Description page of Project Settings.


#include "CSPowerUpActor.h"
#include "CoopShooter/Public/CSCharacter.h"

// Sets default values
ACSPowerUpActor::ACSPowerUpActor()
{
	SetReplicates(true);

	PowerUpInterval = 0;
	TotalNumberOfTicks = 0;

}

// Called when the game starts or when spawned
void ACSPowerUpActor::BeginPlay()
{
	Super::BeginPlay();



	
}

void ACSPowerUpActor::OnTickPowerUp()
{
	TicksProcessed++;

	OnPowerUpTicked(CurrentActor);

	if (TicksProcessed >= TotalNumberOfTicks)
	{
		OnExpired(CurrentActor);
		GetWorld()->GetTimerManager().ClearTimer(PowerUpHandle);

		Destroy();
	}
}

void ACSPowerUpActor::ActivatePowerUp()
{
	if (PowerUpInterval > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(PowerUpHandle, this, &ACSPowerUpActor::OnTickPowerUp, PowerUpInterval, true);
	}
	else
	{
		OnTickPowerUp();
	}
}

