// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitDynamicObject.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "DynamicOrbitsManager.h"

// Sets default values
AOrbitDynamicObject::AOrbitDynamicObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AOrbitDynamicObject::BeginPlay()
{
	Super::BeginPlay();	
}

void AOrbitDynamicObject::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OrbitalPosition = GetActorLocation();
}

// Called every frame
void AOrbitDynamicObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AOrbitDynamicObject::AppendPredictionPoint(FVector NewPoint, double TimeStep, bool Forced)
{
	bool Result = false;
	double DistanceToFirst = 0.;
	double DistanceToLast = 0.;
	if (!PredictedData.PathPoints.IsEmpty())
	{
		DistanceToFirst = FVector::Dist(OrbitalPosition, PredictedData.PathPoints[0]);
		DistanceToLast = FVector::Dist(NewPoint, PredictedData.LastVisualizedPoint);
	}

	// remove last point, if there's too many already
	if (DistanceToFirst > MinimalPredictionDistance
		&& PredictedData.PointsCount > MaxPredictionPoints
		&& !PredictedData.PathPoints.IsEmpty())
	{
		PredictedData.PathPoints.RemoveAt(0);
		PredictedData.PointsCount -= 1;

		//UE_LOG(LogTemp, Log, TEXT("Point removed. Orbital position: %s, First point: %s, distance: %f"),
		//	*OrbitalPosition.ToCompactString(),
		//	*PredictedData.PathPoints[0].ToCompactString(),
		//	DistanceToFirst
		//);
	}

	if(DistanceToLast > MinimalPredictionDistance
		|| Forced)
	{
		PredictedData.PathPoints.Add(NewPoint);
		PredictedData.PointsCount += 1;

		PredictedData.LastVisualizedPoint = NewPoint;
		//PredictedData.PredictionTimeAccumulator = 0.;
		//UE_LOG(LogTemp, Log, TEXT("Point added. New point: %s, LastPoint: %s, distance: %f"),
		//	*NewPoint.ToCompactString(),
		//	*PredictedData.LastVisualizedPoint.ToCompactString(),
		//	DistanceToLast
		//);
		Result = true;
	}
	else
	{
		//UE_LOG(LogTemp, Log, TEXT("Point skipped. New point: %s, LastPoint: %s, distance: %f"), 
		//	*NewPoint.ToCompactString(), 
		//	*PredictedData.LastVisualizedPoint.ToCompactString(),
		//	DistanceToLast
		//);
		Result = false;
	}

	PredictedData.LastPredictedPoint = NewPoint;
	//PredictionTimeAccumulator += TimeStep;
	return Result;
}

void AOrbitDynamicObject::UpdatePredictionPath()
{
	//if (PredictedData.PointsCount != SplineOrbitPointsCount)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Prediction spline not fully populated for an update. Owner: %s"), *GetDebugName(this));
	//	return;
	//}

	OnPredictionUpdate.Broadcast();
}

void AOrbitDynamicObject::ClearPrediction()
{
	PredictedData.PathPoints.Empty(PredictedData.PointsCount); 
	PredictedData.PointsCount = 0;
}

void AOrbitDynamicObject::RecalculatePrediction()
{
	if (!Manager)
	{
		UE_LOG(LogTemp, Warning, TEXT("No OrbitManager assigned."));
		return;
	}

	Manager->CalculateDynBodyPrediction(this);
	OnPredictionUpdate.Broadcast();
}
