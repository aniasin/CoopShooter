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
	OnActivated(CurrentActor);

	if (PowerUpInterval > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(PowerUpHandle, this, &ACSPowerUpActor::OnTickPowerUp, PowerUpInterval, true);
	}
	else
	{
		OnTickPowerUp();
	}
}

