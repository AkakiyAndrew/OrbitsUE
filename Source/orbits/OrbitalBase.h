// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitalBase.generated.h"

UCLASS()
class ORBITS_API AOrbitalBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbitalBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> Root;

	FVector OrbitalPosition;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:
	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION(BlueprintCallable, Category = "Orbital")
	FVector GetOrbitalPosition() const { return OrbitalPosition; };
	void SetOrbitalPosition(FVector NewPos) { OrbitalPosition = NewPos; };
};
