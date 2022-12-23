// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "GameplayFramework/PlayerCharacter.h"
#include "GameplayFramework/TestGameInstance.h"
#include "Health/HealthComponent.h"
#include "Inventory/BaseItem.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogInventory)


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent():
InventorySize(5),
CurrentSelectedSlot(0)
{
	this->OnSelectedBaseItemChanged.AddDynamic(this, &UInventoryComponent::SetCurrentSelectedItem);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner()) { UE_LOG(LogInventory, Error, TEXT("No owner in InventoryComponent")) return;  }
	
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance) { UE_LOG(LogInventory, Error, TEXT("No GameInstance in InventoryComponent")) return; }
	
	UTestGameInstance* TestGameInstance = Cast<UTestGameInstance>(GameInstance);
	if (!TestGameInstance) { UE_LOG(LogInventory, Error, TEXT("Cant cast GameInstance to TestGameInstance in InventoryComponent")) return; }
	
	if (TestGameInstance->GetTransferredItems().Num() > 0)
	{
		for (const auto& BaseItemSubclass : TestGameInstance->GetTransferredItems())
		{
			if (!BaseItemSubclass.Get())
			{
				UE_LOG(LogInventory, Error, TEXT("Empty base item subclass in GetTransferredItems"));
				continue;
			}
		
			const FTransform& OwnerTransform = Cast<AActor>(GetOwner())->GetActorTransform();
		
			ABaseItem* SpawnedBaseItem = ABaseItem::SpawnBaseItem(GetOwner(), BaseItemSubclass, OwnerTransform);
		
			AddItem(SpawnedBaseItem);
		}
	}
	
	GetWorld()->OnBeginTearingDown().AddUObject(this, &UInventoryComponent::OnLevelChanged);

	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!PlayerCharacter){ UE_LOG(LogInventory, Warning, TEXT("Owner of inventory component is not player character")) return; }

	UHealthComponent* HealthComponent = PlayerCharacter->GetHealthComponent();
	if (!HealthComponent){ UE_LOG(LogInventory, Warning, TEXT("No health component in player character")) return; }
	
	HealthComponent->OnDeath.AddDynamic(this, &UInventoryComponent::ClearInventory);
}

void UInventoryComponent::OnLevelChanged()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance) { UE_LOG(LogInventory, Error, TEXT("No GameInstance in InventoryComponent")) return; }
	
	UTestGameInstance* TestGameInstance = Cast<UTestGameInstance>(GameInstance);
	if (!TestGameInstance) { UE_LOG(LogInventory, Error, TEXT("Cant cast GameInstance to TestGameInstance in InventoryComponent")) return; }

	TArray<TSubclassOf<ABaseItem>> ItemsToTransfer;
	
	for (const auto BaseItem : GetItems())
	{
		ItemsToTransfer.Add(BaseItem->GetClass());
	}
	
	TestGameInstance->SetTransferredItems(ItemsToTransfer);
}

void UInventoryComponent::ClearInventory(UHealthComponent* InHealthComponent)
{
	Items.Empty();
	SetCurrentSelectedSlot(0);
	if (OnBaseItemDropped.IsBound()) { OnBaseItemDropped.Broadcast(nullptr, this); }
	if (OnSelectedBaseItemChanged.IsBound()) { OnSelectedBaseItemChanged.Broadcast(nullptr, this); }
}

bool UInventoryComponent::AddItem(ABaseItem* BaseItem)
{
	if (!BaseItem) { UE_LOG(LogInventory, Error, TEXT("Can't add item, ref is null")) return false; }
	
	const FBaseItemData BaseItemData = BaseItem->GetBaseItemData();
	if (BaseItemData.IsEmpty()) { UE_LOG(LogInventory, Error, TEXT("Can't add item, data is empty")) return false; }

	SetCurrentSelectedSlot(Items.AddUnique(BaseItem));
	SetCurrentSelectedItem(BaseItem, this);
	BaseItem->SetOwner(GetOwner());
	
	if (OnBaseItemAdded.IsBound()) { OnBaseItemAdded.Broadcast(BaseItem, this); }

	if (OnSelectedBaseItemChanged.IsBound()) { OnSelectedBaseItemChanged.Broadcast(BaseItem, this); }
	
	return true;
}

bool UInventoryComponent::DropItem(ABaseItem* BaseItem)
{
	if (!BaseItem) { UE_LOG(LogInventory, Error, TEXT("Can't add item, ref is null")) return false; }
	
	if (!Items.Contains(BaseItem)) { UE_LOG(LogInventory, Error, TEXT("Item not in inventory")) return false; }

	Items.Remove(BaseItem);
	BaseItem->SetOwner(nullptr);
	CurrentSelectedItem = nullptr;
	ShowItem(BaseItem);
	SetCurrentSelectedItem(GetItems().IsValidIndex(0) ? GetItems()[0] : nullptr, this);
	
	if (OnBaseItemDropped.IsBound()) { OnBaseItemDropped.Broadcast(BaseItem, this); }
	
	return true;
}

bool UInventoryComponent::UseBaseItem(ABaseItem* BaseItem)
{
	if (!BaseItem) { UE_LOG(LogInventory, Error, TEXT("Can't use item, ref is null")) return false; }
	
	if (!Items.Contains(BaseItem)) { UE_LOG(LogInventory, Error, TEXT("Item not in inventory")) return false; }

	if (!GetOwner()) { UE_LOG(LogInventory, Error, TEXT("No owner")) return false; }

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	if (!PlayerCharacter) { UE_LOG(LogInventory, Error, TEXT("Owner is not player character")) return false; }

	if (IInteractionInterface::Execute_UseObject(BaseItem, PlayerCharacter))
	{
		if (OnBaseItemUsed.IsBound()) { OnBaseItemUsed.Broadcast(BaseItem, this, PlayerCharacter); }
	}
	
	return true;
}

void UInventoryComponent::SetCurrentSelectedSlot(const int32 NewCurrentSelectedSlot)
{
	CurrentSelectedSlot = NewCurrentSelectedSlot;

	if (OnSelectedBaseItemChanged.IsBound())
	{ OnSelectedBaseItemChanged.Broadcast(GetItems().IsValidIndex(GetCurrentSelectedSlot()) ? GetItems()[GetCurrentSelectedSlot()] : nullptr, this); }
}

void UInventoryComponent::SetCurrentSelectedItem(ABaseItem* InSelectedBaseItem, UInventoryComponent* InventoryComponent)
{
	if (GetCurrentSelectedItem())
	{
		HideItem(GetOwner(), GetCurrentSelectedItem());
	}
	
	if (InSelectedBaseItem)
	{
		ShowItem(InSelectedBaseItem);
		InSelectedBaseItem->SetActorEnableCollision(false);
		const FAttachmentTransformRules AttachmentRule = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		const FName SocketToAttachName = FName("hand_rSocket");
		const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
		InSelectedBaseItem->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRule, SocketToAttachName);
	}

	CurrentSelectedItem = InSelectedBaseItem;
}

void UInventoryComponent::SetNextSlot()
{
	if (GetCurrentSelectedSlot() + 1 > InventorySize - 1) { SetCurrentSelectedSlot(0); }
	else { SetCurrentSelectedSlot(GetCurrentSelectedSlot() + 1); }
}

void UInventoryComponent::SetPreviousSlot()
{
	if (GetCurrentSelectedSlot() - 1 < 0) { SetCurrentSelectedSlot(InventorySize - 1); }
	else { SetCurrentSelectedSlot(FMath::Clamp(CurrentSelectedSlot - 1, 0, InventorySize - 1)); }
}

void UInventoryComponent::ShowItem(ABaseItem* BaseItemToShow)
{
	BaseItemToShow->SetActorEnableCollision(true);
	UE_LOG(LogInventory, Log, TEXT("Item to Show: collision enabled"));
	
	BaseItemToShow->SetActorHiddenInGame(false);
	UE_LOG(LogInventory, Log, TEXT("Item to Show: visible enabled"));
	
	BaseItemToShow->DetachFromActor(FDetachmentTransformRules(
		EDetachmentRule::KeepWorld,
		EDetachmentRule::KeepWorld,
		EDetachmentRule::KeepWorld,
		true));
	UE_LOG(LogInventory, Log, TEXT("Item to Show: was detached"));
}

void UInventoryComponent::HideItem(AActor* ActorToAttach, ABaseItem* BaseItemToHide)
{
	BaseItemToHide->SetActorEnableCollision(false);
	UE_LOG(LogInventory, Log, TEXT("Item to hide: collision disabled"));
	
	BaseItemToHide->SetActorHiddenInGame(true);
	UE_LOG(LogInventory, Log, TEXT("Item to hide: visible disabled"));
	
	const FAttachmentTransformRules AttachmentRule = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	BaseItemToHide->AttachToActor(ActorToAttach, AttachmentRule);
	UE_LOG(LogInventory, Log, TEXT("Item to hide: was attached"));
}






