// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UHealthComponent;
class ABaseItem;
class UCameraComponent;
class UInventoryComponent;

UCLASS()
class TEST_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	//** Used to initialize variables */
	APlayerCharacter();
	
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly)
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	UCameraComponent* CharacterCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	//The actor with which the character interacts
	UPROPERTY(BlueprintGetter = GetInteractionActor)
	AActor* InteractionActor;
	
	UPROPERTY()
	ABaseItem* CurrentSelectedItem;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Camera")
	UCameraComponent* GetCameraComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	UHealthComponent* GetHealthComponent() const;

	/** Used to determine the actor the character is focused on */
	void FocusActor();

	/** Who is the character interacting with */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	AActor* GetInteractionActor() const;

	/** Radius in which the character can interact with objects */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRadius;
	
	/** Show sphere to define interaction area */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool DebugInteraction;
	
	/**
	* Get the coordinates of the place in front of the character
	* where it is worth spawning the dropped item
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	FVector GetItemDropPosition();
	
	UFUNCTION()
	void OnDeath(UHealthComponent* InHealthComponent);
	
	//Turn left right speed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseTurnRate = 45.0f;
	
	//Turn up down speed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseLookUpRate = 45.0f;
	
	//Look up and down
	void LookUp(float Value);
	
	//Turn left and right
	void Turn(float Value);

	//Forward and backward movement
	UFUNCTION()
	void MoveForward(float Value);

	//Right and left movement
	void MoveRight(float Value);
	
	//Interaction with the object
	UFUNCTION()
	void Interact();

	UFUNCTION()
	void Use();
};
