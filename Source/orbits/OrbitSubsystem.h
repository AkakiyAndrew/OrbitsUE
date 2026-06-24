// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OrbitSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class ORBITS_API UOrbitSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	class AOrbitManager* Manager = nullptr;
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	AOrbitManager* GetOrbitManager() const {return Manager;};
};
