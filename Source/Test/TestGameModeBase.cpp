// Copyright Epic Games, Inc. All Rights Reserved.


#include "TestGameModeBase.h"
#include "GameplayFramework/PlayerCharacter.h"
#include "GameplayFramework/PlayerCharacterController.h"
#include "UI/PlayerHUD.h"

ATestGameModeBase::ATestGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	HUDClass = APlayerHUD::StaticClass();
	PlayerControllerClass = APlayerCharacterController::StaticClass();
}
