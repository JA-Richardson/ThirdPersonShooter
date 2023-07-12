// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Enemy.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Defeated();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float Health = 100.f;
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
					   class AController* InstigatedBy, AActor* DamageCauser);
private:
	UPROPERTY(EditAnywhere, Category = "Enemy Stats")
	float MaxHealth = 100.f;

	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	float ElimDelay = 3.f;
	void ElimTimerFinished();

	

};
