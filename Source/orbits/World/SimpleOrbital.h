// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "orbits/Orbital/GravityAffected.h"
#include "SimpleOrbital.generated.h"

UCLASS()
class ORBITS_API ASimpleOrbital : public AActor, public IGravityAffected
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASimpleOrbital();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
