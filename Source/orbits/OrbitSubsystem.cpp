// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitSubsystem.h"

#include "OrbitManager.h"
#include "Kismet/GameplayStatics.h"

void UOrbitSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	TArray<AActor*> Buf;
	UGameplayStatics::GetAllActorsOfClass(this, AOrbitManager::StaticClass(), Buf);
	if (Buf.Num() == 1)
	{
		Manager = Cast<AOrbitManager>(Buf[0]);
	}
	else
	{
		UE_LOG(LogActor, Error, TEXT("Wrong Manager's count."));
	}
}
