// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "FreeCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ORBITS_API AFreeCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	AFreeCameraManager(const FObjectInitializer& ObjectInitializer);
	
public:
	void ToggleFreeCameraMode(bool IsLanded);
};
