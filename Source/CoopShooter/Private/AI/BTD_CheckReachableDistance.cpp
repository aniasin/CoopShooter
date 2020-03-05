// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopShooter/Public/AI/BTD_CheckReachableDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

bool UBTD_CheckReachableDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComponent || !AIController) { return false; }

	UObject* Target = BlackboardComponent->GetValueAsObject(ActorToCheck.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(Target);

	if (!TargetActor) { return false; }

	UE_LOG(LogTemp, Warning, TEXT("Checking distance..."))
	FVector AiPosition = AIController->GetPawn()->GetActorLocation();
	FVector TargetPosition = TargetActor->GetActorLocation();

	float Distance = (AiPosition - TargetPosition).Size();

	if (Distance < DistanceToCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("TRUE..."))
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FALSE..."))
		return false;
	}

}
