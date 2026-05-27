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

void AOrbitDynamicObject::AppendPredictionPoint(FVector NewPoint)
{
	PredictedPathPoint.Add(NewPoint);
	CurrentPathPointsCount += 1;

	if (CurrentPathPointsCount > SplineOrbitPointsCount)
	{
		// remove last point, if there's too many already
		PredictedPathPoint.RemoveAt(0);
		CurrentPathPointsCount -= 1;
	}

	// TODO: spline meshes spawn at begin play/init??
	// 
	// TODO: remove PredictedPathPoint array, replace with spline points - or not?
	// TODO: add new spline point, remove the last
	// OR
	// keep array, update spline meshes?
}

void AOrbitDynamicObject::UpdatePredictionSpline()
{
	if (CurrentPathPointsCount != SplineOrbitPointsCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Prediction spline not fully populated for an update. Owner: %s"), *GetDebugName(this));
		return;
	}

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
