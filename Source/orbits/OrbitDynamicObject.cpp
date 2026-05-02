// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitDynamicObject.h"

// Sets default values
AOrbitDynamicObject::AOrbitDynamicObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOrbitDynamicObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrbitDynamicObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ShowPredictedOrbit)
	{
		int Count = 0;

		for (FVector& Point : PredictedPathPoint)
		{
			if (PredictedPathPoint.IsValidIndex(Count + 1))
			{
				DrawDebugLine(GetWorld(), Point, PredictedPathPoint[Count + 1], PreviewLinesColor, false, 3);
			}
			Count += 1;
		}
	}
}