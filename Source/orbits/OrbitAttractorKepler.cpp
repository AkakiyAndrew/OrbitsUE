// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitAttractorKepler.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "OrbitalBlueprintFunctionLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "K2Node_AddComponent.h"


AOrbitAttractorKepler::AOrbitAttractorKepler()
{
	OrbitingBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbitingBody"));

	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("OrbitSpline"));
	SplinePath->SetClosedLoop(true);
}


void AOrbitAttractorKepler::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(OrbitalParameters.ParentObject)
		OrbitalPeriod = 2 * UE_DOUBLE_PI * FMath::Sqrt(
			FMath::Pow(OrbitalParameters.SemiMajorAxis, 3) / (OrbitalParameters.ParentObject->GetBodyGM() + GetBodyGM())
		);

	SplineMeshesSetUp();
}

void AOrbitAttractorKepler::BeginPlay()
{
	Super::BeginPlay();
	
	if (!bSplineMeshesSetUp)
	{
		SplineMeshes.Empty();

		SplineMeshesSetUp();

	}
	UpdateOrbit();
}

//void AOrbitAttractorKepler::OnConstruction(const FTransform& Transform)
//{
//}

TArray<FVector> AOrbitAttractorKepler::CreateOrbitPoints()
{
	TArray<FVector> Result;

	if (!IsValid(OrbitalParameters.ParentObject))
	{
		return Result;
	}

	for (int32 Num = 1; Num <= SplineOrbitPointsCount; Num++)
	{
		Result.Add(OrbitalParameters.ParentObject->GetActorLocation() + UOrbitalBlueprintFunctionLibrary::CalcOrbitalPosition(
			static_cast<float>(Num) / SplineOrbitPointsCount,
			OrbitalParameters.SemiMajorAxis,
			OrbitalParameters.Eccentrity,
			OrbitalParameters.Inclination,
			OrbitalParameters.AscendingNode,
			OrbitalParameters.PeriapsisArgument
		));
	}
	return Result;
}

void AOrbitAttractorKepler::UpdateOrbit()
{
	TArray<FVector> OrbitPathPoints = CreateOrbitPoints();
	if (!SplineVisuals.SplineSectionMesh or !SplineVisuals.SplineMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spline visuals not provided."));
		return;
	}
	if (OrbitPathPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spline orbital points for preview."));
		return;
	}

	// recreate meshes, if different count
	if (SplineMeshes.Num() != SplineOrbitPointsCount)
	{
		SplineMeshesSetUp();
	}

	SetActorLocation(OrbitalParameters.ParentObject->GetMassCenterPosition(0.f));

	SplinePath->SetSplinePoints(OrbitPathPoints, ESplineCoordinateSpace::World, true);

	// repurpose already created components
	for (int32 PointIndex = 0; PointIndex < SplineOrbitPointsCount; PointIndex++)
	{
		if (!SplineMeshes[PointIndex])
			continue;
		
		// Start and End points/tangents
		FVector StartPos, StartTangent, EndPos, EndTangent;
		SplinePath->GetLocationAndTangentAtSplinePoint(PointIndex, StartPos, StartTangent, ESplineCoordinateSpace::Local);
		SplinePath->GetLocationAndTangentAtSplinePoint(PointIndex + 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

		SplineMeshes[PointIndex]->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
	}

	UpdateOrbitalPosition(0.);
}

void AOrbitAttractorKepler::UpdateOrbitalPosition(double SimTime)
{
	OrbitalPosition = GetMassCenterPosition(SimTime);
	OrbitingBody->SetRelativeLocation(OrbitalPosition);
}

FVector AOrbitAttractorKepler::GetMassCenterPosition(double SimTime) const
{
	//UE_LOG(LogTemp, Log, TEXT("MassCenter, FMod: %f, Period: %f, SimTime: %f."), FMath::Fmod(SimTime, OrbitalPeriod), OrbitalPeriod, SimTime);
	return OrbitalParameters.ParentObject->GetActorLocation() + UOrbitalBlueprintFunctionLibrary::CalcOrbitalPosition(
		OrbitalParameters.StartingOffset + (FMath::Fmod(SimTime, OrbitalPeriod) / OrbitalPeriod),
		OrbitalParameters.SemiMajorAxis,
		OrbitalParameters.Eccentrity,
		OrbitalParameters.Inclination,
		OrbitalParameters.AscendingNode,
		OrbitalParameters.PeriapsisArgument
	);
}


void AOrbitAttractorKepler::SplineMeshesSetUp()
{
	// set up mesh material instance
	if (!SplineMaterialInstance)
	{
		SplineMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, SplineVisuals.SplineMaterial);
	}

	if (SplineMaterialInstance)
	{
		SplineMaterialInstance->SetVectorParameterValue(TEXT("Colour"), SplineVisuals.SplineColor);
		SplineMaterialInstance->SetScalarParameterValue(TEXT("Glow"), SplineVisuals.SplineGlow);
		SplineMaterialInstance->SetScalarParameterValue(TEXT("BandWidth"), SplineVisuals.SplineBandWidth);
		SplineMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), SplineVisuals.SplineOpacity);
	}


	// TODO: optimize to not delete & re-create components (check num before & after)
	for (USplineMeshComponent* MeshComp : SplineMeshes)
	{
		if (MeshComp)
		{
			MeshComp->DestroyComponent();
		}
	}

	SplineMeshes.SetNum(SplineOrbitPointsCount);

	for (int32 i = 0; i < SplineOrbitPointsCount; i++)
	{
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->SetMobility(EComponentMobility::Static);

		// Register and attach
		SplineMesh->CreationMethod = EComponentCreationMethod::Native;
		SplineMesh->RegisterComponent();
		SplineMesh->AttachToComponent(SplinePath, FAttachmentTransformRules::KeepRelativeTransform);

		SplineMesh->SetStaticMesh(SplineVisuals.SplineSectionMesh);
		if (SplineMaterialInstance)
		{
			SplineMesh->SetMaterial(0, SplineMaterialInstance);
		}

		SplineMeshes[i] = SplineMesh;
	}

	bSplineMeshesSetUp = true;
}