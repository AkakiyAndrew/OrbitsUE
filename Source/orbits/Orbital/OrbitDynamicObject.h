// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrbitalBase.h"
#include "OrbitDynamicObject.generated.h"

class ACelestialBody;
class IGravityAffected;
class UOrbitalObjectComponent;
class UDynamicOrbitsManagerComponent; // Forward declaration for the component version
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPredictionUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGravityChanged, bool, NewState);

struct FPredictedData
{
	TArray<FVector> PathPoints; // TODO: remake as TList for quicker removal??
	int32 PointsCount = 0;
	FVector LastPredictedPoint;
	FVector LastVisualizedPoint;

	FVector LastPredictedVelocity;
	double LastPredictedSimTime = 0.;
	//double TimeAccumulator = 0.;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORBITS_API UOrbitDynamicObjectComponent : public UOrbitalBaseComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	UOrbitDynamicObjectComponent();
	virtual void InitializeComponent() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

protected:
	// Dynamic Prediction Params
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals | Prediction params")
	double PredictionStep = 0.02;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals | Prediction params")
	double MinimalPredictionDistance = 5.;
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals | Prediction params")
	double MaximalPredictionWaitTime = 2.; // top limit for time accumulator since last prediction update
	UPROPERTY(EditAnywhere, Category = "Orbit Visuals | Prediction params")
	int32 MaxPredictionPoints = 100;

	// niagara component for ribbon
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orbit Visuals")
	// class UNiagaraComponent* PathRibbonComponent;
	
	// TODO: something to detect and process going into Attractor's gravity field

private:
	FPredictedData PredictedData;
	bool bOrbitMovable = true;
	ACelestialBody* GravityAttractor = nullptr;
	FTimerHandle GravityDirectionTimerHandle;
	IGravityAffected* OwnerPtr;
	float Mass = 100.f;
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;
	
	UPROPERTY(BlueprintAssignable, Category = "Orbital")
	FMovementUpdate OnMovementUpdate;
	//UFUNCTION(BlueprintCallable)
	//void TogglePredictPathVisibility(bool Show);
	virtual void OrbitalInit() override;
	bool AppendPredictionPoint(const FVector& NewPoint, double TimeStep, bool Forced = false);
	void UpdateOrbitalMovement(const FVector& NewPosition, const FVector& NewVelocity);
	void UpdatePredictionPath() const;
	void ClearPrediction();
	UFUNCTION(BlueprintCallable, Category = "Orbital")
	void CalculatePrediction();
	UFUNCTION(BlueprintCallable, Category = "Orbital")
	// TODO: take into account bodies gravity zones
	void AddOrbitalVelocity(const FVector& Delta);
	
	
	UFUNCTION(BlueprintCallable, Category = "Orbital")
	FVector GetLastPredictedPoint() const { return PredictedData.LastPredictedPoint; };
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetPredictionPoints() const { return PredictedData.PathPoints; };
	UPROPERTY(BlueprintAssignable, Category = "Orbital")
	FPredictionUpdate OnPredictionUpdate;

	
	FPredictedData& GetPredictedData() { return PredictedData; };
	int32 GetMaxPredictPoints() const { return MaxPredictionPoints; };

	// for DynObject to change "local" position
	// TODO: or rather change velocity? no, inconsistency could break simulation 
	void UpdateActorPosition(const FVector& NewPos) const { GetOwner()->SetActorLocation(NewPos); };
	// TODO: subscribe on owner's HitEvent to update orbital velocity
	
	UFUNCTION()
	void OnEnteringGravityField(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void OnLeavingGravityField(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void DirectInGravity();
	ACelestialBody* GetAttractor() const { return GravityAttractor; };
	FGravityChanged OnGravityChanged;
	void MarkAsMovable() { bOrbitMovable = true; }; // CalculatePrediction(); };
	
	// for custom kinematics
	void AddMass();
	void DecreaseMass();
	float GetMass() { return Mass; };
	
};