// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ThirdPersonHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/CharacterOverlay.h"

void AThirdPersonHUD::BeginPlay()
{
	Super::BeginPlay();
	AddCharacterOverlay();
}

void AThirdPersonHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}
void AThirdPersonHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCentre(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		float SpreadScaled = CrosshairSpeadMax * HUDPackage.CrosshairSpread;
	
		if(HUDPackage.CrosshairCentre)
		{
			const FVector2D Spread(0.f,0.f);
			DrawCrosshair(HUDPackage.CrosshairCentre, ViewportCentre, Spread, HUDPackage.CrosshairColour);
		}
		if(HUDPackage.CrosshairLeft)
		{
			const FVector2D Spread(-SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCentre,Spread, HUDPackage.CrosshairColour);
		}
		if(HUDPackage.CrosshairRight)
		{
			const FVector2D Spread(SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCentre, Spread, HUDPackage.CrosshairColour);
		}
		if(HUDPackage.CrosshairTop)
		{
			const FVector2D Spread(0.f,-SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCentre, Spread, HUDPackage.CrosshairColour);
		}
		if(HUDPackage.CrosshairBottom)
		{
			const FVector2D Spread(0.f,SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCentre, Spread, HUDPackage.CrosshairColour);
		}
	}
	
}



void AThirdPersonHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCentre, FVector2D Spread, FLinearColor Color)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCentre.X - (TextureWidth/2.f) + Spread.X, ViewportCentre.Y - (TextureHeight/2.f) + Spread.Y);

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, Color);
}
