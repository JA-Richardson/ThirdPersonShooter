// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/ThirdPersonPlayerController.h"

#include "Character/ThirdPersonCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/ThirdPersonHUD.h"
void AThirdPersonPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ThirdPersonHUD = Cast<AThirdPersonHUD>(GetHUD());
}

void AThirdPersonPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AThirdPersonCharacter* TPCharacter = Cast<AThirdPersonCharacter>(InPawn);
	if(TPCharacter)
	{
		SetHUDHealth(TPCharacter->GetHealth(), TPCharacter->GetMaxHealth());
	}

	
}

void AThirdPersonPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	ThirdPersonHUD = ThirdPersonHUD == nullptr ? Cast<AThirdPersonHUD>(GetHUD()) : ThirdPersonHUD;
	const bool bHUDValid = ThirdPersonHUD && ThirdPersonHUD->CharacterOverlay && ThirdPersonHUD->CharacterOverlay->HealthBar && ThirdPersonHUD->CharacterOverlay->HealthText;
	if(bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		ThirdPersonHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		const FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		ThirdPersonHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	
		
	
}




