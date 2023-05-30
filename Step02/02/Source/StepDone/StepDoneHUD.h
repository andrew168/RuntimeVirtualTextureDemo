// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StepDoneHUD.generated.h"

UCLASS()
class AStepDoneHUD : public AHUD
{
	GENERATED_BODY()

public:
	AStepDoneHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

