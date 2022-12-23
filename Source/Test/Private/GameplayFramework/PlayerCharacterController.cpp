// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFramework/PlayerCharacterController.h"

void APlayerCharacterController::BeginSpectatingState()
{
	Super::BeginSpectatingState();

	if (OnSpectatingPawnPossess.IsBound()) { OnSpectatingPawnPossess.Broadcast(this); }
}


