// Fill out your copyright notice in the Description page of Project Settings.


#include "SCHealthComponent.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
USCHealthComponent::USCHealthComponent()
{
	MaxHealth = 100.0f;
}

// Called when the game starts
void USCHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USCHealthComponent::HandleTakeAnyDamage);
	}
	CurrentHealth = MaxHealth;
}

void USCHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, 
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0) { return; }

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Health changed : %s"), *FString::SanitizeFloat(CurrentHealth));

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}
