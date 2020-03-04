// Fill out your copyright notice in the Description page of Project Settings.


#include "CSPlayerState.h"

ACSPlayerState::ACSPlayerState()
{
	
}

void ACSPlayerState::AddScore(float ScoreDelta)
{
	Score += ScoreDelta;
}