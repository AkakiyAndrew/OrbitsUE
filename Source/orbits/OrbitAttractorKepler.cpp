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


void AOrbitAttractorKepler::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AOrbitAttractorKepler::BeginPlay()
{
	Super::BeginPlay();
	OrbitalPeriod = 2 * UE_DOUBLE_PI * FMath::Sqrt(FMath::Pow(SemiMajorAxis, 3) / (ParentObject->GetBodyGM() + GetBodyGM()));
}

void AOrbitAttractorKepler::OnConstruction(const FTransform& Transform)
{
	if (!bSplineMeshesSetUp)
	{
		SplineMeshes.Empty();

		SplineMeshesSetUp();

	}

	UpdateOrbit();
}

void AOrbitAttractorKepler::SplineMeshesSetUp()
{
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
		SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMesh->RegisterComponent();
		SplineMesh->AttachToComponent(OrbitSplinePath, FAttachmentTransformRules::KeepRelativeTransform);

		SplineMeshes[i] = SplineMesh;
	}

	bSplineMeshesSetUp = true;
}

TArray<FVector> AOrbitAttractorKepler::CreateOrbitPoints()
{
	TArray<FVector> Result;

	if (!IsValid(ParentObject))
	{
		return Result;
	}

	for (int32 Num = 1; Num <= SplineOrbitPointsCount; Num++)
	{
		Result.Add(ParentObject->GetActorLocation() + UOrbitalBlueprintFunctionLibrary::GetOrbitalPosition(
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

	// recreate meshes, if different count
	if (SplineMeshes.Num() != SplineOrbitPointsCount)
	{
		SplineMeshesSetUp();
	}


	SetActorLocation(ParentObject->GetMassCenterPosition(0.f));
	OrbitSplinePath->SetSplinePoints(OrbitPathPoints, ESplineCoordinateSpace::World, true);
	UMaterialInstanceDynamic* SplineMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, SplineMaterial);
	SplineMaterialInstance->SetVectorParameterValue(TEXT("Colour"), SplineColor);
	SplineMaterialInstance->SetScalarParameterValue(TEXT("Glow"), SplineGlow);
	SplineMaterialInstance->SetScalarParameterValue(TEXT("BandWidth"), SplineBandWidth);
	SplineMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), SplineOpacity);

	// TODO: repurpose already created components?

	int32 PointCounter = 0;
	for (int32 PointIndex = 0; PointIndex < SplineOrbitPointsCount; PointIndex++)
	{
		// Set the static mesh and mobility
		SplineMeshes[PointIndex]->SetStaticMesh(SplineSectionMesh);
		SplineMeshes[PointIndex]->SetMaterial(0, SplineMaterialInstance);

		// Define Start and End points/tangents
		FVector StartPos, StartTangent, EndPos, EndTangent;
		OrbitSplinePath->GetLocationAndTangentAtSplinePoint(PointCounter, StartPos, StartTangent, ESplineCoordinateSpace::Local);
		OrbitSplinePath->GetLocationAndTangentAtSplinePoint(PointCounter + 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

		SplineMeshes[PointIndex]->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
		PointCounter += 1;
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
	return OrbitSplinePath->GetLocationAtTime(FMath::Fmod(SimTime, OrbitalPeriod) / OrbitalPeriod, ESplineCoordinateSpace::Local);;
}
