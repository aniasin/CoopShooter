// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopShooter/Public/AI/BTTask_ShootAtTarget.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "CoopShooter/Public/AI/CSAIController.h"

EBTNodeResult::Type UBTTask_ShootAtTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get Blackboard
	BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComponent || !AIController) { return EBTNodeResult::Failed; }

	ACSCharacter* CurrentPawn = Cast<ACSCharacter>(AIController->GetPawn());
	if (!CurrentPawn) {	return EBTNodeResult::Failed; }

	CurrentPawn->AIFire();
	   
	return EBTNodeResult::Succeeded;

}
