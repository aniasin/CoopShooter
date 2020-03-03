// Fill out your copyright notice in the Description page of Project Settings.


#include "CSPickUpActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "CoopShooter/Public/CSPowerUpActor.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "Net/UnrealNetwork.h"
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

	RespawnTime = 10;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ACSPickUpActor::BeginPlay()
{
	Super::BeginPlay();

	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No power up assigned in %s !"), *GetName())
			return;
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		Respawn();
	}
}


void ACSPickUpActor::Respawn()
{

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PowerUp = GetWorld()->SpawnActor<ACSPowerUpActor>(PowerUpClass, GetActorTransform(), SpawnParams);
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

}

void ACSPickUpActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ACSCharacter* Player = Cast<ACSCharacter>(OtherActor);
	if (Player)
	{
		if (!PowerUp) { return; }
		if (GetLocalRole() == ROLE_Authority)
		{
			GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ACSPickUpActor::Respawn, RespawnTime, false);
			SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			PowerUp->CurrentActor = OtherActor;
			PowerUp->ActivatePowerUp();
		}
	}


}
