// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSPowerUpActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UPointLightComponent;
class URotatingMovementComponent;

UCLASS()
class COOPSHOOTER_API ACSPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSPowerUpActor();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		USceneComponent* SceneComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UPointLightComponent* LightComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		URotatingMovementComponent* RotatingMovementComp;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float PowerUpInterval;
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
		float TotalNumberOfTicks;

	int32 TicksProcessed;

	FTimerHandle PowerUpHandle;

	UFUNCTION()
		void OnTickPowerUp();

public:	
	void ActivatePowerUp();


	class AActor* CurrentActor;

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnActivated(AActor* Actor);
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnExpired();
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpTicked();

};
