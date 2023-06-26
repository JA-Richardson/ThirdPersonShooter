// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "ThirdPersonGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API AThirdPersonGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void PlayerEliminated(class AThirdPersonCharacter* ElimmedCharacter, class AThirdPersonPlayerController* ElimmedPlayerController, AThirdPersonPlayerController * EliminatorController);

	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	
};
