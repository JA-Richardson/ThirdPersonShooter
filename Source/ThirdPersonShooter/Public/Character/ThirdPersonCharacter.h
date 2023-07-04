// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/CrosshairInteractInterface.h"
#include "ThirdPersonCharacter.generated.h"

class UInputAction;
struct FInputActionValue;
class UInputMappingContext;

UCLASS()
class THIRDPERSONSHOOTER_API AThirdPersonCharacter : public ACharacter, public ICrosshairInteractInterface
{
	GENERATED_BODY()

public:
	AThirdPersonCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void SetOverlappingWeapon(AWeaponBase* Weapon);
	bool IsAiming() const;
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	void PlayFireMontage(bool bAiming);
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	class AThirdPersonPlayerState* PlayerState;

protected:
	
	virtual void BeginPlay() override;

	//Movement and Interaction
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputMappingContext* CharacterContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* MoveAction;
	void Move(const FInputActionValue& Value);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* LookAction;
	void Look(const FInputActionValue& Value);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* JumpAction;
	void Jump();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* CrouchAction;
	void CrouchStart();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* InteractAction;
	void Interact(const FInputActionValue& Value);
	
	//Aiming and Firing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* AimAction;
	void AimStart(const FInputActionValue& Value);
	void AimEnd(const FInputActionValue& Value);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* FireAction;
	void Fire();
	void FireEnd();
	void AimOffset(float DeltaTime);
	
	//Damage and Health
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                   class AController* InstigatedBy, AActor* DamageCauser);
	void UpdateHUDHealth();
	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health = 100.f;
	//Poll and initialise any data for the HUD
	void PollInit();

private:

	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	void HideCamera();
	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraThreshold = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeaponBase* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeaponBase* LastWeapon);
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;
	UFUNCTION(Server, Reliable)
	void ServerInteract();

	float AO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	UPROPERTY(EditAnywhere, Category = "Aim Offset")
	class UAnimMontage* FireMontage;
	
	//Player Health and Damage
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UFUNCTION()
	void OnRep_Health();
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float ElimDelay = 3.f;
	void ElimTimerFinished();

	class AThirdPersonPlayerController* ThirdPersonPlayerController;

public:
};
