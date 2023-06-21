// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACasing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	float ShellSpeed();
	virtual  void SetLifeSpan(float InLifespan) override;
private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* CasingMesh;
	UPROPERTY(EditAnywhere, Category = "Effects")
	float ShellEjectSpeed = rand() / static_cast<float>(RAND_MAX) * 100.0f;
	UPROPERTY(EditAnywhere, Category = "Effects")
	class USoundCue* ImpactSound;
	
};
