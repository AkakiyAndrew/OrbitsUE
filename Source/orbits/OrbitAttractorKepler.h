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

	// Orbital
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params", meta = (ClampMin = "0", ClampMax = "1"))
	float Eccentrity = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params", meta = (ClampMin = "0", ClampMax = "1"))
	float StartingOffset = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params", meta = (ClampMin = "0", ClampMax = "180"))
	float Inclination = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params", meta = (ClampMin = "0", ClampMax = "360"))
	float AscendingNode = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params", meta = (ClampMin = "0", ClampMax = "180"))
	float PeriapsisArgument = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params", meta = (ClampMin = "1"))
	float SemiMajorAxis = 0.f;
	UPROPERTY(EditAnywhere, Category = "Orbital|Orbit Params")
	TObjectPtr<AOrbitAttractorBase> ParentObject = nullptr;

	TArray<FVector> CreateOrbitPoints();

private:
	// TODO: calc this somehow, from SemiMajorAxis??
	double OrbitalPeriod;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:
	AOrbitAttractorKepler();
	//virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Orbital")
	void UpdateOrbit();

	// To call from Manager's Tick
	void UpdateOrbitalPosition(double SimTime);

	virtual FVector GetMassCenterPosition(double SimTime) const override;
};
