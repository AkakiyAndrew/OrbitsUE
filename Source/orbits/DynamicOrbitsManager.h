// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DynamicOrbitsManager.generated.h"

class AOrbitAttractorBase;
class AOrbitDynamicObject;
class UOrbitalObjectComponent;

UCLASS()
class ORBITS_API ADynamicOrbitsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicOrbitsManager();

private:
	UPROPERTY()
	double TimeAccumulator = 0.f;
	double TimePassed = 0.;
	UPROPERTY()
	TArray<AOrbitAttractorBase*> Attractors;
	UPROPERTY()
	TArray<AOrbitDynamicObject*> DynamicObjects;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly)
	double TimeScale = 1.;
	UPROPERTY(EditInstanceOnly)
	double FixedStep = 0.02;
	UPROPERTY(EditInstanceOnly)
	double SimScale = 1.;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Step(double TimeDelta, double Time);
	UFUNCTION(BlueprintCallable)
	void ComputeStep(FVector &BodyPosition, FVector &BodyVelocity, double TimeDelta, double Time, bool DoLog=true) const;
	UFUNCTION(BlueprintCallable)
	FVector ComputeAcceleration(FVector Position, double Time = 0, bool DoLog=true) const;

	UFUNCTION(BlueprintCallable)
	void CalculateDynBodyPrediction(AOrbitDynamicObject* Body) const;

	UFUNCTION(BlueprintCallable, Category = "Orbital Manager")
	void SetTimeScale(double NewTimeScale) { TimeScale = NewTimeScale; };

	UFUNCTION(BlueprintCallable, Category = "Orbital Manager")
	void ToggleObjectsVisibility(bool NewState);
	
	// spawns dyn orbital object for "local" orbital component's host 
	AOrbitDynamicObject* RegisterDynamicObject(const UOrbitalObjectComponent* OrbitalComponent);
};
