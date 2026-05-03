// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicOrbitsManager.h"
#include "Kismet/GameplayStatics.h"
#include "OrbitDynamicObject.h"
#include "OrbitAttractorBase.h"

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


	for (AOrbitDynamicObject*& Body : DynamicObjects)
	{
		float Time = 0;
		FVector TempBodyPosition = Body->OrbitalPosition;
		FVector TempBodyVelocity = Body->Velocity;
		
		Body->PredictedPathPoint.Add(TempBodyPosition); // TODO: needed or not?

		for (int32 PredictStepCounter = 0; PredictStepCounter < PreviewSteps; PredictStepCounter++)
		{
			ComputeStep(TempBodyPosition, TempBodyVelocity, FixedStep, Time);
			Body->PredictedPathPoint.Add(TempBodyPosition);
			//UE_LOG(LogTemp, Log, TEXT("Predicted Pos: %s"), *TempBodyPosition.ToString());

			Time += FixedStep;
			if (PredictStepCounter == PreviewSteps - 1)
				Body->LastPredictedVelocity = TempBodyVelocity;
		}
	}
	// TODO: make first (big) prediction for each dynamic object
}

// Called every frame
void ADynamicOrbitsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeAccumulator += TimeScale * DeltaTime;

	while (TimeAccumulator > FixedStep)
	{
		Step(FixedStep, TimePassed);
		TimePassed += FixedStep;
		TimeAccumulator -= FixedStep;
	}
}

void ADynamicOrbitsManager::Step(float TimeDelta, float Time)
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

		ComputeStep(Pos, Velocity, TimeDelta, Time);
		//UE_LOG(LogTemp, Log, TEXT("Computed Pos: %s"), *Pos.ToString());
		
		//UE_LOG(LogTemp, Log,
		//	TEXT("Step after, Pos: %s, Velocity: %s"),
		//	*Pos.ToString(),
		//	*Velocity.ToString()
		//);

		Body->OrbitalPosition = Pos;
		Body->SetActorLocation(Pos);
		Body->Velocity = Velocity;

		// remove oldest predicted point and predict new one, from last position
		FVector LastPoint = Body->PredictedPathPoint[PreviewSteps - 1];
		float LastTimePrediction = PreviewSteps * TimeDelta;
		ComputeStep(LastPoint, Body->LastPredictedVelocity, TimeDelta, LastTimePrediction, false);
		Body->PredictedPathPoint.RemoveAt(0);
		Body->PredictedPathPoint.Add(LastPoint);
	}
}

void ADynamicOrbitsManager::ComputeStep(FVector &BodyPosition, FVector &BodyVelocity, float TimeDelta, float Time, bool DoLog)
{
	FVector Acc0 = ComputeAcceleration(BodyPosition, Time, DoLog);
	BodyPosition += BodyVelocity * TimeDelta + 0.5 * Acc0 * TimeDelta * TimeDelta;

	//if (DoLog)
	//{
	//	UE_LOG(LogTemp, Log,
	//		TEXT("ComputeStep Acc0, BodyPosition: %s, Acc0: %s"),
	//		*BodyPosition.ToString(),
	//		*Acc0.ToString()
	//	);
	//}

	FVector Acc1 = ComputeAcceleration(BodyPosition, Time + TimeDelta, DoLog);
	BodyVelocity += 0.5 * (Acc0 + Acc1) * TimeDelta;

	//if (DoLog)
	//{
	//	UE_LOG(LogTemp, Log,
	//		TEXT("ComputeStep Acc1, BodyPosition: %s, BodyVelocity: %s, Acc1: %s"),
	//		*BodyPosition.ToString(),
	//		*BodyVelocity.ToString(),
	//		*Acc1.ToString()
	//	);
	//}
}

FVector ADynamicOrbitsManager::ComputeAcceleration(FVector Position, float Time, bool DoLog)
{
	FVector Acceleration;

	for (AOrbitAttractorBase*& Attractor : Attractors)
	{
		FVector AttractorPosition = Attractor->GetMassCenterPosition(Time);
		FVector VectorToAttractor = AttractorPosition - Position;
		double DistSq = VectorToAttractor.SquaredLength();
		double InvDist3 = 1.0 / (FMath::Sqrt(DistSq) * DistSq);

		Acceleration += VectorToAttractor * (Attractor->GetBodyGM() * InvDist3);
		//if(DoLog)
			//UE_LOG(LogTemp, Log, TEXT("AttractorPosition: %s, Position: %s, Time: %f"), *AttractorPosition.ToString(), *Position.ToString(), Time);
	}
	//if(DoLog)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("Acceleration: %s, Time: %f"), *Acceleration.ToString(), Time);
	//}
	return Acceleration;
}

