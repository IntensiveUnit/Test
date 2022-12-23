// Fill out your copyright notice in the Description page of Project Settings.


#include "Health/HealthComponent.h"

DEFINE_LOG_CATEGORY(LogHealthComponent)

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetHealth(MaxHealth);

	AActor* ComponentOwner = GetOwner();
	if (ComponentOwner)
	{
		
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeAnyDamage);
		ComponentOwner->OnTakePointDamage.AddDynamic(this, &UHealthComponent::OnTakePointDamage);
		ComponentOwner->OnTakeRadialDamage.AddDynamic(this, &UHealthComponent::OnTakeRadialDamage);
	}
}

void UHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
									   AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogHealthComponent, Display, TEXT("On take any damage: %f"), Damage);
	ApplyDamage(Damage);
}

void UHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
	class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType,
	AActor* DamageCauser)
{
	UE_LOG(LogHealthComponent, Display, TEXT("On point damage: %f"), Damage);
	ApplyDamage(Damage);
}

void UHealthComponent::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin,
	FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogHealthComponent, Display, TEXT("On radial damage: %f"), Damage);
	ApplyDamage(Damage);
}

void UHealthComponent::ApplyDamage(const float Damage)
{
	if (Damage <= 0.0f || IsDead() || !GetWorld()) return;

	SetHealth(Health - Damage);
	
	if (IsDead()) { OnDeath.Broadcast(this); }
}

void UHealthComponent::SetHealth(const float NewHealth)
{
	const auto NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	const auto HealthDelta = NextHealth - Health;
 
	Health = NextHealth;
	OnHealthChanged.Broadcast(Health, HealthDelta);
}

float UHealthComponent::GetMaxHealth() const { return MaxHealth; }

float UHealthComponent::GetCurrentHealth() const { return Health; }






