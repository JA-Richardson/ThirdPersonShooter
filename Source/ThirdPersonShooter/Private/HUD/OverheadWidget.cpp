// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayName(FString Text)
{
	if(PlayerName)
	{
		PlayerName->SetText(FText::FromString(Text));
	}
	
}

void UOverheadWidget::SetRoleName(FString Text)
{
	if(NetRole)
	{
		NetRole->SetText(FText::FromString(Text));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{

	ENetRole LocalRole = InPawn->GetRemoteRole();
	FString Role;
	switch (LocalRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString LocalRoleString = FString::Printf(TEXT("%s"), *Role);
	SetRoleName(LocalRoleString);
	
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	const APlayerState* PlayerState = InPawn->GetPlayerState<APlayerState>();
	if(PlayerState)
	{
		FString PlayerNameString = FString::Printf(TEXT("%s"), *PlayerState->GetPlayerName());
		SetDisplayName(PlayerNameString);
	}
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
