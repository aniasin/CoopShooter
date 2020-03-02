// Fill out your copyright notice in the Description page of Project Settings.


#include "SCHealthComponent.h"
#include "GameFramework/Actor.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USCHealthComponent::USCHealthComponent()
{
	MaxHealth = 100.0f;


}

// Called when the game starts
void USCHealthComponent::BeginPlay()
{
	SetIsReplicated(true);
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USCHealthComponent::HandleTakeAnyDamage);
	}
	CurrentHealth = MaxHealth;}

void USCHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = CurrentHealth - OldHealth;
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, nullptr, nullptr, nullptr);
}

void USCHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, 
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		if (Damage <= 0) { return; }

		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
		OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);

		UE_LOG(LogTemp, Warning, TEXT("Health %s of %s"), *FString::SanitizeFloat(CurrentHealth), *GetOwner()->GetName())
	}

}

void USCHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USCHealthComponent, CurrentHealth);
}