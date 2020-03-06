// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ShootAtTarget.generated.h"

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API UBTTask_ShootAtTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
		FBlackboardKeySelector Target;

private:

	class AAIController* AIController;
	class UBlackboardComponent* BlackboardComponent;
};
