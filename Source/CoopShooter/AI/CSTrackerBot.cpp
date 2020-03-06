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
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
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

	HealthComp = CreateDefaultSubobject<USCHealthComponent>(TEXT("HealthComp"));

	AudioComp = CreateDefaultSubobject <UAudioComponent>(TEXT("AudioComponent"));

	bUseVelocityChange = true;
	MovementForce = 1000;
	RequiredistanceToTarget = 100;
	Damage = 65;
	DamageRadius = 250;
	Timer = 1;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACSTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(HealthComp)) { return; }

	HealthComp->OnHealthChanged.AddDynamic(this, &ACSTrackerBot::HandleTakeDamage);
	
	if (GetLocalRole() == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();
	}
}

// Called every frame
void ACSTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority && !bExploded)
	{
		MoveToNextPoint();
	}
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

	TArray<AActor*> SpottedPlayers;
	TArray<ACSCharacter*>PlayersToSort;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSCharacter::StaticClass(), SpottedPlayers);
	if (SpottedPlayers.Num() > 0)
	{
		for (int32 i = 0; i < SpottedPlayers.Num(); i++)
		{
			float Distance = FVector::Distance(SpottedPlayers[i]->GetActorLocation(), GetActorLocation());
			ACSCharacter* PlayerCharacter = Cast<ACSCharacter>(SpottedPlayers[i]);
			PlayerCharacter->DistanceToQuerier = Distance;
			PlayersToSort.Add(PlayerCharacter);
		}
		TArray<ACSCharacter*>PlayersSorted = SortPlayersByDistance(PlayersToSort);
		BestTarget = PlayersSorted[0];
	}

	if (BestTarget)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);


		if (NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
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

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	AudioComp->SetVolumeMultiplier(0);
	UGameplayStatics::PlaySoundAtLocation(this, SoundExplosion, GetActorLocation());

	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*>IgnoreActor;
		IgnoreActor.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation(), DamageRadius, BotDamageType, IgnoreActor, this,
			GetController(), true, COLLISION_WEAPON);

		DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 12, FColor::Red, false, 1, 0, 1);

		GetWorldTimerManager().ClearAllTimersForObject(this);
		DetachFromControllerPendingDestroy();
		SetLifeSpan(3.0f);
	}
}

void ACSTrackerBot::SelfDamage()
{
	UGameplayStatics::ApplyDamage(this, 20, GetController(), this, BotDamageType);
	UGameplayStatics::SpawnSoundAttached(SoundWarning, RootComponent);
}

void ACSTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HealthComp->IsFriendly(OtherActor, this))
	{
		return;
	}

	ACSCharacter* Player = Cast<ACSCharacter>(OtherActor);
	if (Player && !bStartedSelfDestroy)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			GetWorldTimerManager().SetTimer(BotTimerHandle, this, &ACSTrackerBot::SelfDamage, Timer, true, 0);
		}

		bStartedSelfDestroy = true;
		UGameplayStatics::SpawnSoundAttached(SoundSpotted, RootComponent);
	}

}

void ACSTrackerBot::NotifyActorEndOverlap(AActor* OtherActor)
{

}

// tools
TArray<ACSCharacter*> ACSTrackerBot::SortPlayersByDistance(TArray<ACSCharacter*> PlayerToSort)
{
	auto SortPred = [](ACSCharacter& A, ACSCharacter& B)->bool
	{
		return(A.DistanceToQuerier) <= (B.DistanceToQuerier);
	};
	PlayerToSort.Sort(SortPred);

	return PlayerToSort;
}
