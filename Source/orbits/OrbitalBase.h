// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrbitalBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORBITS_API UOrbitalBaseComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	UOrbitalBaseComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	FVector OrbitalPosition;

public:
	class AOrbitManager* Manager = nullptr;
	
	virtual void InitializeComponent() override; // Replaces PostInitializeComponents
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Orbital")
	FVector GetOrbitalPosition() const { return OrbitalPosition; };
	void SetOrbitalPosition(const FVector& NewPos);
	
	// register itself in OrbitalManager, called in Actor's PostInitializeComponents?
	UFUNCTION(BlueprintCallable, Category = "Orbital")
	virtual void OrbitalInit();
};