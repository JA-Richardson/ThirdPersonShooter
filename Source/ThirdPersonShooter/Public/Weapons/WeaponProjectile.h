// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponBase.h"
#include "WeaponProjectile.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API AWeaponProjectile : public AWeaponBase
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& HitTarget) override;
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<class AProjectileBase> ProjectileClass;
	
};
