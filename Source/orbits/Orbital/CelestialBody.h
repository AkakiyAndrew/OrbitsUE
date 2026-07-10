// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CelestialBody.generated.h"

class USphereComponent;

UCLASS()
class ORBITS_API ACelestialBody : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACelestialBody();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Orbit")
	TObjectPtr<class UOrbitAttractorKeplerComponent> OrbitComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gravity")
	TObjectPtr<USphereComponent> GravityField;

public:
	UOrbitAttractorKeplerComponent* GetOrbitComponent() const { return OrbitComponent; }
	USphereComponent* GetGravityFieldComponent() const { return GravityField; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
