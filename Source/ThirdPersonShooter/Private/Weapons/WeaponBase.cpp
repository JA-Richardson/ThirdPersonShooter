// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"

//#include "Character/ThirdPersonCharacter.h"
#include "CADKernel/UI/Display.h"
#include "CADKernel/UI/Display.h"
#include "Character/ThirdPersonCharacter.h"
#include "CompGeom/FitOrientedBox2.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "PlayerController/ThirdPersonPlayerController.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);
	
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

}

void AWeaponBase::Fire(const FVector& HitTarget)
{
	if(FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if(CasingClass)
	{
		const USkeletalMeshSocket* CasingSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if(CasingSocket)
		{
			FTransform CasingTransform = CasingSocket->GetSocketTransform(WeaponMesh);
			FActorSpawnParameters SpawnParams;
			UWorld* World = GetWorld();
			if(World)
			{
				World->SpawnActor<ACasing>(CasingClass, CasingTransform.GetLocation(), CasingTransform.GetRotation().Rotator(), SpawnParams);
			}
		}
	}
	SpendRound();
}

void AWeaponBase::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	Owner = nullptr;
	PlayerController = nullptr;
	
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if(PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnSphereEndOverlap);
	}
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, WeaponState);
	DOREPLIFETIME(AWeaponBase, Ammo);
	
}

void AWeaponBase::OnRep_Owner()
{
	Super::OnRep_Owner();
	if(Owner == nullptr)
	{
		Owner = nullptr;
		PlayerController = nullptr;
	}
	else
		SetHUDAmmo();
	
}

void AWeaponBase::SetHUDAmmo()
{
	Character = Character == nullptr ? Cast<AThirdPersonCharacter>(GetOwner()) : Character;
	if(Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<AThirdPersonPlayerController>(Character->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeaponBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap"));
	AThirdPersonCharacter* Character1 = Cast<AThirdPersonCharacter>(OtherActor);
	if(Character1)
	{
		Character1->SetOverlappingWeapon(this);
	}
}

void AWeaponBase::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//AThirdPersonCharacter* Character = Cast<AThirdPersonCharacter>(OtherActor);
	if(Character)
	{
		Character->SetOverlappingWeapon(nullptr);
	}
}
void AWeaponBase::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch(WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		break;
	case EWeaponState::EWS_Dropped:
		if(HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		break;
	default: ;
	}
	
}
void AWeaponBase::OnRep_WeaponState()
{
	
	switch(WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		break;
	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		break;
	default: ;
	}
}

void AWeaponBase::OnRep_Ammo()
{
	Character = Character == nullptr ? Cast<AThirdPersonCharacter>(GetOwner()) : Character;
	SetHUDAmmo();
}

void AWeaponBase::SpendRound()
{
	Ammo--;
	SetHUDAmmo();
}


void AWeaponBase::ShowPickupWidget(bool bshowWidget)
{
	if(PickupWidget)
	{
		PickupWidget->SetVisibility(bshowWidget);
	}
}



