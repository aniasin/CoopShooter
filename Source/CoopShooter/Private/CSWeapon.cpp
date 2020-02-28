// Fill out your copyright notice in the Description page of Project Settings.


#include "CSWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraShake.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopShooter/CoopShooter.h"
#include "TimerManager.h"
#include "Engine/World.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines For Weapons"), ECVF_Cheat);

// Sets default values
ACSWeapon::ACSWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;
	RateOfFire = 600.0f;
}

void ACSWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ACSWeapon::Fire()
{
	// Trace from pawn eyes to crosshair location
	AActor* OwnerActor = GetOwner();

	LastFireTime = GetWorld()->GetTimeSeconds();

	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();

	FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerActor);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FVector TracerEndPoint = TraceEnd;
	

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
	{
		// Blocking hits ! process Damage
		AActor* HitActor = Hit.GetActor();

		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

		float ActualDamage = BaseDamage;
		if (SurfaceType == SURFACE_FLESHVULNERABLE)
		{
			ActualDamage *= 4.0f;
		}

		UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit,
			OwnerActor->GetInstigatorController(), this, DamageType);

		UParticleSystem* SelectedEffect = nullptr;
		switch (SurfaceType)
		{
		case SURFACE_FLESHDEFAULT:
			SelectedEffect = FleshImpactEffect;
				break;
		case SURFACE_FLESHVULNERABLE:
			SelectedEffect = FleshImpactEffect;
				break;
		default:
			SelectedEffect = DefaultImpactEffect;
			break;
		}
		if (!SelectedEffect) { return; }
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

		TracerEndPoint = Hit.ImpactPoint;
	}


	if (DebugWeaponDrawing > 0)
	{
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 5.0f, 0, 1.0f);
	}

	PlayFireEffects(TracerEndPoint);
}

void ACSWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ACSWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ACSWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ACSWeapon::PlayFireEffects(FVector EndPoint)
{
	if (!MuzzleEffect) { return; }
	UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);

	FVector MuzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	if (!TracerEffect) { return; }
	UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzleLocation);

	if (!TracerComp) { return; }
	TracerComp->SetVectorParameter(TracerTargetName, EndPoint);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) { return; }
	APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PlayerController) { return; }
	PlayerController->ClientPlayCameraShake(FireCameraShake);
}
