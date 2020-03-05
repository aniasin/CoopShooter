// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SearchTarget.generated.h"

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API UBTService_SearchTarget : public UBTService
{
	GENERATED_BODY()

		UBTService_SearchTarget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UPROPERTY(EditAnywhere, Category = "Blackboard")
		FBlackboardKeySelector CanSeePlayerKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		FBlackboardKeySelector LastKnownPositionKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
		FBlackboardKeySelector LastKnownDirectionKey;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
	bool bLastCanSeePlayer;
};
