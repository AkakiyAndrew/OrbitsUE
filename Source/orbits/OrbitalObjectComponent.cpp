// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitalObjectComponent.h"

#include "DynamicOrbitsManager.h"
#include "OrbitDynamicObject.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UOrbitalObjectComponent::UOrbitalObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOrbitalObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> ManagerBuf;
	UGameplayStatics::GetAllActorsOfClass(this, ADynamicOrbitsManager::StaticClass(), ManagerBuf);
	if (ManagerBuf.Num() == 1)
	{
		Manager = Cast<ADynamicOrbitsManager>(ManagerBuf[0]);
	}

	OrbitalInit();
}


// Called every frame
void UOrbitalObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UOrbitalObjectComponent::OrbitalInit()
{
	if (!Manager)
	{
		UE_LOG(LogTemp, Warning, TEXT("No OrbitManager assigned."));
		return;
	}
	LinkedOrbital = Manager->RegisterDynamicObject(this);
}

void UOrbitalObjectComponent::AddOrbitalVelocity(const FVector& VelocityDelta) const
{
	LinkedOrbital->AddVelocity(VelocityDelta);
}

