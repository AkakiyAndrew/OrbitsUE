// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OrbitalBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ORBITS_API UOrbitalBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Orbital")
	static FVector GetOrbitalPosition(double PathFraction, double SemiMajorAxis, double Eccentricity);
};
