// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Casing.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ACasing::ACasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectSpeed = rand() / static_cast<float>(RAND_MAX) * 100.0f;
	

}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellSpeed());
	
}

void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Collision with floor")));
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	Destroy();
}

float ACasing::ShellSpeed()
{
	float r = (ShellEjectSpeed =  rand() / static_cast<float>(RAND_MAX) * 100.0f);
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Shell Speed: %f"), ShellEjectSpeed));
	}
	return r;
}

