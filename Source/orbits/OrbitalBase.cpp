// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitalBase.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

// Sets default values
AOrbitalBase::AOrbitalBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

// Called when the game starts or when spawned
void AOrbitalBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOrbitalBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	OrbitalPosition = GetActorLocation();
}

// Called every frame
void AOrbitalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
