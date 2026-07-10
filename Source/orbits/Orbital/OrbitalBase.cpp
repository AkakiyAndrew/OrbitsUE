// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitalBase.h"

#include "OrbitManager.h"
#include "OrbitSubsystem.h"

// Sets default values
UOrbitalBaseComponent::UOrbitalBaseComponent()
{
 	// Set this component to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

// Called when the game starts
void UOrbitalBaseComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (Manager)
	{
		Manager->RegisterObject(this);
	}
}

void UOrbitalBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	OrbitalInit();
}

// Called every frame
void UOrbitalBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UOrbitalBaseComponent::SetOrbitalPosition(const FVector& NewPos)
{
	OrbitalPosition = NewPos;
	
	if (GetOwner()) // Added null check for owner
	{
		GetOwner()->SetActorLocation(OrbitalPosition);
	}
}

void UOrbitalBaseComponent::OrbitalInit()
{
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		Manager = GameInstance->GetSubsystem<UOrbitSubsystem>()->GetOrbitManager();
		if (!Manager)
		{
			UE_LOG(LogTemp, Warning, TEXT("No OrbitManager assigned or found."));
			return;
		}
	}
	
	if (GetOwner())
	{
		OrbitalPosition = GetOwner()->GetActorLocation();
	};
}
