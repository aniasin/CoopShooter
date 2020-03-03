// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSPowerUpActor.generated.h"

UCLASS()
class COOPSHOOTER_API ACSPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSPowerUpActor();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USceneComponent* SceneComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UPointLightComponent* LightComp;


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
	void OnExpired(AActor* Actor);
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpTicked(AActor* Actor);

};
