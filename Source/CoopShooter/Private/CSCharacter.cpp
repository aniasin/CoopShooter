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

// Sets default values
ACSCharacter::ACSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	HealthComp = CreateDefaultSubobject<USCHealthComponent>(TEXT("HealthComp"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

}

// Called when the game starts or when spawned
void ACSCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFov = Camera->FieldOfView;

	// Spawn a default Weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ACSWeapon>(StarterWeaponClass, FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
		CurrentWeapon->SetOwner(this);
	}
	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ACSCharacter::OnHealthChanged);
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
	if (!CurrentWeapon) { return; }
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
	UE_LOG(LogTemp, Warning, TEXT("Took %s Damage"), *FString::SanitizeFloat(HealthDelta))
		if (CurrentHealth <= 0 || bIsDead)
		{
			bIsDead = true;
			GetMovementComponent()->StopMovementImmediately();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			DetachFromControllerPendingDestroy();
			SetLifeSpan(10.0f);
		}
}

// Called every frame
void ACSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

FVector ACSCharacter::GetPawnViewLocation() const
{
	if (Camera)
	{
		return Camera->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

