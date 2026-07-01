// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FlyingPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ORBITS_API AFlyingPlayerController : public APlayerController
{
	GENERATED_BODY()

	AFlyingPlayerController(const FObjectInitializer& ObjectInitializer);
};
