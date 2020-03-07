// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "CSAIController.generated.h"

class ACSCharacter;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API ACSAIController : public AAIController
{
	GENERATED_BODY()

		ACSAIController();

	ACSCharacter* AICharacter;

	UAIPerceptionComponent* PerceptionComponent;
	UAISenseConfig_Sight* SightConfig;
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float SightRange = 5000;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float LoseSightRange = 6000;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float HearingRange = 2000;
	UPROPERTY(EditAnywhere, Category = "AI")
		UBehaviorTree* BehaviorTree;
	UBlackboardComponent* BlackboardComponent;

	FTimerHandle FDiscardTarget_TimerHandle;
	// How long Bot will search if no sight
	UPROPERTY(EditAnywhere, Category = "AI")
	float TimeToSearch = 10;

	UFUNCTION()
		void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

public:
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other)const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		bool bCanSeePlayer = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		FVector LastKnownPlayerPosition;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	FVector LastKnownPlayerDirection;

	AActor* CurrentTargetActor;
	void DiscardTarget();

protected:
	void OnPossess(APawn* InPawn);

	UFUNCTION()
	void GameIsOver();

};
