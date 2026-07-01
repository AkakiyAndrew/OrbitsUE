// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingPlayerController.h"

#include "FreeCameraManager.h"

AFlyingPlayerController::AFlyingPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AFreeCameraManager::StaticClass();
}
