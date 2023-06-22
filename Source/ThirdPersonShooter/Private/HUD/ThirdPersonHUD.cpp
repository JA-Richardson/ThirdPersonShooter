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
	
		if(HUDPackage.CrosshairCentre)
		{
			DrawCrosshair(HUDPackage.CrosshairCentre, ViewportCentre);
		}
		if(HUDPackage.CrosshairLeft)
		{
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCentre);
		}
		if(HUDPackage.CrosshairRight)
		{
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCentre);
		}
		if(HUDPackage.CrosshairTop)
		{
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCentre);
		}
		if(HUDPackage.CrosshairBottom)
		{
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCentre);
		}
	}
	
}

void AThirdPersonHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCentre)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCentre.X - (TextureWidth/2.f), ViewportCentre.Y - (TextureHeight/2.f));

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, FLinearColor::White);
}
