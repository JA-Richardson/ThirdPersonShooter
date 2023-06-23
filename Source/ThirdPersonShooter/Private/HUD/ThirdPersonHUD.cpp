// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ThirdPersonHUD.h"


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
			DrawCrosshair(HUDPackage.CrosshairCentre, ViewportCentre, Spread);
		}
		if(HUDPackage.CrosshairLeft)
		{
			const FVector2D Spread(-SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCentre,Spread);
		}
		if(HUDPackage.CrosshairRight)
		{
			const FVector2D Spread(SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCentre, Spread);
		}
		if(HUDPackage.CrosshairTop)
		{
			const FVector2D Spread(0.f,-SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCentre, Spread);
		}
		if(HUDPackage.CrosshairBottom)
		{
			const FVector2D Spread(0.f,SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCentre, Spread);
		}
	}
	
}

void AThirdPersonHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCentre, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCentre.X - (TextureWidth/2.f) + Spread.X, ViewportCentre.Y - (TextureHeight/2.f) + Spread.Y);

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, FLinearColor::White);
}
