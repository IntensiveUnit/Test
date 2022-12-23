// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "BaseItem.generated.h"

class AItemLogic;

USTRUCT()
struct FBaseItemData : public FTableRowBase
{
	GENERATED_BODY()

	//Item display name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	//Displayed item description
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	//Image of the item displayed in the ui
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Image;
	
	FBaseItemData(const FText InName = FText(), const FText InDescription = FText(), UTexture2D* InImage = nullptr,
	              UStaticMesh* InStaticMesh = nullptr, const bool InIsContainLogic = false,
	              const FText InLogicDescription = FText(), const TSubclassOf<AItemLogic> InConsumableItemLogic = nullptr):
		Name(InName),
		Description(InDescription),
		Image(InImage)
	{
	}

	FBaseItemData(const FBaseItemData* InItemInfo):
		Name(InItemInfo->Name),
		Description(InItemInfo->Description),
		Image(InItemInfo->Image)
	{
	}

	bool operator==(const FBaseItemData& BaseItemDataForComparison) const
	{
		return this->Name.EqualTo(BaseItemDataForComparison.Name) &&
			this->Description.EqualTo(BaseItemDataForComparison.Description) &&
			this->Image == BaseItemDataForComparison.Image;
	}

	bool IsEmpty() const
	{
		return this->Name.IsEmpty()
		&& this->Description.IsEmpty()
		&& this->Image;
	}
};

DECLARE_LOG_CATEGORY_EXTERN(LogBaseItem, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBaseItemWasPickedUp, class ABaseItem*, Item, APlayerCharacter*, WhoPickedUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBaseItemWasUsed, class ABaseItem*, Item, APlayerCharacter*, WhoUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBaseItemWasDropped, class ABaseItem*, Item, APlayerCharacter*, WhoDropped);

UCLASS(Abstract)
class TEST_API ABaseItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	/** Basic information about the item, used for ui */
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetBaseItemData)
	FBaseItemData BaseItemData;
	
	/** ObjectInteractionInterface */
	virtual bool InteractWithObject_Implementation(APlayerCharacter* InteractionInstigator) override;
	
	/** ObjectInteractionInterface */
	virtual bool SelectObject_Implementation(APlayerCharacter* ResponsibleForSelection) override;
	
	/** ObjectInteractionInterface */
	virtual bool UseObject_Implementation(APlayerCharacter* ResponsibleForUse) override;
	
	/** Spawn an item */
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static ABaseItem* SpawnBaseItem(const UObject* WorldContextObject, const TSubclassOf<ABaseItem> BaseItemSubclass, const FTransform& SpawnTransform);
	
	/** Get this inventory item's data */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FBaseItemData& GetBaseItemData();
	
	/** Called when an item in a level has been picked up by a player */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnBaseItemWasPickedUp OnItemWasPickedUp;

	/** Called when an item in a level has been used by a player */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnBaseItemWasUsed OnItemWasUsed;
	
	/** Called when an item has been dropped by a player */
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnBaseItemWasDropped OnItemWasDropped;
};
