// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_CheckReachableDistance.generated.h"

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API UBTD_CheckReachableDistance : public UBTDecorator
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ActorToCheck;

	UPROPERTY(EditAnywhere, Category = "Blackboard")

	float DistanceToCheck;

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)const override;

};
