// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbitAttractorBase.h"
#include "OrbitAttractorKepler.generated.h"

USTRUCT(BlueprintType)
struct FOrbitalParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Orbit Params", meta = (ClampMin = "0", ClampMax = "1"))
	float Eccentrity = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Params", meta = (ClampMin = "0", ClampMax = "1"))
	float StartingOffset = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Params", meta = (ClampMin = "0", ClampMax = "180"))
	float Inclination = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Params", meta = (ClampMin = "0", ClampMax = "360"))
	float AscendingNode = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Params", meta = (ClampMin = "0", ClampMax = "180"))
	float PeriapsisArgument = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Params", meta = (ClampMin = "1"))
	float SemiMajorAxis = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Orbit Params")
	TObjectPtr<AOrbitAttractorBase> ParentObject = nullptr;
};

USTRUCT(BlueprintType)
struct FSplineVisuals
{
	GENERATED_BODY()

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
};

class USplineComponent;
class USplineMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class ORBITS_API AOrbitAttractorKepler : public AOrbitAttractorBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> OrbitingBody;
	
	// Orbital
	UPROPERTY(EditAnywhere, Category = "Orbital")
	FOrbitalParameters OrbitalParameters;
	TArray<FVector> CreateOrbitPoints();
	
	// Spline Visuals
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USplineComponent> SplinePath;

	UPROPERTY(EditAnywhere)
	FSplineVisuals SplineVisuals;
	UPROPERTY(EditAnywhere, Category = "Orbital", meta = (ClampMin = "0"))
	int32 SplineOrbitPointsCount = 0;

	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshes;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SplineMaterialInstance;
	bool bSplineMeshesSetUp = false;
	void SplineMeshesSetUp();

private:
	double OrbitalPeriod;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:
	AOrbitAttractorKepler();
	//virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Orbital")
	void UpdateOrbit();
	void UpdateOrbitalPeriod();

	// To call from Manager's Tick
	void UpdateOrbitalPosition(double SimTime);

	virtual FVector GetMassCenterPosition(double SimTime) const override;

	int32 GetSplinePointsCount() const { return SplineOrbitPointsCount; }
};
