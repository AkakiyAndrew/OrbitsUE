// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicOrbitsManager.h"
#include "Kismet/GameplayStatics.h"
#include "OrbitDynamicObject.h"
#include "OrbitAttractorBase.h"
#include "OrbitAttractorKepler.h"

// Sets default values
ADynamicOrbitsManager::ADynamicOrbitsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADynamicOrbitsManager::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> Buf;
	UGameplayStatics::GetAllActorsOfClass(this, AOrbitDynamicObject::StaticClass(), Buf);
	for (AActor*& Actor : Buf)
	{
		DynamicObjects.Add(Cast<AOrbitDynamicObject>(Actor));
	}
	Buf.Empty();

	UGameplayStatics::GetAllActorsOfClass(this, AOrbitAttractorBase::StaticClass(), Buf);
	for (AActor*& Actor : Buf)
	{
		Attractors.Add(Cast<AOrbitAttractorBase>(Actor));
	}
	Buf.Empty();

	// first (big) prediction for each dynamic object
	// TODO: wrap in func for preduction recalc for each body separetly (here & after movement)
	for (AOrbitDynamicObject*& Body : DynamicObjects)
	{
		double Time = 0.;
		FVector TempBodyPosition = Body->OrbitalPosition;
		FVector TempBodyVelocity = Body->Velocity;
		
		Body->AppendPredictionPoint(TempBodyPosition, FixedStep, true);
		
		//Body->PredictedPathPoint.Add(TempBodyPosition); // TODO: needed or not?

		for (int32 PointsAdded = 1; Body->GetCurrentPredictionPointCount() < Body->GetSplinePointsCount(); )
		{
			ComputeStep(TempBodyPosition, TempBodyVelocity, FixedStep, Time);
			if (Body->AppendPredictionPoint(TempBodyPosition, FixedStep))
			{
				PointsAdded += 1;
			}
			//Body->PredictedPathPoint.Add(TempBodyPosition);
			Time += FixedStep;
			//UE_LOG(LogTemp, Log, TEXT("Predicted Pos: %s"), *TempBodyPosition.ToString());
		}
		Body->UpdatePredictionSpline();
		Body->LastPredictedVelocity = TempBodyVelocity;
		Body->LastPredictedSimTime = Time;
	}
}

// Called every frame
void ADynamicOrbitsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeAccumulator += TimeScale * DeltaTime;

	while (TimeAccumulator > FixedStep)
	{
		Step(FixedStep, TimePassed);
		
		for (AOrbitAttractorBase*& Attractor: Attractors)
		{
			AOrbitAttractorKepler* OrbitingAttractor = Cast<AOrbitAttractorKepler>(Attractor);
			if (OrbitingAttractor)
			{
				OrbitingAttractor->UpdateOrbitalPosition(TimePassed);
			}
		}
		
		TimePassed += FixedStep;
		TimeAccumulator -= FixedStep;
	}
}

void ADynamicOrbitsManager::Step(double TimeDelta, double Time)
{
	for (AOrbitDynamicObject* &Body : DynamicObjects)
	{
		// wrap to func, for predictions
		FVector Pos = Body->OrbitalPosition;
		FVector Velocity = Body->Velocity;

		//UE_LOG(LogTemp, Log, 
		//	TEXT("Step before, Pos: %s, Velocity: %s, Time: %f"), 
		//	*Pos.ToString(),
		//	*Velocity.ToString(),
		//	Time
		//);

		ComputeStep(Pos, Velocity, FixedStep, Time);
		//UE_LOG(LogTemp, Log, TEXT("Computed Pos: %s"), *Pos.ToString());
		
		//UE_LOG(LogTemp, Log,
		//	TEXT("Step after, Pos: %s, Velocity: %s"),
		//	*Pos.ToString(),
		//	*Velocity.ToString()
		//);

		Body->OrbitalPosition = Pos;
		Body->SetActorLocation(Pos);
		Body->Velocity = Velocity;
		//DrawDebugSphere(GetWorld(), Pos, 100, 10, FColor::Blue, false, 0.3);

		// remove oldest predicted point and predict new one, from last position
		FVector LastPoint = Body->GetLastPredictedPoint();
		double LastTimePrediction = Body->LastPredictedSimTime; // TODO: wrong prediction time (save in body?)
		ComputeStep(LastPoint, Body->LastPredictedVelocity, FixedStep, LastTimePrediction, false);
		//Body->PredictedPathPoint.RemoveAt(0);
		//Body->PredictedPathPoint.Add(LastPoint);
		Body->AppendPredictionPoint(LastPoint, FixedStep);
		Body->UpdatePredictionSpline();
		Body->LastPredictedSimTime = LastTimePrediction + FixedStep;
	}
}

void ADynamicOrbitsManager::ComputeStep(FVector &BodyPosition, FVector &BodyVelocity, double TimeDelta, double Time, bool DoLog)
{
	FVector Acc0 = ComputeAcceleration(BodyPosition, Time, DoLog);
	BodyPosition += BodyVelocity * FixedStep + 0.5 * Acc0 * FixedStep * FixedStep;

	if (DoLog)
	{
		UE_LOG(LogTemp, Log,
			TEXT("ComputeStep Acc0, BodyPosition: %s, Acc0: %s, Time: %f"),
			*BodyPosition.ToString(),
			*Acc0.ToString(),
			Time
		);
	}

	FVector Acc1 = ComputeAcceleration(BodyPosition, Time + FixedStep, DoLog);
	BodyVelocity += 0.5 * (Acc0 + Acc1) * FixedStep;

	if (DoLog)
	{
		UE_LOG(LogTemp, Log,
			TEXT("ComputeStep Acc1, BodyPosition: %s, BodyVelocity: %s, Acc1: %s, Time: %f"),
			*BodyPosition.ToString(),
			*BodyVelocity.ToString(),
			*Acc1.ToString(),
			Time
		);
	}
}

FVector ADynamicOrbitsManager::ComputeAcceleration(FVector Position, double Time, bool DoLog)
{
	FVector Acceleration = FVector();

	for (AOrbitAttractorBase*& Attractor : Attractors)
	{
		FVector AttractorPosition = Attractor->GetMassCenterPosition(Time);
		if(!DoLog)
			DrawDebugSphere(GetWorld(), AttractorPosition, 30, 10, FColor::Red);
		FVector VectorToAttractor = AttractorPosition - Position;
		double DistSq = VectorToAttractor.SquaredLength();
		double InvDist3 = 1.0 / (FMath::Sqrt(DistSq) * DistSq);

		Acceleration += VectorToAttractor * (Attractor->GetBodyGM() * InvDist3);
		if(DoLog)
			UE_LOG(LogTemp, Log, 
				TEXT("AttractorPosition: %s, VectorToAttractor: %s, DistSq: %f, InvDist3: %f, GM: %f, Acceleration: %s, Position: %s, Time: %f"), 
				*AttractorPosition.ToString(),
				*VectorToAttractor.ToString(),
				DistSq,
				InvDist3,
				Attractor->GetBodyGM(),
				*Acceleration.ToString(),
				*Position.ToString(), 
				Time
			);
	}
	//if(DoLog)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("Acceleration: %s, Time: %f"), *Acceleration.ToString(), Time);
	//}
	return Acceleration;
}

