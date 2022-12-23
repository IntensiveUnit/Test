// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Inventory/BaseItem.h"
#include "TestGameInstance.generated.h"

/**
 * Custom game instance, used to transfer items data from one level to another
 */
UCLASS()
class TEST_API UTestGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ABaseItem>> TransferredItems;

	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<ABaseItem>> GetTransferredItems();

	UFUNCTION(BlueprintCallable)
	void SetTransferredItems(TArray<TSubclassOf<ABaseItem>> NewTransferredItems);
	
	
};
