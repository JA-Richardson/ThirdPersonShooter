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
	virtual void OnRep_Score() override;
	void IncreaseScore(float ScoreAmount);
private:
	class AThirdPersonCharacter* ThirdPersonCharacter;
	class AThirdPersonPlayerController* ThirdPersonPlayerController;
	
};
