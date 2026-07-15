// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitDynamicObject.h"

#include <dxcore_interface.h>

#include "CelestialBody.h"
#include "GravityAffected.h"
#include "OrbitAttractorKepler.h"
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
		if (!GetOwner()->Implements<UGravityAffected>())
		{
			UE_LOG(LogTemp, Error, TEXT("DynObject Owner doesn't implements IGravityAffected: %s."), *GetOwner()->GetName())
		}
		OwnerPtr = Cast<IGravityAffected>(GetOwner());
				
		// subscribe to check overlaps with CelestialBodies
		// TODO: make check on game start somehow
		GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UOrbitDynamicObjectComponent::OnEnteringGravityField);
		GetOwner()->OnActorEndOverlap.AddDynamic(this, &UOrbitDynamicObjectComponent::OnLeavingGravityField);
	}
}

// Called every frame
void UOrbitDynamicObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	for (int32 PointIndex = 1; PointIndex < PredictedData.PointsCount; PointIndex++)
	{
		DrawDebugLine(GetWorld(), PredictedData.PathPoints[PointIndex], PredictedData.PathPoints[PointIndex-1], FColor::Purple, false, 0);
		DrawDebugSphere(GetWorld(), PredictedData.PathPoints[PointIndex], 10, 4, FColor::White, false, 0);
	}
	
	DrawDebugSphere(GetWorld(), OrbitalPosition, 100, 4, FColor::Red, false, 0);
}

void UOrbitDynamicObjectComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner())
	{
		// one-time check at game start
		TArray<AActor*> Overlapping;
		GetOwner()->GetOverlappingActors(Overlapping, ACelestialBody::StaticClass());
		if (Overlapping.Num() == 1)
		{
			OnEnteringGravityField(GetOwner(), Overlapping[0]);
		}
	}
	CalculatePrediction(); // request from manager after all Attractors are registered
}

void UOrbitDynamicObjectComponent::OrbitalInit()
{
	Super::OrbitalInit();
}

bool UOrbitDynamicObjectComponent::AppendPredictionPoint(const FVector& NewPoint, const double TimeStep, const bool Forced)
{
	if (!bOrbitMovable)
	{
		return true;
	}
	
	// TODO: add argument of distance to the closest attractor, if pass min threshold then add point
	// TODO: cull points that left behind (make array of SimTime for points?)
	bool Result;
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
	}

	if(DistanceToLast > MinimalPredictionDistance
		|| Forced)
	{
		PredictedData.PathPoints.Add(NewPoint);
		PredictedData.PointsCount += 1;

		PredictedData.LastVisualizedPoint = NewPoint;
		//PredictedData.PredictionTimeAccumulator = 0.;
		Result = true;
	}
	else
	{
		Result = false;
	}

	PredictedData.LastPredictedPoint = NewPoint;
	PredictedData.LastPredictedSimTime += TimeStep;
	//PredictionTimeAccumulator += TimeStep;
	return Result;
}

void UOrbitDynamicObjectComponent::UpdateOrbitalMovement(const FVector& NewPosition, const FVector& NewVelocity)
{
	if (!GetOwner() || !bOrbitMovable)
	{
		return;
	}
	
	//SetOrbitalPosition(NewPosition);
	OrbitalPosition = NewPosition;
	Velocity = NewVelocity;

	FHitResult Hit;
	GetOwner()->SetActorLocation(NewPosition, true, &Hit);
	
	if (Hit.bBlockingHit)
	{
		if (Cast<ACelestialBody>(Hit.GetActor()))
		{
			// bounce off celestial body with velocity reduction
			Velocity = Velocity.MirrorByVector(Hit.Normal) * 0.5;
			
			// try to land object on surface
			if (OwnerPtr->TryLand(Velocity.Length()))
			{
				bOrbitMovable = false;
				ClearPrediction();
				UE_LOG(LogTemp, Log, TEXT("Landed: %s"), *GetOwner()->GetName());
			}
			// TODO: if landed, mark as non-orbit-updateable 
			// TODO: for non-characters, how to mark them back? check on another hit?
			// TODO: yes, mark it when hitted by another non-marked object
		}
		else if (UOrbitDynamicObjectComponent* AnotherComponent = Cast<UOrbitDynamicObjectComponent>(Hit.GetActor()->GetComponentByClass(StaticClass())))
		{
			// push back if it's another Dynamic
			FVector RelativeVelocity = Velocity - AnotherComponent->Velocity;

			float ClosingSpeed = RelativeVelocity.Dot(Hit.Normal);
			float Elasticy = 0.5; 
			
			float Impulse = -(1 + Elasticy) * ClosingSpeed / (1/Mass + 1/AnotherComponent->GetMass());

			AddOrbitalVelocity(Impulse * Hit.Normal / Mass);
			AnotherComponent->MarkAsMovable();
			AnotherComponent->AddOrbitalVelocity(-1 * Impulse * Hit.Normal / AnotherComponent->GetMass());
		}
		
		UE_LOG(LogTemp, Log, TEXT("Hit."));
		CalculatePrediction();
		// TODO: rotate Owner by where it was hitted
	}
	
	OnMovementUpdate.Broadcast();
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
	
	if (Velocity.IsZero())
	{
		UE_LOG(LogTemp, Log, TEXT("DynObject not moving, no prediction available."));
		return;
	}
	
	if (!bOrbitMovable)
	{
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Requesting prediction, pos: %s, vel: %s."), *OrbitalPosition.ToString(), *Velocity.ToString());
	Manager->CalculateDynBodyPrediction(this);
	UE_LOG(LogTemp, Log, TEXT("Got prediction, first point: %s."), *PredictedData.PathPoints[0].ToString());
	OnPredictionUpdate.Broadcast();
}

void UOrbitDynamicObjectComponent::AddOrbitalVelocity(const FVector& VelocityDelta)
{
	Velocity += VelocityDelta;
	CalculatePrediction();
}

void UOrbitDynamicObjectComponent::OnEnteringGravityField(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ACelestialBody* Body = Cast<ACelestialBody>(OtherActor))
	{
		GravityAttractor = Body;
		GetWorld()->GetTimerManager().SetTimer(GravityDirectionTimerHandle, this, &UOrbitDynamicObjectComponent::DirectInGravity, 0.2f, true);
		// ClearPrediction();
		// TODO: for character, make OnEnteringGravity delegate, so its SkelMesh began to "fall" (animation variable)
		OnGravityChanged.Broadcast(false);
		
		UE_LOG(LogTemp, Log, TEXT("Entering grav field."));
		DrawDebugSphere(GetWorld(), OrbitalPosition, 100, 4, FColor::Green, false, 2);
	}
}

void UOrbitDynamicObjectComponent::OnLeavingGravityField(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != GravityAttractor)
	{
		return;
	}
	
	// TODO: calculate from kepler attractor its velocity at given SimTime
	// TODO: move into Jump() for Character
	//UpdateOrbitalMovement(GetOwner()->GetActorLocation(), GravityAttractor->GetOrbitComponent()->GetInstantVelocity());
	
	GravityAttractor = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(GravityDirectionTimerHandle);
	
	// TODO: call Character's switch to zerogravity controls (input mapping)
	
	CalculatePrediction();
	OnGravityChanged.Broadcast(false);
	
	UE_LOG(LogTemp, Log, TEXT("Leaving grav field."));
	DrawDebugSphere(GetWorld(), OrbitalPosition, 100, 4, FColor::Orange, false, 2);
}

void UOrbitDynamicObjectComponent::DirectInGravity()
{
	FVector AttractorPos = GravityAttractor->GetOrbitComponent()->GetOrbitalPosition();
	FVector CurrentPos = GetOwner()->GetActorLocation();
	FVector Dir = (AttractorPos - CurrentPos).GetSafeNormal();
	
	//float GravityStrength = (AttractorPos - CurrentPos).Length() / GravityAttractor->GetOrbitComponent()->GetGravityFieldRadius() * GravityAttractor->GetOrbitComponent()->GetGravityStrength();
	
	// add to Velocity instead?
	//OwnerPtr->MoveInGravity(Dir * GravityStrength);
	OwnerPtr->RotateToGravity(Dir);
	UE_LOG(LogTemp, Log, TEXT("Directing in gravity: %s"), *Dir.ToString());
	DrawDebugDirectionalArrow(GetWorld(), OrbitalPosition, OrbitalPosition + Dir*200, 5, FColor::Blue, false, 0.5);
}
