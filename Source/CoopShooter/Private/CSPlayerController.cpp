// Fill out your copyright notice in the Description page of Project Settings.


#include "CSPlayerController.h"
#include "GenericTeamAgentInterface.h"

ACSPlayerController::ACSPlayerController()
{
	// Assign this controller to team 1
	SetGenericTeamId(FGenericTeamId(1));
}

void ACSPlayerController::BeginPlay()
{

}
