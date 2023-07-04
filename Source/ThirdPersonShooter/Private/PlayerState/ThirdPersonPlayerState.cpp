// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ThirdPersonPlayerState.h"

#include "Character/ThirdPersonCharacter.h"
#include "PlayerController/ThirdPersonPlayerController.h"

void AThirdPersonPlayerState::IncreaseScore(float ScoreAmount)
{
	float CurrentScore = GetScore();
	
	SetScore(CurrentScore += ScoreAmount);
	ThirdPersonCharacter = ThirdPersonCharacter == nullptr ? Cast<AThirdPersonCharacter>(GetPawn()) : ThirdPersonCharacter;
	if(ThirdPersonCharacter)
	{
		ThirdPersonPlayerController = ThirdPersonPlayerController == nullptr ? Cast<AThirdPersonPlayerController>(ThirdPersonCharacter->Controller) : ThirdPersonPlayerController;
		if(ThirdPersonPlayerController)
		{
			ThirdPersonPlayerController->SetHUDScore(GetScore());
		}
	}
	
}

void AThirdPersonPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	ThirdPersonCharacter = ThirdPersonCharacter == nullptr ? Cast<AThirdPersonCharacter>(GetPawn()) : ThirdPersonCharacter;
	if(ThirdPersonCharacter)
	{
		ThirdPersonPlayerController = ThirdPersonPlayerController == nullptr ? Cast<AThirdPersonPlayerController>(ThirdPersonCharacter->Controller) : ThirdPersonPlayerController;
		if(ThirdPersonPlayerController)
		{
			ThirdPersonPlayerController->SetHUDScore(GetScore());
		}
	}
}


