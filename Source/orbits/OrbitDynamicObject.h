// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitDynamicObject.generated.h"

UCLASS()
class ORBITS_API AOrbitDynamicObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbitDynamicObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	FVector OrbitalPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Velocity;
	UPROPERTY(EditAnywhere)
	bool ShowPredictedOrbit = false;
	UPROPERTY(EditAnywhere)
	FColor PreviewLinesColor = FColor::Black;
	TArray<FVector> PredictedPathPoint; // TODO: remake as TList for quicker removal??
	FVector LastPredictedVelocity;
	double LastPredictedSimTime = 0.;
};
