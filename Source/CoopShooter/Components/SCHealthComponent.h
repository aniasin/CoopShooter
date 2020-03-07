// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SCHealthComponent.generated.h"

// On Health Changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USCHealthComponent*, HealthComp, float, CurrentHealth, float, HealthDelta,
const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPSHOOTER_API USCHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USCHealthComponent();

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	bool bIsDead;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
		uint8 TeamID;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	float GetCurrentHealth();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadWrite, Category = "HealthComponent")
		float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float MaxHealth;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage,
		const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Event")
		FOnDeath OnDeathEvent;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthCOmponent")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);

	UFUNCTION()
		void OnRep_Health(float OldHealth);
};
