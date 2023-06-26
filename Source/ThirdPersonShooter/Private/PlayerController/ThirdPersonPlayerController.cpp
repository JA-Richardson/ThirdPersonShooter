// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/ThirdPersonPlayerController.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/ThirdPersonHUD.h"
void AThirdPersonPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ThirdPersonHUD = Cast<AThirdPersonHUD>(GetHUD());
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


