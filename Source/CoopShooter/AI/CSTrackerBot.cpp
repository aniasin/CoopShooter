// Fill out your copyright notice in the Description page of Project Settings.


#include "CSTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "CoopShooter/Components/SCHealthComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/DamageType.h"
#include "CoopShooter/CoopShooter.h"
#include "Components/SphereComponent.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ACSTrackerBot::ACSTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->SetSphereRadius(150);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CollisionSphere->SetGenerateOverlapEvents(true);

	AudioComp = CreateDefaultSubobject <UAudioComponent>(TEXT("AudioComponent"));

	HealthComp = CreateDefaultSubobject<USCHealthComponent>(TEXT("HealthComp"));

	bUseVelocityChange = true;
	MovementForce = 1000;
	RequiredistanceToTarget = 100;
	Damage = 65;
	DamageRadius = 250;
	Timer = 1;

}

// Called when the game starts or when spawned
void ACSTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->OnHealthChanged.AddDynamic(this, &ACSTrackerBot::HandleTakeDamage);
	
	NextPathPoint =  GetNextPathPoint();
}

// Called every frame
void ACSTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToNextPoint();
}

void ACSTrackerBot::HandleTakeDamage(USCHealthComponent* HealthComponent, float CurrentHealth, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode if health 0 or reached player
	if (CurrentHealth <= 0)
	{
		SelfDestruct();
	}

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->GetTimeSeconds());
	}
}

FVector ACSTrackerBot::GetNextPathPoint()
{
	if (bExploded)
	{
		return GetActorLocation();
	}

	ACharacter* TargetActor = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), TargetActor);

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}
	return GetActorLocation();
}

void ACSTrackerBot::MoveToNextPoint()
{
	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredistanceToTarget && !bExploded)
	{
		NextPathPoint = GetNextPathPoint();
	}
	else if (!bExploded)
	{
		// Keep Moving
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;
		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		// Tweak rolling sound according to velocity
		float CurrentVelocity = GetVelocity().Size();
		float VolumeToSet = FMath::GetMappedRangeValueClamped(FVector2D(50, 1000), FVector2D(0, 1.5), CurrentVelocity);
		AudioComp->SetVolumeMultiplier(VolumeToSet);
	}
}

void ACSTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}
	bExploded = true;
	TArray<AActor*>IgnoreActor;
	IgnoreActor.Add(this);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	AudioComp->SetVolumeMultiplier(0);
	UGameplayStatics::PlaySoundAtLocation(this, SoundExplosion, GetActorLocation());

	UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation(), DamageRadius, BotDamageType, IgnoreActor, this,
		GetController(), true, COLLISION_WEAPON);

	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 12, FColor::Red, false, 1, 0, 1);

	GetWorldTimerManager().ClearAllTimersForObject(this);
	DetachFromControllerPendingDestroy();
	SetLifeSpan(10.0f);
}

void ACSTrackerBot::SelfDamage()
{
	UGameplayStatics::ApplyDamage(this, 20, GetController(), this, BotDamageType);
	UGameplayStatics::SpawnSoundAttached(SoundWarning, RootComponent);
}

void ACSTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ACSCharacter* Player = Cast<ACSCharacter>(OtherActor);
	if (Player && !bStartedSelfDestroy)
	{
		GetWorldTimerManager().SetTimer(BotTimerHandle, this, &ACSTrackerBot::SelfDamage, Timer, true, 0);
		bStartedSelfDestroy = true;
		UGameplayStatics::SpawnSoundAttached(SoundSpotted, RootComponent);
	}

}

void ACSTrackerBot::NotifyActorEndOverlap(AActor* OtherActor)
{

}

