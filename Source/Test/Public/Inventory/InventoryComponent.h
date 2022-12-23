// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Health/HealthComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBaseItemAdded, class ABaseItem*, AddedBaseItem, class UInventoryComponent*, InventoryComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBaseItemUsed, class ABaseItem*, UsedBaseItem, class UInventoryComponent*, InventoryComponent, class APlayerCharacter*, WhoUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBaseItemRemoved, class ABaseItem*, RemovedBaseItem, class UInventoryComponent*, InventoryComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBaseItemDropped, class ABaseItem*, DroppedBaseItem, class UInventoryComponent*, InventoryComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSelectedBaseItemChanged, class ABaseItem*, SelectedBaseItem, class UInventoryComponent*, InventoryComponent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<ABaseItem*> Items;
	
	UPROPERTY(EditDefaultsOnly, BlueprintGetter=GetInventorySize)
	int32 InventorySize;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter=GetCurrentSelectedSlot)
	int32 CurrentSelectedSlot;
	
	UPROPERTY(EditDefaultsOnly)
	ABaseItem* CurrentSelectedItem;

protected:
	UInventoryComponent();

	virtual void BeginPlay() override;
	
	void OnLevelChanged();
	
public:
	UFUNCTION()
	void ClearInventory(UHealthComponent* InHealthComponent);
	
	UFUNCTION(BlueprintCallable)
	bool AddItem(ABaseItem* BaseItem);
	
	UFUNCTION(BlueprintCallable)
	bool DropItem(ABaseItem* BaseItem);

	UFUNCTION(BlueprintCallable)
	bool UseBaseItem(ABaseItem* BaseItem);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<ABaseItem*> GetItems() { return Items; };
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetInventorySize() const { return InventorySize; };

	UFUNCTION(BlueprintCallable)
	void SetCurrentSelectedSlot(int32 NewCurrentSelectedSlot);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentSelectedSlot() const { return CurrentSelectedSlot; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ABaseItem* GetCurrentSelectedItem() const { return CurrentSelectedItem; };

	UFUNCTION(BlueprintCallable)
	void SetCurrentSelectedItem(ABaseItem* InSelectedBaseItem, UInventoryComponent* InventoryComponent);

	UFUNCTION()
	void SetNextSlot();

	UFUNCTION()
	void SetPreviousSlot();

	UFUNCTION()
	static void ShowItem(ABaseItem* BaseItemToShow);

	UFUNCTION()
	static void HideItem(AActor* ActorToAttach, ABaseItem* BaseItemToHide);
	
	//Called when an item in a level has been used by a player
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FBaseItemAdded OnBaseItemAdded;

	//Called when an item in a level has been used by a player
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FBaseItemUsed OnBaseItemUsed;
	
	//Called when an item has been dropped by a player
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FBaseItemRemoved OnBaseItemRemoved;

	//Called when an item has been dropped by a player
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FBaseItemDropped OnBaseItemDropped;

	//Called when an item has been dropped by a player
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FSelectedBaseItemChanged OnSelectedBaseItemChanged;
};


