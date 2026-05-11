// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitAttractorKepler.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "OrbitalBlueprintFunctionLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "K2Node_AddComponent.h"


AOrbitAttractorKepler::AOrbitAttractorKepler()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	OrbitingBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbitingBody"));

	OrbitSplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("OrbitSpline"));
	OrbitSplinePath->SetClosedLoop(true);
}

void AOrbitAttractorKepler::OnConstruction(const FTransform& Transform)
{
	UpdateOrbit();
}

TArray<FVector> AOrbitAttractorKepler::CreateOrbitPoints()
{
	TArray<FVector> Result;

	if (!IsValid(ParentObject))
	{
		return Result;
	}

	for (int32 Num = 1; Num < SplineOrbitPointsCount; Num++)
	{
		Result.Add(ParentObject->GetMassCenterPosition(0.f) + UOrbitalBlueprintFunctionLibrary::GetOrbitalPosition(
			static_cast<float>(Num) / SplineOrbitPointsCount,
			SemiMajorAxis,
			Eccentrity,
			Inclination,
			AscendingNode,
			PeriapsisArgument
		));
	}
	return Result;
}

void AOrbitAttractorKepler::UpdateOrbit()
{
	TArray<FVector> OrbitPathPoints = CreateOrbitPoints();
	if (!SplineSectionMesh or !SplineMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spline visuals not provided."));
		return;
	}
	if (OrbitPathPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spline orbital points for preview."));
		return;
	}

	OrbitalPeriod = 2 * UE_DOUBLE_PI * FMath::Sqrt(FMath::Pow(SemiMajorAxis, 3) / (ParentObject->GetBodyGM() + GetBodyGM()));

	OrbitSplinePath->SetSplinePoints(OrbitPathPoints, ESplineCoordinateSpace::World, true);
	UMaterialInstanceDynamic* SplineMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, SplineMaterial);
	SplineMaterialInstance->SetVectorParameterValue(TEXT("Colour"), SplineColor);
	SplineMaterialInstance->SetScalarParameterValue(TEXT("Glow"), SplineGlow);
	SplineMaterialInstance->SetScalarParameterValue(TEXT("BandWidth"), SplineBandWidth);
	SplineMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), SplineOpacity);

	// TODO: repurpose already created components?

	int32 PointCounter = 0;
	for (FVector& PathPoint : OrbitPathPoints)
	{
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);

		// Set the static mesh and mobility
		SplineMesh->SetStaticMesh(SplineSectionMesh);
		SplineMesh->SetMaterial(0, SplineMaterialInstance);
		SplineMesh->SetMobility(EComponentMobility::Static);

		// Register and attach
		SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMesh->RegisterComponent();
		SplineMesh->AttachToComponent(OrbitSplinePath, FAttachmentTransformRules::KeepRelativeTransform);

		// Define Start and End points/tangents
		FVector StartPos, StartTangent, EndPos, EndTangent;
		OrbitSplinePath->GetLocationAndTangentAtSplinePoint(PointCounter, StartPos, StartTangent, ESplineCoordinateSpace::Local);
		OrbitSplinePath->GetLocationAndTangentAtSplinePoint(PointCounter + 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

		SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
		PointCounter += 1;
	}
}

void AOrbitAttractorKepler::UpdateOrbitalPosition(double SimTime)
{
	OrbitalPosition = GetMassCenterPosition_Implementation(SimTime);
	OrbitingBody->SetRelativeLocation(OrbitalPosition);
}

FVector AOrbitAttractorKepler::GetMassCenterPosition_Implementation(double SimTime) const
{
	return OrbitSplinePath->GetLocationAtTime(FMath::Fmod(SimTime, OrbitalPeriod) / OrbitalPeriod, ESplineCoordinateSpace::Local);;
}
