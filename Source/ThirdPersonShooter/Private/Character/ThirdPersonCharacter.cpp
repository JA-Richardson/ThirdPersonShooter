// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ThirdPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "ThirdPersonComponents/CombatComponent.h"
//#include "ToolContextInterfaces.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/ThirdPersonGameMode.h"
#include "Net/UnrealNetwork.h"
#include "ThirdPersonComponents/CombatComponent.h"
#include "Util/ColorConstants.h"
#include "Weapons/WeaponBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerController/ThirdPersonPlayerController.h"
#include "PlayerState/ThirdPersonPlayerState.h"
#include "ThirdPersonShooter/ThirdPersonShooter.h"

// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

// Called every frame
void AThirdPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
	HideCamera();
}

void AThirdPersonCharacter::Elim()
{
	if(Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	MulticastElim();
	GetWorldTimerManager().SetTimer(ElimTimer, this, &AThirdPersonCharacter::ElimTimerFinished, ElimDelay);
}

void AThirdPersonCharacter::MulticastElim_Implementation()
{
	if(ThirdPersonPlayerController)
	{
		ThirdPersonPlayerController->SetHUDWeaponAmmo(0);
	}
}

void AThirdPersonCharacter::ElimTimerFinished()
{
	AThirdPersonGameMode* GameMode = GetWorld()->GetAuthGameMode<AThirdPersonGameMode>();
	if(GameMode)
	{
		GameMode->RequestRespawn(this, Controller);
	}
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();
	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AThirdPersonCharacter::ReceiveDamage);
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if(UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if(LocalPlayerSubsystem)
			{
				LocalPlayerSubsystem->AddMappingContext(CharacterContext, 0);
			}
		}
	}
	
}

void AThirdPersonCharacter::UpdateHUDHealth()
{
	ThirdPersonPlayerController = ThirdPersonPlayerController == nullptr ? Cast<AThirdPersonPlayerController>(Controller) : ThirdPersonPlayerController;
	if(ThirdPersonPlayerController)
	{
		ThirdPersonPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AThirdPersonCharacter::PollInit()
{
	if (PlayerState == nullptr)
	{
		PlayerState = GetPlayerState<AThirdPersonPlayerState>();
		if(PlayerState)
		{
			PlayerState->IncreaseScore(0.f);
			PlayerState->IncreaseDefeat(0);
		}
	}
}


void AThirdPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AThirdPersonCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AThirdPersonCharacter, Health);
}

void AThirdPersonCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(Combat)
	{
		Combat->OwnerCharacter = this;
	}
}

void AThirdPersonCharacter::PlayFireMontage(bool bAiming)
{
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(FireMontage);
	FName SectionName;
	SectionName = bAiming? FName("RifleADS") : FName("RifleHip");
	AnimInstance->Montage_JumpToSection(SectionName, FireMontage);
}



void AThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveAxisValue = Value.Get<FVector2D>();
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MoveAxisValue.Y);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MoveAxisValue.X);
}

void AThirdPersonCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if(IsAiming())
	{
		if(Controller)
		{
			AddControllerYawInput(LookAxisValue.X * 0.5f);
			AddControllerPitchInput(LookAxisValue.Y * 0.5f);
		}
	}
	else
	{
		if(Controller)
		{
			AddControllerYawInput(LookAxisValue.X);
			AddControllerPitchInput(LookAxisValue.Y);
		}
	}
}

void AThirdPersonCharacter::Jump()
{
	Super::Jump();
}

void AThirdPersonCharacter::Interact(const FInputActionValue& Value)
{
	const bool bPressed = Value.Get<bool>();
	if(bPressed)
	{
		if(Combat)
		{
			if(HasAuthority())
			{
				Combat->EquipWeapon(OverlappingWeapon);
			}
			else
			{
				ServerInteract();
			}
		}
	}
}

void AThirdPersonCharacter::AimStart(const FInputActionValue& Value)
{
	
	if(Combat)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Aiming"));
		Combat->SetAiming(true);
	}
	
}

void AThirdPersonCharacter::AimEnd(const FInputActionValue& Value)
{
	if(Combat)
	{
		Combat->SetAiming(false);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AimEnd"));
	}

}

void AThirdPersonCharacter::CrouchStart()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	
	
}

void AThirdPersonCharacter::Fire()
{
	if(Combat)
	{
		Combat->FireButtonPressed(true);
	}
	
}

void AThirdPersonCharacter::FireEnd()
{
	if(Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AThirdPersonCharacter::AimOffset(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	const float Speed = Velocity.Size();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();

	if(Speed == 0.f && !bIsInAir) //not moving
	{
		FRotator CurrentRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, StartingAimRotation);
		AO_Yaw = DeltaRotation.Yaw;
		bUseControllerRotationYaw = false;
	}
	if(Speed > 0.0f || bIsInAir) // moving
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	
}

void AThirdPersonCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	if(Health == 0.f)
	{
		//disable physics
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
		//disable movement
		GetCharacterMovement()->DisableMovement();
		//disable combat
		if(Combat)
		{
			Combat->SetAiming(false);
			Combat->FireButtonPressed(false);
		}
		//disable input
		DisableInput(ThirdPersonPlayerController);
		//disable camera
		ThirdPersonPlayerController->SetViewTargetWithBlend(nullptr, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
		//disable camera rotation
		
		
		AThirdPersonGameMode* GameMode = GetWorld()->GetAuthGameMode<AThirdPersonGameMode>();
		if(GameMode)
		{
			ThirdPersonPlayerController = ThirdPersonPlayerController == nullptr ? Cast<AThirdPersonPlayerController>(Controller) : ThirdPersonPlayerController;
			AThirdPersonPlayerController* AttackerController = Cast<AThirdPersonPlayerController>(InstigatedBy);
			GameMode->PlayerEliminated(this, ThirdPersonPlayerController, AttackerController);
		}
	}
	
	
}

void AThirdPersonCharacter::SetOverlappingWeapon(AWeaponBase* Weapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool AThirdPersonCharacter::IsAiming() const
{
	return (Combat && Combat->bIsAiming);
}

void AThirdPersonCharacter::OnRep_OverlappingWeapon(AWeaponBase* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AThirdPersonCharacter::HideCamera()
{
	if(!IsLocallyControlled()) return;
	if((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
	
}

void AThirdPersonCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	//Hit react montage goes here
}



void AThirdPersonCharacter::ServerInteract_Implementation()
{
	if(Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Jump);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Interact);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::AimStart);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::AimEnd);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::CrouchStart);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Fire);
	}

}



