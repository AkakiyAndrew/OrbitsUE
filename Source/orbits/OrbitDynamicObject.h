// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbitalBase.h"
#include "OrbitDynamicObject.generated.h"

class UOrbitalObjectComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPredictionUpdate);

struct FPredictedData
{
	TArray<FVector> PathPoints; // TODO: remake as TList for quicker removal??
	int32 PointsCount = 0;
	FVector LastPredictedPoint;
	FVector LastVisualizedPoint;

	FVector LastPredictedVelocity;
	double LastPredictedSimTime = 0.;
	//double TimeAccumulator = 0.;
};

class ADynamicOrbitsManager;

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
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals | Prediction params")
	int32 MaxPredictionPoints = 100;

	// niagara component for ribbon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orbit Visuals")
	class UNiagaraComponent* PathRibbonComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:
	FPredictedData PredictedData;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;
	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
	ADynamicOrbitsManager* Manager;
	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
	UOrbitalObjectComponent* LinkedComponent;
	
	//UFUNCTION(BlueprintCallable)
	//void TogglePredictPathVisibility(bool Show);
	
	bool AppendPredictionPoint(FVector NewPoint, double TimeStep, bool Forced = false);
	void UpdateOrbitalMovement(const FVector& NewPosition, const FVector& NewVelocity);
	void UpdatePredictionPath() const;
	void ClearPrediction();
	UFUNCTION(BlueprintCallable, Category = "Orbital")
	void CalculatePrediction();
	UFUNCTION(BlueprintCallable, Category = "Orbital")
	void AddVelocity(const FVector Added) { Velocity += Added; CalculatePrediction(); };


	UFUNCTION(BlueprintCallable, Category = "Orbital")
	FVector GetLastPredictedPoint() const { return PredictedData.LastPredictedPoint; };
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetPredictionPoints() const { return PredictedData.PathPoints; };
	UPROPERTY(BlueprintAssignable, Category = "Orbital")
	FPredictionUpdate OnPredictionUpdate;

	FPredictedData& GetPredictedData() { return PredictedData; };
	int32 GetMaxPredictPoints() const { return MaxPredictionPoints; };

};
