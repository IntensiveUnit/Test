// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFramework/TestGameInstance.h"

TArray<TSubclassOf<ABaseItem>> UTestGameInstance::GetTransferredItems()
{
	return TransferredItems;
}

void UTestGameInstance::SetTransferredItems(TArray<TSubclassOf<ABaseItem>> NewTransferredItems)
{
	TransferredItems = NewTransferredItems;
}
