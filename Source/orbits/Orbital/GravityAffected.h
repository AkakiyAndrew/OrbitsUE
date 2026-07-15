// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GravityAffected.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UGravityAffected : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ORBITS_API IGravityAffected
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void RotateToGravity(const FVector& Direction) {};
	virtual bool TryLand(double Speed) { return false; };
};
