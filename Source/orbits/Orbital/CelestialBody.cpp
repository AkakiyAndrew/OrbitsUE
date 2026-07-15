// Fill out your copyright notice in the Description page of Project Settings.


#include "CelestialBody.h"

#include "OrbitAttractorKepler.h"
#include "Components/SphereComponent.h"


// Sets default values
ACelestialBody::ACelestialBody()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	
	OrbitComponent = CreateDefaultSubobject<UOrbitAttractorKeplerComponent>(TEXT("OrbitComponent"));
	GravityField = CreateDefaultSubobject<USphereComponent>(TEXT("GravityField"));
	GravityField->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACelestialBody::BeginPlay()
{
	Super::BeginPlay();
}

void ACelestialBody::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	GravityField->SetSphereRadius(OrbitComponent->GetGravityFieldRadius());
}

// Called every frame
void ACelestialBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

