// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopShooter/Public/AI/BTService_SearchTarget.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "CoopShooter/Public/AI/CSAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SearchTarget::UBTService_SearchTarget(const FObjectInitializer& ObjectInitializer)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_SearchTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) { return; }

}

void UBTService_SearchTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) { return; }

	// Get AIController
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) { return; }
	ACSAIController* CurrentController = Cast<ACSAIController>(AIController);

	// Update the blackboard with the value of bCanSeePlayer from Controller
	BlackboardComp->SetValueAsBool(CanSeePlayerKey.SelectedKeyName, CurrentController->bCanSeePlayer);

	// if the lastCanSee is different than CanSee then update LastKnownPosition
	if (CurrentController->bCanSeePlayer != bLastCanSeePlayer) {
		BlackboardComp->SetValueAsVector(LastKnownPositionKey.SelectedKeyName, CurrentController->LastKnownPlayerPosition);
		BlackboardComp->SetValueAsVector(LastKnownDirectionKey.SelectedKeyName, CurrentController->LastKnownPlayerDirection);
	}
	bLastCanSeePlayer = CurrentController->bCanSeePlayer;


	//Call to the parent TickNode
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

