// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbitalBase.h"
#include "GameFramework/Actor.h"
#include "OrbitManager.generated.h"

class UOrbitAttractorBaseComponent; // Changed from AOrbitAttractorBase
class UOrbitDynamicObjectComponent; // Changed from AOrbitDynamicObject
class UOrbitalObjectComponent;

UCLASS()
class ORBITS_API AOrbitManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbitManager();

private:
	UPROPERTY()
	double TimeAccumulator = 0.f;
	double TimePassed = 0.;
	UPROPERTY()
	TArray<UOrbitAttractorBaseComponent*> Attractors; // Changed from AOrbitAttractorBase
	UPROPERTY()
	TArray<UOrbitDynamicObjectComponent*> DynamicObjects; // Changed from AOrbitDynamicObject
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditInstanceOnly)
	double TimeScale = 1.;
	UPROPERTY(EditInstanceOnly)
	double FixedStep = 0.02;
	UPROPERTY(EditInstanceOnly)
	double SimScale = 1.;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PreInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	void Step(double TimeDelta, double Time);
	UFUNCTION(BlueprintCallable)
	void ComputeStep(FVector &BodyPosition, FVector &BodyVelocity, double TimeDelta, double Time, bool DoLog=true) const;
	UFUNCTION(BlueprintCallable)
	FVector ComputeAcceleration(FVector Position, double Time = 0, bool DoLog=true) const;

	UFUNCTION(BlueprintCallable)
	void CalculateDynBodyPrediction(UOrbitDynamicObjectComponent* Body) const; // Changed from AOrbitDynamicObject

	UFUNCTION(BlueprintCallable, Category = "Orbital Manager")
	void SetTimeScale(double NewTimeScale) { TimeScale = NewTimeScale; };
	double GetTimeStep() const { return FixedStep * SimScale; }
	
	UFUNCTION(BlueprintCallable, Category = "Orbital Manager")
	void ToggleObjectsVisibility(bool NewState);
	
	// spawns dyn orbital object for "local" orbital component's host 
	void RegisterObject(UOrbitalBaseComponent* OrbitalComponent); // Changed from AOrbitDynamicObject
};