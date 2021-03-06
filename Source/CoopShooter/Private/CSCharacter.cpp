// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopShooter/Public/CSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CoopShooter/Public/CSWeapon.h"
#include "Engine/World.h"
#include "CoopShooter/CoopShooter.h"
#include "CoopShooter/Components/SCHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACSCharacter::ACSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;


	HealthComp = CreateDefaultSubobject<USCHealthComponent>(TEXT("HealthComp"));
}

// Called to bind functionality to input
void ACSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ACSCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACSCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACSCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACSCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACSCharacter::StopJump);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACSCharacter::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACSCharacter::StopAim);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACSCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACSCharacter::StopFire);
}

// Called when the game starts or when spawned
void ACSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(HealthComp)) { return; }

	HealthComp->OnHealthChanged.AddDynamic(this, &ACSCharacter::OnHealthChanged);
	DefaultFov = Camera->FieldOfView;

	if (GetLocalRole() == ROLE_Authority)
	{
	// Spawn a default Weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ACSWeapon>(StarterWeaponClass, FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
		if (CurrentWeapon)
		{
			if (bIsAiControlled)
			{
				CurrentWeapon->DefaultBulletSpread = 10;
			}
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
			CurrentWeapon->SetOwner(this);

		}
	}
}

void ACSCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ACSCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ACSCharacter::BeginCrouch()
{
	Crouch();
}

void ACSCharacter::EndCrouch()
{
	UnCrouch();
}

void ACSCharacter::StartJump()
{
	Jump();
}

void ACSCharacter::StopJump()
{
	StopJumping();
}

void ACSCharacter::Aim()
{
	
}

void ACSCharacter::StopAim()
{
	
}

void ACSCharacter::Fire()
{
	if (!CurrentWeapon || bIsDead) { return; }
	CurrentWeapon->StartFire();
}

void ACSCharacter::StopFire()
{
	if (!CurrentWeapon) { return; }
	CurrentWeapon->StopFire();
}

void ACSCharacter::OnHealthChanged(USCHealthComponent* HealthComponent, float CurrentHealth, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
		if (CurrentHealth <= 0 || bIsDead)
		{
			bIsDead = true;
			if (CurrentWeapon)
			{
				CurrentWeapon->Destroy();
				// TODO Spawn Weapon Pick up in place
			}
			GetMovementComponent()->StopMovementImmediately();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			DetachFromControllerPendingDestroy();
			SetLifeSpan(3);
		}
}

// Called every frame
void ACSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ACSCharacter::GetPawnViewLocation() const
{
	if (Camera)
	{
		return Camera->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void ACSCharacter::ChangeMaxWalkSpeed(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void ACSCharacter::ClientChangeMaxWalkSpeed_Implementation(float NewSpeed)
{
	ChangeMaxWalkSpeed(NewSpeed);
}

bool ACSCharacter::ClientChangeMaxWalkSpeed_Validate(float NewSpeed)
{
	return true;
}

ACSWeapon* ACSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

////////// AI
void ACSCharacter::AIFire()
{
	if (!CurrentWeapon || bIsDead) { return; }
	Fire();
	GetWorld()->GetTimerManager().SetTimer(FAIFire_TimerHandle, this, &ACSCharacter::AIStopFire, 1, false);
}

void ACSCharacter::AIStopFire()
{
	StopFire();
}


void ACSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACSCharacter, CurrentWeapon);
	DOREPLIFETIME(ACSCharacter, bIsDead);
}
