// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitAttractorKepler.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "OrbitalBlueprintFunctionLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "K2Node_AddComponent.h" // This include might not be needed anymore, but keeping for now.


UOrbitAttractorKeplerComponent::UOrbitAttractorKeplerComponent()
{
	PrimaryComponentTick.bCanEverTick = true; // Components don't tick by default unless specified
}

void UOrbitAttractorKeplerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	for (int32 PointIndex = 0; PointIndex <= PathPointsCount; PointIndex++)
	{
		if (PointIndex == PathPointsCount)
		{
			// from last to first
			DrawDebugLine(GetWorld(), PathPoints[0], PathPoints[PointIndex], PathVisuals.Color, false, 0);
		}
		else
		{
			DrawDebugLine(GetWorld(), PathPoints[PointIndex], PathPoints[PointIndex+1], PathVisuals.Color, false, 0);
		}
	}
}


void UOrbitAttractorKeplerComponent::InitializeComponent()
{
	Super::InitializeComponent(); // Changed from PostInitializeComponents
	UpdateOrbitalPeriod();
}

void UOrbitAttractorKeplerComponent::CreateOrbitPoints()
{
	if (!IsValid(OrbitalParameters.ParentObject))
	{
		return;
	}

	PathPoints.Empty();
	
	for (int32 Num = 1; Num <= PathPointsCount; Num++)
	{
		PathPoints.Add(OrbitalParameters.ParentObject->GetMassCenterPosition(0.) + UOrbitalBlueprintFunctionLibrary::CalcOrbitalPosition(
			static_cast<float>(Num) / PathPointsCount,
			OrbitalParameters.SemiMajorAxis,
			OrbitalParameters.Eccentrity,
			OrbitalParameters.Inclination,
			OrbitalParameters.AscendingNode,
			OrbitalParameters.PeriapsisArgument
		));
	}
}

void UOrbitAttractorKeplerComponent::UpdateOrbit()
{
	UpdateOrbitalPeriod();
	CreateOrbitPoints();
	if (PathPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spline orbital points for preview."));
		return;
	}
	
	if (GetOwner()) // null check for owner
	{
		UpdateOrbitalPosition(0.);
	}
	
#if WITH_EDITOR
	for (int32 PointIndex = 0; PointIndex <= PathPointsCount; PointIndex++)
	{
		DrawDebugSphere(GetWorld(), PathPoints[PointIndex], 10, 4, FColor::White, false, 3);
	}
	
#endif
	
}

void UOrbitAttractorKeplerComponent::UpdateOrbitalPeriod()
{
	if (OrbitalParameters.ParentObject)
		OrbitalPeriod = 2 * UE_DOUBLE_PI * FMath::Sqrt(
			FMath::Pow(OrbitalParameters.SemiMajorAxis, 3) / (OrbitalParameters.ParentObject->GetBodyGM() + GetBodyGM())
		);
}

void UOrbitAttractorKeplerComponent::UpdateOrbitalPosition(double SimTime)
{
	SetOrbitalPosition(GetMassCenterPosition(SimTime));
}

FVector UOrbitAttractorKeplerComponent::GetMassCenterPosition(double SimTime) const
{
	//UE_LOG(LogTemp, Log, TEXT("MassCenter, FMod: %f, Period: %f, SimTime: %f."), FMath::Fmod(SimTime, OrbitalPeriod), OrbitalPeriod, SimTime);
	return OrbitalParameters.ParentObject->GetMassCenterPosition(SimTime) + UOrbitalBlueprintFunctionLibrary::CalcOrbitalPosition(
		OrbitalParameters.StartingOffset + (FMath::Fmod(SimTime, OrbitalPeriod) / OrbitalPeriod),
		OrbitalParameters.SemiMajorAxis,
		OrbitalParameters.Eccentrity,
		OrbitalParameters.Inclination,
		OrbitalParameters.AscendingNode,
		OrbitalParameters.PeriapsisArgument
	);
}
