// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitalBase.generated.h"

class USplineComponent;
class USplineMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class ORBITS_API AOrbitalBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbitalBase();

protected:
	// Prediction Spline Visuals
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USplineComponent> PredictionSplinePath;

	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	float SplineGlow = 30.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	float SplineBandWidth = 0.40f;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	float SplineOpacity = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	FLinearColor SplineColor;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	UMaterial* SplineMaterial;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	UStaticMesh* SplineSectionMesh;
	
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params", meta = (ClampMin = "0"))
	int32 SplineOrbitPointsCount = 0;

	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshes;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SplineMaterialInstance;
	bool bSplineMeshesSetUp = false;
	void SplineMeshesSetUp();

	// TODO: move OrbitalPosition here

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetSplinePointsCount() const { return SplineOrbitPointsCount; }
};
