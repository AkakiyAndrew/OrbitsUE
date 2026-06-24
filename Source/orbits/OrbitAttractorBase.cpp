// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitAttractorBase.h"

#include "OrbitManager.h"

// Sets default values
UOrbitAttractorBaseComponent::UOrbitAttractorBaseComponent()
{
 	// Set this component to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = false;
}

void UOrbitAttractorBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner())
	{
		OrbitalPosition = GetOwner()->GetActorLocation();
	}
}

void UOrbitAttractorBaseComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Manager->RegisterObject(this);
}

FVector UOrbitAttractorBaseComponent::GetMassCenterPosition(double SimTime) const
{
	return OrbitalPosition;
}

float UOrbitAttractorBaseComponent::GetBodyGM() const
{
	return GMass;
}