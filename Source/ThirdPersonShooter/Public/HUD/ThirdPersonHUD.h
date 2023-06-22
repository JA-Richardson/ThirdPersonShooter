// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ThirdPersonHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairCentre;
	class UTexture2D* CrosshairLeft;
	class UTexture2D* CrosshairRight;
	class UTexture2D* CrosshairTop;
	class UTexture2D* CrosshairBottom;
	
};

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API AThirdPersonHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual  void DrawHUD() override;
private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCentre);
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
	
};
