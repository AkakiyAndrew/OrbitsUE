// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbitalBase.h"
#include "OrbitDynamicObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPredictionUpdate);

UCLASS()
class ORBITS_API AOrbitDynamicObject : public AOrbitalBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbitDynamicObject();

protected:
	// Dynamic Prediction Params
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals | Prediction params")
	double PredictionStep = 0.02;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals | Prediction params")
	double MinimalPredictionDistance = 5.;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals | Prediction params")
	double MaximalPredictionWaitTime = 2.; // top limit for time accumulator since last prediction update

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents();

private:
	//TList<FVector> PathPoints;
	TArray<FVector> PredictedPathPoint; // TODO: remake as TList for quicker removal??
	int32 CurrentPathPointsCount = 0;
	FVector LastPredictedPoint;
	FVector LastVisualizedPoint;
	double PredictionTimeAccumulator = 0.;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Orbital")
	FVector OrbitalPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;
	FVector LastPredictedVelocity;
	double LastPredictedSimTime = 0.;

	UFUNCTION(BlueprintCallable)
	void TogglePredictPathVisibility(bool Show);

	bool AppendPredictionPoint(FVector NewPoint, double TimeStep, bool Forced = false);
	UFUNCTION(BlueprintCallable, Category = "Orbital")
	FVector GetLastPredictedPoint() const { return LastPredictedPoint; };
	int32 GetCurrentPredictionPointCount() const { return CurrentPathPointsCount; };
	void UpdatePredictionSpline();

	UPROPERTY(BlueprintAssignable, Category = "Orbital")
	FPredictionUpdate OnPredictionUpdate;

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetPredictionPoints() const { return PredictedPathPoint; };
};
