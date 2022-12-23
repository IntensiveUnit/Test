// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/PlayerHUD.h"
#include "TestPlayerHUD.generated.h"

/**
 * Custom HUD is used to create a key binding for opening the map menu
 */
UCLASS()
class TEST_API ATestPlayerHUD : public APlayerHUD
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent)
	void OpenMapMenu();
};
