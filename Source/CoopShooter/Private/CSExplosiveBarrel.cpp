// Fill out your copyright notice in the Description page of Project Settings.


#include "CSExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "CoopShooter/Components/SCHealthComponent.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ACSExplosiveBarrel::ACSExplosiveBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	HealthComponent = CreateDefaultSubobject<USCHealthComponent>(TEXT("HealthComponent"));
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACSExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(HealthComponent)) { return; }

	HealthComponent->OnHealthChanged.AddDynamic(this, &ACSExplosiveBarrel::OnHealthChanged);
	
}

void ACSExplosiveBarrel::OnHealthChanged(USCHealthComponent* HealthComp, float CurrentHealth, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("EXPLOSION !"))

		if (bExploded)
		{
			return;
		}
	bExploded = true;
	OnRep_Exploded();
}

void ACSExplosiveBarrel::OnRep_Exploded()
{
	UGameplayStatics::SpawnEmitterAttached(ExplosiveImpactEffect, MeshComp);
}

void ACSExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACSExplosiveBarrel, bExploded);
}