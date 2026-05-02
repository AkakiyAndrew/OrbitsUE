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
	DynamicObjects.Append(Buf);
	Buf.Empty();

	UGameplayStatics::GetAllActorsOfClass(this, AOrbitAttractorBase::StaticClass(), Buf);
	Attractors.Append(Buf);
	Buf.Empty();


	for (AOrbitDynamicObject*& Body : DynamicObjects)
	{
		float Time = 0;
		FVector TempBodyPosition = Body->GetActorLocation();
		FVector TempBodyVelocity = Body->Velocity;
		for (int32 PredictStepCounter = 0; PredictStepCounter < PreviewSteps; PredictStepCounter++)
		{
			ComputeStep(TempBodyPosition, TempBodyVelocity, FixedStep, Time);
			Body->PredictedPathPoint.Add(TempBodyPosition);
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

	while (TimeAccumulator >= FixedStep)
	{
		Step(FixedStep, 0.f);

		TimeAccumulator -= FixedStep;
	}
}

void ADynamicOrbitsManager::Step(float TimeDelta, float Time)
{
	for (AOrbitDynamicObject* &Body : DynamicObjects)
	{
		// wrap to func, for predictions
		FVector Pos = Body->GetActorLocation();
		FVector Velocity = Body->Velocity;

		ComputeStep(Pos, Velocity, TimeDelta, Time);
		
		Body->SetActorLocation(Pos);
		Body->Velocity = Velocity;

		// remove oldest predicted point and predict new one, from last position
		FVector LastPoint = Body->PredictedPathPoint[PreviewSteps - 1];
		float LastTimePrediction = PreviewSteps * TimeDelta;
		ComputeStep(LastPoint, Body->LastPredictedVelocity, TimeDelta, LastTimePrediction);
		Body->PredictedPathPoint.RemoveAt(0);
		Body->PredictedPathPoint.Add(LastPoint);
	}
}

void ADynamicOrbitsManager::ComputeStep(FVector &BodyPosition, FVector &BodyVelocity, float TimeDelta, float Time)
{
	FVector Acc0 = ComputeAcceleration(BodyPosition, Time);
	BodyPosition += BodyVelocity * TimeDelta + 0.5 * Acc0 * TimeDelta * TimeDelta;

	FVector Acc1 = ComputeAcceleration(BodyPosition, Time + TimeDelta);
	BodyVelocity += 0.5 * (Acc0 + Acc1) * TimeDelta;
}

FVector ADynamicOrbitsManager::ComputeAcceleration(FVector Position, float Time)
{
	FVector Acceleration;

	for (AOrbitAttractorBase*& Attractor : Attractors)
	{
		Attractor->GetMassCenterPosition(Time);
		FVector VectorToAttractor = Attractor->GetActorLocation() - Position;
		double DistSq = VectorToAttractor.SizeSquared();
		double InvDist3 = 1.0 / (FMath::Sqrt(DistSq) * DistSq);

		Acceleration += VectorToAttractor * (Attractor->GetBodyGM() * InvDist3);
	}

	return Acceleration;
}

