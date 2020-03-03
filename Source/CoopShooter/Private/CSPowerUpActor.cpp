// Fill out your copyright notice in the Description page of Project Settings.


#include "CSPowerUpActor.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"

// Sets default values
ACSPowerUpActor::ACSPowerUpActor()
{
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetRelativeLocation(FVector(0, 0, 40));
	MeshComp->SetIsReplicated(true);

	LightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightComp"));
	LightComp->SetupAttachment(MeshComp);
	LightComp->SetAttenuationRadius(200);
	LightComp->CastShadows = false;
	LightComp->SetIsReplicated(true);


	SetReplicates(true);

	PowerUpInterval = 10;
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
	if (!MeshComp) { return; }
	MeshComp->SetVisibility(false, true);
}

