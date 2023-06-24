// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonComponents/CombatComponent.h"
#include "Weapons/WeaponBase.h"
#include "Character/ThirdPersonCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/ThirdPersonHUD.h"
#include "PlayerController/ThirdPersonPlayerController.h"


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

	if(OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if(OwnerCharacter->GetFollowCamera())
		{
			DefaultFOV = OwnerCharacter->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
	
}
// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(OwnerCharacter && OwnerCharacter->IsLocallyControlled())
	{
		SetHudCrosshair(DeltaTime);
		InterpFOV(DeltaTime);
	}
	FHitResult HitResult;
	CrosshairTrace(HitResult);
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
	if(EquippedWeapon)
	{
		CrosshairShootFactor = 1.f;
	}
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

		if(OwnerCharacter)
		{
			float DistanceToCharacter = (OwnerCharacter->GetActorLocation() - TraceStart).Size();
			TraceStart += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}
		
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
		if(OutHitResult.GetActor() && OutHitResult.GetActor()->Implements<UCrosshairInteractInterface>())
		{
			HUDPackage.CrosshairColour = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColour = FLinearColor::White;
		}
	}
}

void UCombatComponent::SetHudCrosshair(float DeltaTime)
{
	if(OwnerCharacter == nullptr || OwnerCharacter->Controller == nullptr) return;

	OwnerController = OwnerController == nullptr ? Cast<AThirdPersonPlayerController>(OwnerCharacter->Controller) : OwnerController;
	if(OwnerController)
	{
		OwnerHUD = OwnerHUD == nullptr ? Cast<AThirdPersonHUD>(OwnerController->GetHUD()) : OwnerHUD;
		if(OwnerHUD)
		{
			
			if(EquippedWeapon)
			{
				HUDPackage.CrosshairCentre = EquippedWeapon->CrosshairCentre;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairLeft;
                HUDPackage.CrosshairRight = EquippedWeapon->CrosshairRight;
                HUDPackage.CrosshairTop = EquippedWeapon->CrosshairTop;
                HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairBottom;
			}
			else
			{
				HUDPackage.CrosshairCentre = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
			}
			//Calculate crosshair spread
			// [0, 600] -> [0, 1]
			FVector2D WalkSpeedRange(0.f, OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = OwnerCharacter->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if(OwnerCharacter->GetCharacterMovement()->IsFalling())
			{
				CrosshairAirFactor = FMath::FInterpTo(CrosshairAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairAirFactor = FMath::FInterpTo(CrosshairAirFactor, 0.f, DeltaTime, 30.f);
			}

			if(bIsAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.5f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootFactor = FMath::FInterpTo(CrosshairShootFactor, 0.f, DeltaTime, 30.f);
			
			HUDPackage.CrosshairSpread = 0.5f + CrosshairVelocityFactor + CrosshairAirFactor + CrosshairAimFactor + CrosshairShootFactor;
			
			OwnerHUD->SetHUDPackage(HUDPackage);
		}
	}
	
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(EquippedWeapon == nullptr) return;

	if(bIsAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	if(OwnerCharacter && OwnerCharacter->GetFollowCamera())
	{
		OwnerCharacter->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bAiming)
{
	bIsAiming = bAiming;
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

