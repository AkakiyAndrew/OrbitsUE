// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitDynamicObject.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

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

	//if(ShowPredictedOrbit)
	//{
	//	int Count = 0;
	//	for (FVector& Point : PredictedPathPoint)
	//	{
	//		if (PredictedPathPoint.IsValidIndex(Count + 1))
	//		{
	//			DrawDebugLine(GetWorld(), Point, PredictedPathPoint[Count + 1], PreviewLinesColor, false);
	//		}
	//		Count += 1;
	//	}
	//}
}

void AOrbitDynamicObject::TogglePredictPathVisibility(bool Show)
{
	PredictionSplinePath->SetVisibility(Show);
}

bool AOrbitDynamicObject::AppendPredictionPoint(FVector NewPoint, double TimeStep, bool Forced)
{
	bool Result = false;
	double DistanceToFirst = 0.;
	double DistanceToLast = 0.;
	if (!PredictedPathPoint.IsEmpty())
	{
		DistanceToFirst = FVector::Dist(OrbitalPosition, PredictedPathPoint[0]);
		DistanceToLast = FVector::Dist(NewPoint, PredictedPathPoint.Last());
	}

	// remove last point, if there's too many already
	if (DistanceToFirst > MinimalPredictionDistance
		&& CurrentPathPointsCount > SplineOrbitPointsCount
		&& !PredictedPathPoint.IsEmpty())
	{
		PredictedPathPoint.RemoveAt(0);
		CurrentPathPointsCount -= 1;

		UE_LOG(LogTemp, Log, TEXT("Point removed. Orbital position: %s, First point: %s, distance: %f"),
			*OrbitalPosition.ToCompactString(),
			*PredictedPathPoint[0].ToCompactString(),
			DistanceToFirst
		);
	}

	if(DistanceToLast > MinimalPredictionDistance
		|| Forced)
	{
		PredictedPathPoint.Add(NewPoint);
		CurrentPathPointsCount += 1;

		LastVisualizedPoint = NewPoint;
		PredictionTimeAccumulator = 0.;
		UE_LOG(LogTemp, Log, TEXT("Point added. New point: %s, LastPoint: %s, distance: %f"),
			*NewPoint.ToCompactString(),
			*LastVisualizedPoint.ToCompactString(),
			DistanceToLast
		);
		Result = true;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Point skipped. New point: %s, LastPoint: %s, distance: %f"), 
			*NewPoint.ToCompactString(), 
			*LastVisualizedPoint.ToCompactString(), 
			DistanceToLast
		);
		Result = false;
	}

	//PredictedPathPoint.Add(NewPoint);
	//CurrentPathPointsCount += 1;

	//if (CurrentPathPointsCount > SplineOrbitPointsCount)
	//{
	//	// remove last point, if there's too many already
	//	PredictedPathPoint.RemoveAt(0);
	//	CurrentPathPointsCount -= 1;
	//}

	LastPredictedPoint = NewPoint;
	PredictionTimeAccumulator += TimeStep;
	return Result;
}

void AOrbitDynamicObject::UpdatePredictionSpline()
{
	//if (CurrentPathPointsCount != SplineOrbitPointsCount)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Prediction spline not fully populated for an update. Owner: %s"), *GetDebugName(this));
	//	return;
	//}

	OnPredictionUpdate.Broadcast();

	//PredictionSplinePath->SetSplinePoints(PredictedPathPoint, ESplineCoordinateSpace::World, true);

	//// repurpose already created components
	//for (int32 PointIndex = 0; PointIndex < SplineOrbitPointsCount; PointIndex++)
	//{
	//	if (!SplineMeshes[PointIndex])
	//		continue;

	//	// Start and End points/tangents
	//	FVector StartPos, StartTangent, EndPos, EndTangent;
	//	PredictionSplinePath->GetLocationAndTangentAtSplinePoint(PointIndex, StartPos, StartTangent, ESplineCoordinateSpace::Local);
	//	PredictionSplinePath->GetLocationAndTangentAtSplinePoint(PointIndex + 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

	//	SplineMeshes[PointIndex]->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
	//}

	//PredictionSplinePath->UpdateSpline();
}
