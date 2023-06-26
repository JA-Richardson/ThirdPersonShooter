// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/ThirdPersonGameMode.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AThirdPersonGameMode::PlayerEliminated(AThirdPersonCharacter* ElimmedCharacter,
                                            AThirdPersonPlayerController* ElimmedPlayerController, AThirdPersonPlayerController* EliminatorController)
{
	
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
