// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/BaseItem.h"

#include "GameplayFramework/PlayerCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogBaseItem);

bool ABaseItem::InteractWithObject_Implementation(APlayerCharacter* InteractionInstigator)
{
	UInventoryComponent* InventoryComponent = InteractionInstigator->GetInventoryComponent();

	const bool Result = InventoryComponent->AddItem(this);
	
	return Result;
}

bool ABaseItem::SelectObject_Implementation(APlayerCharacter* ResponsibleForSelection)
{
	if (!ResponsibleForSelection) { UE_LOG(LogBaseItem, Warning, TEXT("Actor responsible for selection is not valid")) return false; };
	
	const FAttachmentTransformRules AttachmentRule = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	AttachToActor(ResponsibleForSelection, AttachmentRule, FName("hand_rSocket"));
	return IInteractionInterface::SelectObject_Implementation(ResponsibleForSelection);
}


bool ABaseItem::UseObject_Implementation(APlayerCharacter* ResponsibleForUse)
{
	if (!ResponsibleForUse) { UE_LOG(LogBaseItem, Warning, TEXT("Actor responsible for use is not valid")) return false; };
	
	return false;
}

ABaseItem* ABaseItem::SpawnBaseItem(const UObject* WorldContextObject, const TSubclassOf<ABaseItem> BaseItemSubclass, const FTransform& SpawnTransform)
{
	
	if (!WorldContextObject->GetWorld()){ UE_LOG(LogBaseItem, Error, TEXT("WorldContextObject is not valid in spawning base item"));
		return nullptr;}

	if (!BaseItemSubclass.Get()) { UE_LOG(LogBaseItem, Error, TEXT("Subclass of base item is not valid in spawning base item"));
		return nullptr; }
	
	ABaseItem* BaseItem = WorldContextObject->GetWorld()->SpawnActorDeferred<ABaseItem>(BaseItemSubclass.Get(), SpawnTransform);

	if (!BaseItem) { UE_LOG(LogBaseItem, Error, TEXT("Base item was not spawned in spawning base item"));
		return nullptr; }
	
	UGameplayStatics::FinishSpawningActor(BaseItem, SpawnTransform); UE_LOG(LogBaseItem, Error, TEXT("Spawning base item is done"));
	return BaseItem;
}

FBaseItemData& ABaseItem::GetBaseItemData()
{
	return BaseItemData;
}



