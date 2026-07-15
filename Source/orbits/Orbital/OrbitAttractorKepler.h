// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbitAttractorBase.h" // This will include UOrbitAttractorBaseComponent
#include "OrbitAttractorKepler.generated.h"

class USplineComponent;
class USplineMeshComponent;
class UMaterialInstanceDynamic;

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

	UPROPERTY(EditAnywhere, Category = "ORbit Params")
	AActor* ParentActor = nullptr;
};

USTRUCT(BlueprintType)
struct FOrbitPathVisuals
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	float Glow = 30.f;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	float BandWidth = 0.40f;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	float Opacity = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	FColor Color = FColor::Black;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	UMaterial* Material;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals")
	UStaticMesh* SectionMesh;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORBITS_API UOrbitAttractorKeplerComponent : public UOrbitAttractorBaseComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Orbital")
	FOrbitalParameters OrbitalParameters;
	void CreateOrbitPoints();
	
	UPROPERTY(EditAnywhere, Category = "Orbital", meta = (ClampMin = "3"))
	int32 PathPointsCount = 0;

	UPROPERTY(EditAnywhere, Category = "Orbital")
	FOrbitPathVisuals PathVisuals;
	
private:
	double OrbitalPeriod;
	TObjectPtr<UOrbitAttractorBaseComponent> ParentObjectComponent = nullptr;
	TArray<FVector> PathPoints;
	double LastSimTIme = 0.;
	
public:
	UOrbitAttractorKeplerComponent();
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Orbital")
	void UpdateOrbit();
	void UpdateOrbitalPeriod();
	void UpdateParentHierarchy();

	// To call from Manager's Tick
	void UpdateOrbitalPosition(double SimTime);

	virtual FVector GetMassCenterPosition(double SimTime) const override;
	FVector GetInstantVelocity() const;

	int32 GetSplinePointsCount() const { return PathPointsCount; }
};