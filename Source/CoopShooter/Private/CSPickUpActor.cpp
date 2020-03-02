// Fill out your copyright notice in the Description page of Project Settings.


#include "CSPickUpActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "CoopShooter/Public/CSPowerUpActor.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "Engine/World.h"

// Sets default values
ACSPickUpActor::ACSPickUpActor()
{

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75);
	RootComponent = SphereComp;
	
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->DecalSize = FVector(64,75,75);
	DecalComp->SetRelativeRotation(FRotator(90, 0, 0));
	DecalComp->SetupAttachment(RootComponent);
	DecalComp->SetVisibility(false);

	RespawnTime = 10;

}

// Called when the game starts or when spawned
void ACSPickUpActor::BeginPlay()
{
	Super::BeginPlay();
	Respawn();
}


void ACSPickUpActor::Respawn()
{
	DecalComp->SetVisibility(true);
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
	PowerUp = GetWorld()->SpawnActor<ACSPowerUpActor>(PowerUpClass, GetActorLocation(), GetActorRotation());

}

void ACSPickUpActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ACSCharacter* Player = Cast<ACSCharacter>(OtherActor);
	if (Player)
	{
		DecalComp->SetVisibility(false);

		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ACSPickUpActor::Respawn, RespawnTime, false);
		PowerUp->OnActivated(Player);
		PowerUp->CurrentActor = Player;
		PowerUp->ActivatePowerUp();
	}

}
