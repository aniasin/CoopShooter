// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopShooter/Public/AI/CSAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionTypes.h"
#include "NavigationSystem.h"
#include "BrainComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoopShooter/Public/CSCharacter.h"
#include "DrawDebugHelpers.h"



ACSAIController::ACSAIController()
{
	// Creating Perception component
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(FName("HearingConfig"));

	// Configure the sight and hearing sense
	SightConfig->SightRadius = SightRange;
	SightConfig->LoseSightRadius = LoseSightRange;
	HearingConfig->HearingRange = HearingRange;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	// Assign the sight and hearing sense to the perception component
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACSAIController::OnTargetPerceptionUpdate);
}

void ACSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AICharacter = Cast<ACSCharacter>(InPawn);

	if (AICharacter && BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		BlackboardComponent = GetBlackboardComponent();
	}
	if (!BehaviorTree) { UE_LOG(LogTemp, Warning, TEXT("No behavior tree set in %s !"), *AICharacter->GetName()) }

	// Assign this controller to team 10
	SetGenericTeamId(FGenericTeamId(10));
}

void ACSAIController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	UE_LOG(LogTemp, Warning, TEXT("Perception Update !"))
	if (!AICharacter) { return; }
	//Retrieving perceived actors
	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), SeenActors);

	// Numbers of seen actors and if they enter or exit view
	int32 NumberOfActorsSeen = SeenActors.Num();

	LastKnownPlayerPosition = Stimulus.StimulusLocation;
	bCanSeePlayer = Stimulus.WasSuccessfullySensed();

	ACSCharacter* Player = Cast<ACSCharacter>(Actor);

	// Gain Sight
	if (Player && bCanSeePlayer)
	{
		CurrentTargetActor = Player;
		SetFocus(CurrentTargetActor);
		UE_LOG(LogTemp, Warning, TEXT("%s GAIN SIGHT %s !"), *GetPawn()->GetName(), *Player->GetName())
	}
	// Sight is lost
	if (Player && !bCanSeePlayer)
	{
		ClearFocus(EAIFocusPriority::Default);
		// set last Player direction
		LastKnownPlayerDirection = Player->GetVelocity().GetUnsafeNormal() * 1000;
		DrawDebugSphere(GetWorld(), LastKnownPlayerDirection, 100, 12, FColor::Red, false, 5, 1);

		UE_LOG(LogTemp, Warning, TEXT("%s LOST SIGHT WITH %s !"), *GetPawn()->GetName(), *Player->GetName())
	}
	BlackboardComponent->SetValueAsObject("TargetActor", CurrentTargetActor);
}

ETeamAttitude::Type ACSAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			return Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
		}
	}
	return ETeamAttitude::Neutral;
}


