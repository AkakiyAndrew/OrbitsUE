// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrbitalObjectComponent.generated.h"


class ADynamicOrbitsManager;
class AOrbitDynamicObject;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ORBITS_API UOrbitalObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOrbitalObjectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY()
	AOrbitDynamicObject* LinkedOrbital;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	ADynamicOrbitsManager* Manager = nullptr;
	// register itself in OrbitalManager
	void OrbitalInit();
	
	// for DynObject to change "local" position
	// TODO: or rather change velocity? no, inconsistency could break simulation 
	void UpdateActorPosition(const FVector& NewPos) const { GetOwner()->SetActorLocation(NewPos); };
	// for "local" forces to change orbital velocity
	// TODO: take into account bodies gravity zones
	void AddOrbitalVelocity(const FVector& VelocityDelta) const;
	// TODO: subscribe on owner's HitEvent to update orbital velocity
};
