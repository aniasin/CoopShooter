// Fill out your copyright notice in the Description page of Project Settings.


#include "SCHealthComponent.h"
#include "GameFramework/Actor.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "CoopShooter/Public/SCGameMode.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USCHealthComponent::USCHealthComponent()
{
	MaxHealth = 100.0f;

	TeamID = 255;
}

// Called when the game starts
void USCHealthComponent::BeginPlay()
{
	SetIsReplicated(true);
	Super::BeginPlay();

	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USCHealthComponent::HandleTakeAnyDamage);
		}
	}
	CurrentHealth = MaxHealth;}

bool USCHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (!ActorA || !ActorB)
	{
		return false;
	}
	USCHealthComponent* TestHealthCompA = Cast<USCHealthComponent>(ActorA->GetComponentByClass(USCHealthComponent::StaticClass()));
	USCHealthComponent* TestHealthCompB = Cast<USCHealthComponent>(ActorB->GetComponentByClass(USCHealthComponent::StaticClass()));
	if (!TestHealthCompA || !TestHealthCompB)
	{
		return false;
	}
	return TestHealthCompA->TeamID == TestHealthCompB->TeamID;
}

void USCHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = CurrentHealth - OldHealth;
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, nullptr, nullptr, nullptr);
}

void USCHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, 
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0 || CurrentHealth <= 0) { return; }

	if (IsFriendly(DamageCauser, DamagedActor) && DamageCauser != DamagedActor)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);

	OnRep_Health(CurrentHealth);

	bIsDead = CurrentHealth <= 0;

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(CurrentHealth), *GetOwner()->GetName())

	if (CurrentHealth <= 0 || bIsDead)
	{
		OnDeathEvent.Broadcast();

		ASCGameMode* GM = Cast<ASCGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->ActorKilled(DamageCauser, DamagedActor, InstigatedBy);
		}
	}

}

void USCHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USCHealthComponent, CurrentHealth);
}