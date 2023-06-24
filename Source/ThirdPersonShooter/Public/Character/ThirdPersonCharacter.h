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
	// Sets default values for this character's properties
	AThirdPersonCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void SetOverlappingWeapon(AWeaponBase* Weapon);
	bool IsAiming() const;
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	void PlayFireMontage(bool bAiming);
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
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
	UInputAction* InteractAction;
	void Interact(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* AimAction;
	void AimStart(const FInputActionValue& Value);
	void AimEnd(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* CrouchAction;
	void CrouchStart();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* FireAction;
	void Fire();
	void FireEnd();

	void AimOffset(float DeltaTime);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

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

public:
	
};
