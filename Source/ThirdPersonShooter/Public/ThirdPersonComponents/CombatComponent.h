// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/ThirdPersonHUD.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRDPERSONSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	friend class AThirdPersonCharacter;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void EquipWeapon(class AWeaponBase* WeaponToEquip);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bAiming);
	void FireButtonPressed(bool bPressed);
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& HitLocation);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& HitLocation);
	void CrosshairTrace(FHitResult& OutHitResult);
	void SetHudCrosshair(float DeltaTime);
	
private:
	class AThirdPersonCharacter* OwnerCharacter;
	class AThirdPersonPlayerController* OwnerController;
	class AThirdPersonHUD* OwnerHUD;
	UPROPERTY(Replicated)
	class AWeaponBase* EquippedWeapon;
	UPROPERTY(Replicated)
	bool bIsAiming;
	bool bFireButtonPressed;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed = 600.f;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
	FVector HitTarget;

	//HUD and Crosshairs
	float CrosshairVelocityFactor;
	float CrosshairAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootFactor;
	FHUDPackage HUDPackage;

	//Aiming and FOV
	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float ZoomedFOV = 30.f;

	float CurrentFOV;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float ZoomInterpSpeed;

	void InterpFOV(float DeltaTime);

		
};
