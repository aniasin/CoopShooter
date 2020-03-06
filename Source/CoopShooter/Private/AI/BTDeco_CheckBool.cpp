// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopShooter/Public/AI/BTDeco_CheckBool.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoopShooter/Public/AI/CSAIController.h"

bool UBTDeco_CheckBool::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComponent || !AIController) { return false; }

	return BlackboardComponent->GetValueAsBool(Bool.SelectedKeyName);
}
