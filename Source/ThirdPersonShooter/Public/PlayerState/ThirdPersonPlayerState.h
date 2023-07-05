// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ThirdPersonPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API AThirdPersonPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	void IncreaseScore(float ScoreAmount);
	void IncreaseDefeat(int DefeatAmount);
	UFUNCTION()
	virtual void OnRep_Defeat();
	
private:
	UPROPERTY()
	class AThirdPersonCharacter* ThirdPersonCharacter;
	UPROPERTY()
	class AThirdPersonPlayerController* ThirdPersonPlayerController;
	UPROPERTY(ReplicatedUsing = OnRep_Defeat)
	int32 Defeat;
	
};
