// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitAttractorBase.h"

// Sets default values
AOrbitAttractorBase::AOrbitAttractorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AOrbitAttractorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AOrbitAttractorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OrbitalPosition = GetActorLocation();
}

// Called every frame
void AOrbitAttractorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AOrbitAttractorBase::GetMassCenterPosition_Implementation(float TimeOffset) const
{
	return OrbitalPosition;
}

float AOrbitAttractorBase::GetBodyGM() const
{
	return GMass;
}
