// Fill out your copyright notice in the Description page of Project Settings.



#include "GameplayFramework/PlayerCharacter.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayFramework/PlayerCharacterController.h"
#include "GameplayFramework/TestPlayerHUD.h"
#include "Health/HealthComponent.h"
#include "Interfaces/InteractionInterface.h"
#include "Inventory/BaseItem.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

APlayerCharacter::APlayerCharacter():
InteractionRadius(500.f),
DebugInteraction(false)
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->OnDeath.AddDynamic(this, &APlayerCharacter::OnDeath);
	
	CharacterCamera = CreateDefaultSubobject<UCameraComponent>(FName("CharacterCamera"));
	CharacterCamera->SetupAttachment(GetRootComponent());
	CharacterCamera->bUsePawnControlRotation = true;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FocusActor();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn Right / Left", this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &APlayerCharacter::Interact);
	PlayerInputComponent->BindAction("UseItem", EInputEvent::IE_Pressed, this, &APlayerCharacter::Use);
	PlayerInputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, InventoryComponent, &UInventoryComponent::SetNextSlot);
	PlayerInputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, InventoryComponent, &UInventoryComponent::SetPreviousSlot);

	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController){ return; }

	ATestPlayerHUD* TestPlayerHUD = Cast<ATestPlayerHUD>(PlayerController->GetHUD());
	if (!TestPlayerHUD){ return; }
	
	PlayerInputComponent->BindAction("OpenMapMenu", EInputEvent::IE_Pressed, TestPlayerHUD, &ATestPlayerHUD::OpenMapMenu);
}

UInventoryComponent* APlayerCharacter::GetInventoryComponent()
{
	return InventoryComponent;
}

UCameraComponent* APlayerCharacter::GetCameraComponent()
{
	return CharacterCamera;
}

UHealthComponent* APlayerCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

void APlayerCharacter::FocusActor()
{
	const UWorld* World = GetWorld();

	if (!World){ InteractionActor = nullptr; return;}
	
	FHitResult HitResult;
	const float InteractionSphereRadius = 5.f;

	const FVector StartTraceLocation = CharacterCamera->GetComponentLocation();
	const FVector EndTraceLocation = CharacterCamera->GetComponentLocation() + CharacterCamera->GetForwardVector() * InteractionRadius;
	
	if(!World->SweepSingleByChannel(
		HitResult,
		StartTraceLocation,
		EndTraceLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeSphere(InteractionSphereRadius)))
	{ InteractionActor = nullptr; return; }

	if (DebugInteraction)
	{
		const float DebugSphereSegments = 16.f;
		DrawDebugSphere(GetWorld(), HitResult.Location, InteractionSphereRadius, DebugSphereSegments, InteractionActor ? FColor::Green : FColor::Red);
	}
	
	if (!HitResult.GetActor()) { InteractionActor = nullptr; return; }
	
	if (!HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{ InteractionActor = nullptr; return; }
	
	InteractionActor = HitResult.GetActor();
}

AActor* APlayerCharacter::GetInteractionActor() const
{
	return InteractionActor;
}

FVector APlayerCharacter::GetItemDropPosition()
{
	const float Offset = 200.f;
	return GetActorLocation() + GetActorForwardVector() * Offset;
}

void APlayerCharacter::OnDeath(UHealthComponent* InHealthComponent)
{
	GetCharacterMovement()->DisableMovement();
	
	GetMesh()->UnHideBoneByName(FName("neck_01"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
	
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}

void APlayerCharacter::LookUp(float Value)
{
	const float PitchScalar = Value * BaseLookUpRate * GetWorld()->DeltaTimeSeconds;
	
	AddControllerPitchInput(PitchScalar);
}

void APlayerCharacter::Turn(float Value)
{
	const float YawScalar = Value * BaseTurnRate * GetWorld()->DeltaTimeSeconds;
	
	AddControllerYawInput(YawScalar);
}

void APlayerCharacter::MoveForward(float Value)
{
	const FVector MovementVector = UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0));
	
	AddMovementInput(MovementVector, Value);
}

void APlayerCharacter::MoveRight(float Value)
{
	const FVector MovementVector = UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0));
	
	AddMovementInput(MovementVector, Value);
}

void APlayerCharacter::Interact()
{
	if (!GetInteractionActor()) { return; }
	
	IInteractionInterface::Execute_InteractWithObject(GetInteractionActor(), this);
}

void APlayerCharacter::Use()
{
	ABaseItem* ItemForUse = InventoryComponent->GetCurrentSelectedItem();
	
	if (ItemForUse)
	{
		IInteractionInterface::Execute_UseObject(ItemForUse, this);
	}
}



