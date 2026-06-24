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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double GMass;

public:	
	UFUNCTION(BlueprintCallable)
	virtual FVector GetMassCenterPosition(double SimTime) const;
	UFUNCTION(BlueprintCallable)
	float GetBodyGM() const;
};