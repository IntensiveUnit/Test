// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnPossess, class APlayerCharacterController*, PlayerCharacterController);

/**
 * Custom controller, used to keep track of when pawn changes
 */
UCLASS()
class TEST_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()
	
	virtual void BeginSpectatingState() override;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnPawnPossess OnSpectatingPawnPossess;
	
};
