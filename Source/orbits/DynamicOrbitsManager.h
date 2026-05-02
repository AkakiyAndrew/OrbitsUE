// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DynamicOrbitsManager.generated.h"

class AOrbitAttractorBase;
class AOrbitDynamicObject;

UCLASS()
class ORBITS_API ADynamicOrbitsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicOrbitsManager();

private:
	UPROPERTY()
	float TimeAccumulator = 0.f;
	TArray<AOrbitAttractorBase*> Attractors;
	TArray<AOrbitDynamicObject*> DynamicObjects;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float TimeScale = 1.f;
	UPROPERTY(EditDefaultsOnly)
	float FixedStep = 0.02f;
	UPROPERTY(EditDefaultsOnly)
	int32 PreviewSteps = 100;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FVector ComputeAcceleration(FVector Position, float Time = 0);
	UFUNCTION(BlueprintCallable)
	void Step(float TimeDelta, float Time);
	void ComputeStep(FVector &BodyPosition, FVector &BodyVelocity, float TimeDelta, float Time);
};
