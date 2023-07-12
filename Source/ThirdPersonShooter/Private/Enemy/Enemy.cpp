// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "Components/CapsuleComponent.h"
#include "GameModes/ThirdPersonGameMode.h"
#include "ThirdPersonShooter/ThirdPersonShooter.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);

	OnTakeAnyDamage.AddDynamic(this, &AEnemy::ReceiveDamage);
	
}

void AEnemy::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
	AActor* DamageCauser)
{
    UE_LOG(LogTemp, Warning, TEXT("Damage taken"));
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	if (Health == 0.f)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
		UE_LOG(LogTemp, Warning, TEXT("Enemy is dead"));
	}
}

void AEnemy::ElimTimerFinished()
{
	AThirdPersonGameMode* GameMode = GetWorld()->GetAuthGameMode<AThirdPersonGameMode>();
	if(GameMode)
	{
		GameMode->RequestRespawn(this, Controller);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Defeated()
{
	GetWorldTimerManager().SetTimer(ElimTimer, this, &AEnemy::ElimTimerFinished, ElimDelay);
}

