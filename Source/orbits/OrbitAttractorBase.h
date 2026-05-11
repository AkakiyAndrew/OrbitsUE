// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitAttractorBase.generated.h"

UCLASS()
class ORBITS_API AOrbitAttractorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbitAttractorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	double GMass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector OrbitalPosition;

	UFUNCTION(BlueprintNativeEvent)
	FVector GetMassCenterPosition(double SimTime) const;
	UFUNCTION(BlueprintCallable)
	float GetBodyGM() const;
};
