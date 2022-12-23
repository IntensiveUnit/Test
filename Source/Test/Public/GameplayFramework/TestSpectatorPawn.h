// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "TestSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API ATestSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	/** Opening a new map and character respawn */
	void Respawn() const;

	/** Time after spawn spectator respawn will occur */
	UPROPERTY(EditDefaultsOnly, Category = "Respawning")
	float RespawnTime;

	/** What level to open before the character spawns */
	UPROPERTY(EditDefaultsOnly, Category = "Respawning")
	FName LevelToOpen;
	
	FTimerHandle RespawnTimerHandle;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Respawning")
	float GetRespawnTime() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Respawning")
	FName GetLevelToOpen() const;
};
