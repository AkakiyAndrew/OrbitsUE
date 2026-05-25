// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbitalBase.h"
#include "OrbitDynamicObject.generated.h"

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
	double PredictionStep = 0.02f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents();

private:
	//TList<FVector> PathPoints;
	TArray<FVector> PredictedPathPoint; // TODO: remake as TList for quicker removal??
	int32 CurrentPathPointsCount = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	FVector OrbitalPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;
	FVector LastPredictedVelocity;
	double LastPredictedSimTime = 0.;

	UFUNCTION(BlueprintCallable)
	void TogglePredictPathVisibility(bool Show);

	void AppendPredictionPoint(FVector NewPoint);
	FVector GetLastPredictedPoint() const { return PredictedPathPoint[CurrentPathPointsCount - 1]; };
	void UpdatePredictionSpline();
};
