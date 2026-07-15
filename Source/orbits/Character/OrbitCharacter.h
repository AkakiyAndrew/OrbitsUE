// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "orbits/Orbital/GravityAffected.h"
#include "OrbitCharacter.generated.h"

struct FInputActionValue;
class UOrbitDynamicObjectComponent;
class UInputAction;
class UCameraComponent;
class UInputMappingContext;

UCLASS()
class ORBITS_API AOrbitCharacter : public ACharacter, public IGravityAffected
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOrbitCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit", meta = (AllowPrivateAccess = "true"))
	UOrbitDynamicObjectComponent* OrbitComponent;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	
	/** Follow camera */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;
	
	/** MappingContext for zero gravity*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* MappingContextSpace;
	
	/** MappingContext for walking on surface*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* MappingContextSurface;
	
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// void Jump(const FInputActionValue& Value);
	
	
	// TODO: something to detect & interact with access points/items/etc 
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float ThrustersAcceleration = 10.f;
	
	bool bInGravity = false;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// virtual void MoveInGravity(const FVector& Delta) override;
	virtual void RotateToGravity(const FVector& Direction) override;
	void ToggleInputMode(bool IsLanded);
	// void OnGravityUpdate(bool NewState);
	
	virtual bool TryLand(double Speed) override;
	virtual void Jump() override;
};
