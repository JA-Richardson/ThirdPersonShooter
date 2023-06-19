// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonComponents/CombatComponent.h"
#include "Weapons/WeaponBase.h"
#include "Character/ThirdPersonCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCombatComponent::SetAiming(bool bAiming)
{
	bIsAiming = bAiming;
	ServerSetAiming(bAiming);
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	FHitResult HitResult;
	CrosshairTrace(HitResult);
	ServerFire(HitResult.ImpactPoint);
	
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& HitLocation)
{
	HitTarget = HitLocation;
	MulticastFire(HitLocation);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& HitLocation)
{
	OwnerCharacter->PlayFireMontage(bIsAiming);

	if(EquippedWeapon == nullptr) return;
	EquippedWeapon->Fire(HitLocation);
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Firing"));
	}
}

void UCombatComponent::CrosshairTrace(FHitResult& OutHitResult)
{
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);
	if(bScreenToWorld)
	{
		FVector TraceStart = CrosshairWorldPosition;
		FVector TraceEnd = CrosshairWorldPosition + CrosshairWorldDirection * TRACE_LENGTH;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerCharacter);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);
		if (!OutHitResult.bBlockingHit)
		{
			OutHitResult.ImpactPoint = TraceEnd;
		}
		
	}
	
}

void UCombatComponent::ServerSetAiming_Implementation(bool bAiming)
{
	bIsAiming = bAiming;
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::EquipWeapon(class AWeaponBase* WeaponToEquip)
{
	if(OwnerCharacter == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = OwnerCharacter->GetMesh()->GetSocketByName(FName("hand_r_socket"));
	if(HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, OwnerCharacter->GetMesh());
	}
	EquippedWeapon->SetOwner(OwnerCharacter);
	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

