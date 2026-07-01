// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeCameraManager.h"

AFreeCameraManager::AFreeCameraManager(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ViewRollMin = -179.9f;
	ViewRollMax = 179.9f;
}
