// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitDynamicObject.h"
#include "OrbitManager.h"

// Sets default values
UOrbitDynamicObjectComponent::UOrbitDynamicObjectComponent()
{
 	// Set this component to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
}

void UOrbitDynamicObjectComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner())
	{
		OrbitalPosition = GetOwner()->GetActorLocation();
	}
}

// Called every frame
void UOrbitDynamicObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UOrbitDynamicObjectComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CalculatePrediction(); // request from manager after all Attractors are registered
}

void UOrbitDynamicObjectComponent::OrbitalInit()
{
	Super::OrbitalInit();
	
	Manager->RegisterObject(this);
}

bool UOrbitDynamicObjectComponent::AppendPredictionPoint(const FVector& NewPoint, const double TimeStep, const bool Forced)
{
	// TODO: add argument of distance to the closest attractor, if pass min threshold then add point
	// TODO: cull points that left behind (make array of SimTime for points?)
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
	PredictedData.LastPredictedSimTime += TimeStep;
	//PredictionTimeAccumulator += TimeStep;
	return Result;
}

void UOrbitDynamicObjectComponent::UpdateOrbitalMovement(const FVector& NewPosition, const FVector& NewVelocity)
{
	SetOrbitalPosition(NewPosition);
	Velocity = NewVelocity;

	if (GetOwner())
	{
		GetOwner()->SetActorLocation(NewPosition);
	}
}

void UOrbitDynamicObjectComponent::UpdatePredictionPath() const
{
	OnPredictionUpdate.Broadcast();
}

void UOrbitDynamicObjectComponent::ClearPrediction()
{
	PredictedData.PathPoints.Empty(PredictedData.PointsCount); 
	PredictedData.PointsCount = 0;
}

void UOrbitDynamicObjectComponent::CalculatePrediction()
{
	if (!Manager)
	{
		UE_LOG(LogTemp, Warning, TEXT("No OrbitManager assigned."));
		return;
	}

	Manager->CalculateDynBodyPrediction(this);
	OnPredictionUpdate.Broadcast();
}

void UOrbitDynamicObjectComponent::AddOrbitalVelocity(const FVector& VelocityDelta)
{
	OrbitalPosition += VelocityDelta;
	CalculatePrediction();
}
