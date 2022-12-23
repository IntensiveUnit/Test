// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayFramework/TestSpectatorPawn.h"
#include "Kismet/GameplayStatics.h"

void ATestSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ATestSpectatorPawn::Respawn, RespawnTime, false);
}

void ATestSpectatorPawn::Respawn() const
{
	UGameplayStatics::OpenLevel(this, GetLevelToOpen());
}

float ATestSpectatorPawn::GetRespawnTime() const
{
	return RespawnTime;
}

FName ATestSpectatorPawn::GetLevelToOpen() const
{
	return LevelToOpen;
}

