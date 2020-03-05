// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "CSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API ACSPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

		ACSPlayerController();

	FGenericTeamId TeamID;

public:
	
protected:
	virtual void BeginPlay()override;
};
