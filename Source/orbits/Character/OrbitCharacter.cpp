// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitCharacter.h"

#include "orbits/Orbital/OrbitDynamicObject.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
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
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOrbitCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOrbitCharacter::Look);
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
		// find out which way is forward
		FRotationMatrix RotationMatrix(Controller->GetControlRotation());

		FVector Direction = RotationMatrix.GetUnitAxis(EAxis::X) * MovementVector.X + RotationMatrix.GetUnitAxis(EAxis::Y) * MovementVector.Y + RotationMatrix.GetUnitAxis(EAxis::Z) * MovementVector.Z;
		Direction.Normalize();
		FVector ThrustVector = ThrustersAcceleration * Direction;
		
		DrawDebugLine(
			GetWorld(), 
			GetActorLocation(),
			GetActorLocation() + ThrustVector * 100,
			FColor::White,
			false,
			2
			);
		
		OrbitComponent->AddOrbitalVelocity(ThrustVector);
		
		// TODO: use later for in-gravity movement?
		// AddMovementInput(ForwardDirection, MovementVector.Y);
		// AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AOrbitCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector LookAxisVector = Value.Get<FVector>();

	if (Controller != nullptr)
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

void AOrbitCharacter::SetGravityDirection(const FVector& GravityDir)
{
	GetCharacterMovement()->SetGravityDirection(GravityDir);
}