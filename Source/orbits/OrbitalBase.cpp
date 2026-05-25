// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitalBase.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

// Sets default values
AOrbitalBase::AOrbitalBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PredictionSplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("OrbitSpline"));
}

// Called when the game starts or when spawned
void AOrbitalBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOrbitalBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SplineMeshesSetUp();
}

// Called every frame
void AOrbitalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOrbitalBase::SplineMeshesSetUp()
{
	// set up mesh material instance
	if(!SplineMaterialInstance)
	{
		SplineMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, SplineVisuals.SplineMaterial);
	}

	if(SplineMaterialInstance)
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
		SplineMesh->AttachToComponent(PredictionSplinePath, FAttachmentTransformRules::KeepRelativeTransform);

		SplineMesh->SetStaticMesh(SplineVisuals.SplineSectionMesh);
		if(SplineMaterialInstance)
		{
			SplineMesh->SetMaterial(0, SplineMaterialInstance);
		}

		SplineMeshes[i] = SplineMesh;
	}

	bSplineMeshesSetUp = true;
}