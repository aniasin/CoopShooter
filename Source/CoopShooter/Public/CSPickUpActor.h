// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSPickUpActor.generated.h"

UCLASS()
class COOPSHOOTER_API ACSPickUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSPickUpActor();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Respawn();

UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereComp;
UPROPERTY(VisibleAnywhere, Category = "Components")
	class UDecalComponent* DecalComp;

UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
float RespawnTime;
UPROPERTY(EditAnywhere, Category = "PowerUp")
TSubclassOf<class ACSPowerUpActor> PowerUpClass;

FTimerHandle RespawnTimerHandle;

class ACSPowerUpActor* PowerUp;

public:	

	virtual void NotifyActorBeginOverlap(AActor* OtherActoroverride);

};
