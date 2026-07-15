// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitCharacter.h"

#include "orbits/Orbital/OrbitDynamicObject.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FreeCameraManager.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "orbits/Orbital/CelestialBody.h"
#include "orbits/Orbital/OrbitAttractorKepler.h"

// Sets default values
AOrbitCharacter::AOrbitCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	OrbitComponent = CreateDefaultSubobject<UOrbitDynamicObjectComponent>(TEXT("OrbitComponent"));
	GetCapsuleComponent()->SetEnableGravity(false);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true; // Camera rotates based on the Pawn's Control Rotation

	// Disable these so the Controller doesn't force the Actor back to World-Up alignment
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	
}

// Called when the game starts or when spawned
void AOrbitCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Add Input Mapping Context
	ToggleInputMode(bInGravity);
}

// Called every frame
void AOrbitCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AOrbitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AOrbitCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector MovementVector = Value.Get<FVector>();

	if (Controller != nullptr)
	{
		if (bInGravity)
		{
			// on-surface movement
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
		else
		{	
			// zero-gravity movement	
			FRotationMatrix Rotation(Controller->GetControlRotation());
			FVector Direction = (
				Rotation.GetUnitAxis(EAxis::X) * MovementVector.X 
				+ Rotation.GetUnitAxis(EAxis::Y) * MovementVector.Y 
				+ Rotation.GetUnitAxis(EAxis::Z) * MovementVector.Z
				);
			Direction.Normalize();
			FVector ThrustVector = ThrustersAcceleration * Direction;
			
			OrbitComponent->AddOrbitalVelocity(ThrustVector);
		}
	}
}

void AOrbitCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector LookAxisVector = Value.Get<FVector>();

	if (Controller != nullptr)
	{
		if (bInGravity)
		{
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
		else
		{
			// Create a delta rotation from the input. 
			// FRotator params are (Pitch, Yaw, Roll). 
			// Note: You may need to negate Y (Pitch) depending on your Input Mapping Context settings.
			FRotator DeltaRotation(LookAxisVector.Y, LookAxisVector.X, LookAxisVector.Z);

			// TODO: can use this place to clamp / smooth rotation speed
			// Apply the rotation relative to the Actor's current local transform
			AddActorLocalRotation(DeltaRotation);

			// Update the Controller rotation to match the Actor so the Camera remains synced
			Controller->SetControlRotation(GetActorRotation());
		}
	}
}

// void AOrbitCharacter::MoveInGravity(const FVector& Delta)
// {
// 	GetCapsuleComponent()->AddForce(Delta, NAME_None, true);
// }

void AOrbitCharacter::RotateToGravity(const FVector& Direction)
{
	GetCharacterMovement()->SetGravityDirection(Direction);
	// rotate character "upward" relatively to current local gravity direction
	FQuat Align = FQuat::FindBetweenNormals(
		GetActorUpVector(),
		-Direction
		);
	SetActorRotation(Align * GetActorQuat());
}

void AOrbitCharacter::ToggleInputMode(bool IsLanded)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (AFreeCameraManager* Manager = Cast<AFreeCameraManager>(PlayerController->PlayerCameraManager))
		{
			Manager->ToggleFreeCameraMode(IsLanded);
		}
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (bInGravity)
			{
				Subsystem->RemoveMappingContext(MappingContextSpace);
				Subsystem->AddMappingContext(MappingContextSurface, 0);
			}
			else
			{
				Subsystem->RemoveMappingContext(MappingContextSurface);
				Subsystem->AddMappingContext(MappingContextSpace, 0);
			}
		}
	}
}

bool AOrbitCharacter::TryLand(double Speed)
{
	if (Speed <= ThrustersAcceleration)
	{
		// GetCapsuleComponent()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetEnableGravity(true);
		GetCharacterMovement()->GravityScale = 1;
		bInGravity = true;
		ToggleInputMode(false);
		return true;
	}
	
	return false;
}

// void AOrbitCharacter::OnGravityUpdate(bool NewState)
// {
// 	bInGravity = NewState;
// }

void AOrbitCharacter::Jump()
{
	// Super::Jump();
	if (bInGravity)
	{
		// GetCapsuleComponent()->SetSimulatePhysics(false);
		GetCapsuleComponent()->SetEnableGravity(false);
		GetCharacterMovement()->GravityScale = 0;
		bInGravity = false;
		ToggleInputMode(true);
	
		OrbitComponent->MarkAsMovable();
		OrbitComponent->UpdateOrbitalMovement(
			GetActorLocation(),
			OrbitComponent->GetAttractor()->GetOrbitComponent()->GetInstantVelocity() + ThrustersAcceleration
			);
	}
}
