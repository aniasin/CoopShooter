// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDeco_CheckBool.generated.h"

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API UBTDeco_CheckBool : public UBTDecorator
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(VisibleAnywhere, Category = Blackboard)
		FBlackboardKeySelector BoolToCheck;

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)const override;

};
