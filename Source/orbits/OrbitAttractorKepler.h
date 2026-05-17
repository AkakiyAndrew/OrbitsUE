// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbitAttractorBase.h"
#include "OrbitAttractorKepler.generated.h"

class USplineComponent;
class USplineMeshComponent;

UCLASS()
class ORBITS_API AOrbitAttractorKepler : public AOrbitAttractorBase
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> OrbitingBody;
	UPROPERTY()
	TObjectPtr<USplineComponent> OrbitSplinePath;

	// Orbital
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params")
	float Eccentrity = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params")
	float StartingOffset = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params")
	float Inclination = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params")
	float AscendingNode = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params")
	float PeriapsisArgument = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params")
	float SemiMajorAxis = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params")
	TObjectPtr<AOrbitAttractorBase> ParentObject = nullptr;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params")
	int32 SplineOrbitPointsCount = 0;

	TArray<FVector> CreateOrbitPoints();

	// Spline Visuals
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	float SplineGlow = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	float SplineBandWidth = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	float SplineOpacity = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	FLinearColor SplineColor;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	UMaterial* SplineMaterial;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	UStaticMesh* SplineSectionMesh;


private:
	// TODO: calc this somehow, from SemiMajorAxis??
	double OrbitalPeriod;

	bool bSplineMeshesSetUp = false;

	void SplineMeshesSetUp();

	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshes;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:
	AOrbitAttractorKepler();
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Orbital")
	void UpdateOrbit();

	// To call from Manager's Tick
	void UpdateOrbitalPosition(double SimTime);

	virtual FVector GetMassCenterPosition(double SimTime) const override;
};
