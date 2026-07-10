// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbitalBase.h"
#include "OrbitAttractorBase.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORBITS_API UOrbitAttractorBaseComponent : public UOrbitalBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrbitAttractorBaseComponent();
	virtual void InitializeComponent() override;
	
	virtual void BeginPlay() override;
	
protected:
	// for orbital sim
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double GMass;
	
	// in Earth's Gs, for 'local' gravity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity", meta=(ClampMin="0"))
	float GravityStrength = 1;
	
	// in orbital units before sim scaling, for 'local' gravity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity", meta=(ClampMin="1"))
	float GravityFieldRadius = 100;
	
public:	
	UFUNCTION(BlueprintCallable)
	virtual FVector GetMassCenterPosition(double SimTime) const;
	UFUNCTION(BlueprintCallable)
	float GetBodyGM() const;
	float GetGravityStrength() const { return GravityStrength; }
	float GetGravityFieldRadius() const { return GravityFieldRadius; }
};