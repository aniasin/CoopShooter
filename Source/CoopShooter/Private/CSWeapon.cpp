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
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

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
	WeaponRange = 10000;
	BulletSpread = 1;

	SetReplicates(true);
}

void ACSWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ACSWeapon::Fire()
{
	// Trace from pawn eyes to crosshair location
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}


	AActor* OwnerActor = GetOwner();

	LastFireTime = GetWorld()->GetTimeSeconds();

	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();

	// Bullet Spread
if (OwnerActor->GetVelocity().Size() > 0)
{
	BulletSpread = 4;
}
else
{
	BulletSpread = 0;
}
	float HalfRad = FMath::DegreesToRadians(BulletSpread);
	ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

	FVector TraceEnd = EyeLocation + (ShotDirection * WeaponRange);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerActor);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FVector TracerEndPoint = TraceEnd;
	
	EPhysicalSurface SurfaceType = SurfaceType_Default;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
	{
		// Blocking hits ! process Damage
		AActor* HitActor = Hit.GetActor();

		SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

		float ActualDamage = BaseDamage;
		if (SurfaceType == SURFACE_FLESHVULNERABLE)
		{
			ActualDamage *= 4.0f;
		}

		UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, OwnerActor->GetInstigatorController(), OwnerActor, DamageType);
		PlayImpact(SurfaceType, Hit.ImpactPoint);

		TracerEndPoint = Hit.ImpactPoint;
	}


	if (DebugWeaponDrawing > 0)
	{
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 5.0f, 0, 1.0f);
	}

	PlayFireEffects(TracerEndPoint);

	if (GetLocalRole() == ROLE_Authority)
	{
		HitScanTrace.TraceTo = TracerEndPoint;
		HitScanTrace.SurfaceType = SurfaceType;
	}
}

void ACSWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ACSWeapon::ServerFire_Validate()
{
	return true;
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
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
	}
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShellSound, GetActorLocation());
	}

	FVector MuzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	if (TracerEffect)
	{
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzleLocation);
		TracerComp->SetVectorParameter(TracerTargetName, EndPoint);
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) { return; }
	APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PlayerController) { return; }
	PlayerController->ClientPlayCameraShake(FireCameraShake);
}

void ACSWeapon::PlayImpact(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	USoundCue* SelectedSound = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		SelectedSound = HitSoundFlesh;
		break;
	case SURFACE_EXPLOSIVE:
		SelectedEffect = ExplosiveImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		SelectedSound = HitSoundConcrete;
		break;
	}
	FVector ShotDirection = ImpactPoint - MeshComp->GetSocketLocation(MuzzleSocketName);
	ShotDirection.Normalize();

	if (SelectedEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
	if (SelectedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SelectedSound, ImpactPoint);
	}
}

void ACSWeapon::OnRep_HitScanTrace()
{
	// play cosmetics
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpact(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ACSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACSWeapon, HitScanTrace, COND_SkipOwner);
}