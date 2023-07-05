// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/ThirdPersonGameMode.h"

#include "Character/ThirdPersonCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/ThirdPersonPlayerController.h"
#include "PlayerState/ThirdPersonPlayerState.h"


void AThirdPersonGameMode::PlayerEliminated(AThirdPersonCharacter* ElimmedCharacter,
                                            AThirdPersonPlayerController* ElimmedPlayerController, AThirdPersonPlayerController* EliminatorController)
{
	AThirdPersonPlayerState* AttackerState = EliminatorController ? Cast<AThirdPersonPlayerState>(EliminatorController->PlayerState) : nullptr;
	AThirdPersonPlayerState* VictimState = ElimmedPlayerController ? Cast<AThirdPersonPlayerState>(ElimmedPlayerController->PlayerState) : nullptr;

	if(AttackerState && AttackerState != VictimState)
	{
		AttackerState->IncreaseScore(1.f);
	}
	if(VictimState)
	{
		VictimState->IncreaseDefeat(1);
	}
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void AThirdPersonGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if(ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if(ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num()-1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
