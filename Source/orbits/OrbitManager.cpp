// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitManager.h"

#include "OrbitAttractorBase.h"
#include "OrbitAttractorKepler.h"
#include "Kismet/GameplayStatics.h"
#include "OrbitDynamicObject.h"

// Sets default values
AOrbitManager::AOrbitManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOrbitManager::BeginPlay()
{
	Super::BeginPlay();
	
	// The following logic needs to be re-evaluated as AOrbitDynamicObject and AOrbitAttractorBase are now components.
	// Components are not found directly by GetAllActorsOfClass.
	// A new mechanism for registering or discovering these components will be needed.

	// TArray<AActor*> Buf;
	// UGameplayStatics::GetAllActorsOfClass(this, AOrbitDynamicObject::StaticClass(), Buf);
	// for (AActor*& Actor : Buf)
	// {
	// 	DynamicObjects.Add(Cast<UOrbitDynamicObjectComponent>(Actor)); // This cast will fail
	// }
	// Buf.Empty();

	// UGameplayStatics::GetAllActorsOfClass(this, AOrbitAttractorBase::StaticClass(), Buf);
	// for (AActor*& Actor : Buf)
	// {
	// 	Attractors.Add(Cast<UOrbitAttractorBaseComponent>(Actor)); // This cast will fail
	// }
	// Buf.Empty();

	
	
	// first (big) prediction for each dynamic object
	// for (UOrbitDynamicObjectComponent*& Body : DynamicObjects) // Updated type
	// {
	// 	if (Body) // Ensure Body is valid
	// 	{
	// 		Body->Manager = this;
	// 		CalculateDynBodyPrediction(Body);
	// 	}
	// }
}

// Called every frame
void AOrbitManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeAccumulator += TimeScale * DeltaTime;

	while (TimeAccumulator > FixedStep)
	{
		Step(FixedStep, TimePassed);
		
		for (UOrbitAttractorBaseComponent*& Attractor: Attractors) // Updated type
		{
			if (UOrbitAttractorKeplerComponent* OrbitingAttractor = Cast<UOrbitAttractorKeplerComponent>(Attractor)) // Updated type
			{
				OrbitingAttractor->UpdateOrbitalPosition(TimePassed);
			}
		}
		
		TimePassed += FixedStep;
		TimeAccumulator -= FixedStep;
	}
}

void AOrbitManager::Step(double TimeDelta, double Time)
{
	for (UOrbitDynamicObjectComponent* &Body : DynamicObjects) // Updated type
	{
		if (!Body) continue; // Ensure Body is valid

		// wrap to func, for predictions
		FVector Pos = Body->GetOrbitalPosition();
		FVector Velocity = Body->Velocity;

		ComputeStep(Pos, Velocity, FixedStep, Time);
		
		Body->UpdateOrbitalMovement(Pos, Velocity);

		// remove oldest predicted point and predict new one, from last position
		FVector LastPoint = Body->GetLastPredictedPoint();
		ComputeStep(LastPoint, Body->GetPredictedData().LastPredictedVelocity, FixedStep, Body->GetPredictedData().LastPredictedSimTime, false);
		Body->AppendPredictionPoint(LastPoint, FixedStep);
		Body->UpdatePredictionPath();
	}
}

void AOrbitManager::ComputeStep(FVector &BodyPosition, FVector &BodyVelocity, double TimeDelta, double Time, bool DoLog) const
{
	const FVector Acc0 = ComputeAcceleration(BodyPosition, Time, DoLog);
	BodyPosition += BodyVelocity * FixedStep + 0.5 * Acc0 * FixedStep * FixedStep;

	const FVector Acc1 = ComputeAcceleration(BodyPosition, Time + FixedStep, DoLog);
	BodyVelocity += 0.5 * (Acc0 + Acc1) * FixedStep;
}

FVector AOrbitManager::ComputeAcceleration(FVector Position, double Time, bool DoLog) const
{
	FVector Acceleration = FVector();

	for (const UOrbitAttractorBaseComponent* Attractor : Attractors) // Updated type
	{
		if (!Attractor) continue; // Ensure Attractor is valid

		FVector AttractorPosition = Attractor->GetMassCenterPosition(Time);
		if(!DoLog)
			DrawDebugSphere(GetWorld(), AttractorPosition, 30, 10, FColor::Red);
		FVector VectorToAttractor = AttractorPosition - Position;
		double DistSq = VectorToAttractor.SquaredLength();
		double InvDist3 = 1.0 / (FMath::Sqrt(DistSq) * DistSq);

		Acceleration += VectorToAttractor * (Attractor->GetBodyGM() * InvDist3);
	}
	return Acceleration;
}

void AOrbitManager::CalculateDynBodyPrediction(UOrbitDynamicObjectComponent* Body) const // Updated type
{
	if (!Body)
	{
		return;
	}

	// check owner
	if (Body->GetOwner() && Body->GetOwner()->IsActorBeingDestroyed())
	{
		return;
	}

	if (!DynamicObjects.Contains(Body))
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to calculate prediction for body, that not in OrbitManager: %s."), *Body->GetName()); // Updated to GetName()
		return;
	}

	Body->ClearPrediction();

	double Time = 0.;
	FVector TempBodyPosition = Body->GetOrbitalPosition();
	FVector TempBodyVelocity = Body->Velocity;

	Body->AppendPredictionPoint(TempBodyPosition, FixedStep, true);

	for (int32 PointsAdded = 1; PointsAdded < Body->GetMaxPredictPoints(); )
	{
		ComputeStep(TempBodyPosition, TempBodyVelocity, FixedStep, Time);
		if (Body->AppendPredictionPoint(TempBodyPosition, FixedStep))
		{
			PointsAdded += 1;
		}
		Time += FixedStep;
	}
	Body->UpdatePredictionPath();
	Body->GetPredictedData().LastPredictedVelocity = TempBodyVelocity;
	Body->GetPredictedData().LastPredictedSimTime = Time;
}

void AOrbitManager::ToggleObjectsVisibility(bool NewState)
{
	for (UOrbitAttractorBaseComponent*& Attractor : Attractors) // Updated type
	{
		// Components don't have SetActorHiddenInGame. This logic needs to be re-evaluated.
		// Attractor->SetActorHiddenInGame(NewState); 
	}

	for (UOrbitDynamicObjectComponent*& DynObj : DynamicObjects) // Updated type
	{
		// Components don't have SetActorHiddenInGame. This logic needs to be re-evaluated.
		// DynObj->SetActorHiddenInGame(NewState);
	}
}

void AOrbitManager::RegisterObject(UOrbitalBaseComponent* OrbitalComponent) // Updated return type and function name
{
	if (!OrbitalComponent || !OrbitalComponent->GetOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("RegisterDynamicObject: OrbitalComponent or its owner is invalid."));
		return;
	}

	if (UOrbitDynamicObjectComponent* DynCastBuf = Cast<UOrbitDynamicObjectComponent>(OrbitalComponent))
	{
		DynamicObjects.Add(DynCastBuf);
	}
	else if (UOrbitAttractorBaseComponent* AttractorCastBuf = Cast<UOrbitAttractorBaseComponent>(OrbitalComponent))
	{
		Attractors.Add(AttractorCastBuf);
	}
	else
	{
		// default response for logging
		UE_LOG(LogActor, Error, TEXT("Component not registered: %s"), *OrbitalComponent->GetReadableName())
	}
}