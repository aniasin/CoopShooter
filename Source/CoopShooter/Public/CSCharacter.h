// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSCharacter.generated.h"

class USCHealthComponent;

UCLASS()
class COOPSHOOTER_API ACSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACSCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USCHealthComponent* HealthComp;

	void AIFire();
	void AIStopFire();

	UPROPERTY(EditAnywhere, Category = "Player")
	bool bIsAiControlled;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void BeginCrouch();
	void EndCrouch();
	void StartJump();
	void StopJump();

	void Aim();
	void StopAim();
	void Fire();
	void StopFire();


	UPROPERTY(Replicated)
		class ACSWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ACSWeapon> StarterWeaponClass;

	float DefaultFov;

	UFUNCTION()
		void OnHealthChanged(USCHealthComponent* HealthComponent, float CurrentHealth, float HealthDelta,
			const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Event")
	bool bIsDead;

	// Used to sort
	float DistanceToQuerier;

	UFUNCTION(BlueprintCallable, Category = "Event")
	void ChangeMaxWalkSpeed(float NewSpeed);

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable, Category = "Event")
		void ClientChangeMaxWalkSpeed(float NewSpeed);

private:

	FTimerHandle FAIFire_TimerHandle;

};
