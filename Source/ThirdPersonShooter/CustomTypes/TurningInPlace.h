#pragma once
#include "Datasmith/DatasmithCore/Public/DatasmithDefinitions.h"

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_None UMETA(DisplayName = "None"),
	ETIP_Left UMETA(DisplayName = "Left"),
	ETIP_Right UMETA(DisplayName = "Right"),
	ETIP_MAX UMETA(DisplayName = "DefaultMAX")
};


class TurningInPlace
{
public:
	
};
