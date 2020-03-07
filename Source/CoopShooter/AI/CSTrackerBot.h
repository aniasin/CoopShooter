// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CSTrackerBot.generated.h"

class USCHealthComponent;

UCLASS()
class COOPSHOOTER_API ACSTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACSTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USCHealthComponent* HealthComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USphereComponent* CollisionSphere;
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		class UAudioComponent* AudioComp;
	

	UFUNCTION()
	void HandleTakeDamage(USCHealthComponent* HealthComponent, float CurrentHealth, float HealthDelta,
		const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	virtual void NotifyActorBeginOverlap(AActor* OtherActor)override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor)override;

	FVector GetNextPathPoint();
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackeBot" )
	float MovementForce;
	UPROPERTY(EditDefaultsOnly, Category = "TrackeBot")
	bool bUseVelocityChange;
	UPROPERTY(EditDefaultsOnly, Category = "TrackeBot")
	float RequiredistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "TrackeBot|Sounds")
		class USoundCue* SoundSpotted;
	UPROPERTY(EditDefaultsOnly, Category = "TrackeBot|Sounds")
		class USoundCue* SoundExplosion;
	UPROPERTY(EditDefaultsOnly, Category = "TrackeBot|Sounds")
		class USoundCue* SoundWarning;
	UPROPERTY(EditDefaultsOnly, Category = "TrackeBot|Sounds")
		class USoundCue* SoundRolling;

	UPROPERTY(EditDefaultsOnly, Category = "TrackeBot|Effects")
	class UParticleSystem* ExplosionEffect;

	class UMaterialInstanceDynamic* MatInst;

	void MoveToNextPoint();

	void SelfDestruct();
	void SelfDamage();

	bool bStartedSelfDestroy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TrackeBot")
		float Damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TrackeBot")
		float DamageRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TrackeBot")
		TSubclassOf<class UDamageType> BotDamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
		float Timer;

	UPROPERTY()
		bool bExploded;



	FTimerHandle BotTimerHandle;
	FTimerHandle RefreshNavPath_TimerHandle;

	void RefreshNav();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
