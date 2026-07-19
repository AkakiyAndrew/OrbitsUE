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
	
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(RootComponent, FName("headSocket"));
	PitchPivot = CreateDefaultSubobject<USceneComponent>(TEXT("PitchPivot"));
	PitchPivot->SetupAttachment(CameraRoot);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(PitchPivot);
	Camera->bUsePawnControlRotation = false; // Camera rotated manually
	
	
	// Disable these so the Controller doesn't force the Actor back to World-Up alignment
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void AOrbitCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnEnteringGravityField);
	OnActorEndOverlap.AddDynamic(this, &ThisClass::OnLeavingGravityField);
	
	// one-time check at game start
	TArray<AActor*> Overlapping;
	GetOverlappingActors(Overlapping, ACelestialBody::StaticClass());
	if (Overlapping.Num() == 1)
	{
		OnEnteringGravityField(this, Overlapping[0]);
	}
	
	//Add Input Mapping Context
	ToggleInputMode(bOnSurface);
	CameraYaw = CameraRoot->GetRelativeRotation().Yaw;
	CameraPitch = PitchPivot->GetRelativeRotation().Pitch;
	
	CameraRoot->SetRelativeLocation(GetMesh()->GetSocketTransform(HeadSocketName, RTS_Actor).GetLocation());
}

// Called every frame
void AOrbitCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GravityAttractor)
	{
		DirectInGravity();
	}
	
	if (GEngine)
	{
		// Arguments: Key (-1 prevents overwriting), TimeToDisplay (sec), Color, Text
		// FText::Format(TEXT(""), CameraYaw);
		GEngine->AddOnScreenDebugMessage(1, 0, FColor::Cyan, FString::Printf(TEXT("CameraYaw: %f"), CameraYaw));
		GEngine->AddOnScreenDebugMessage(2, 0, FColor::Cyan, FString::Printf(TEXT("CameraPitch: %f"), CameraPitch));
		GEngine->AddOnScreenDebugMessage(3, 0, FColor::Cyan, FString::Printf(TEXT("CameraRoll: %f"), CameraRoot->GetRelativeRotation().Roll));
	}

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
		if (bOnSurface)
		{
			// on-surface movement
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			
			AddMovementInput(Camera->GetForwardVector(), MovementVector.Y);
			AddMovementInput(Camera->GetRightVector(), MovementVector.X);
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
		CameraYaw += LookAxisVector.X; // * Sensitivity;
		CameraPitch += LookAxisVector.Y; // * Sensitivity;
		
		if (GravityAttractor)
		{
			CameraPitch = FMath::Clamp(CameraPitch, -89.f, 89.f);
			// rotate camera up-down 
			PitchPivot->SetRelativeRotation(FRotator(CameraPitch, 0, 0));
			// rotate whole actor left-right when on surface by Camera movement
			AddActorLocalRotation({0, LookAxisVector.X, 0}); 
			//CameraRoot->SetRelativeRotation(FRotator(0, CameraYaw, 0)); // only camera, not actor
		}
		
		// zero-grav, rotate whole actor
		if (!bOnSurface)
		{
			AddActorLocalRotation({LookAxisVector.Y, LookAxisVector.X, 0});
			AddActorLocalRotation({0, 0, LookAxisVector.Z});
		}
		// need for proper z-grav movement
		Controller->SetControlRotation(Camera->GetComponentRotation());
		
		// else
		// {
		// 	// if not on surface - just rotate whole character instead of Camera or other components
		// }
			
		// {
		// add yaw and pitch input to controller
		// AddControllerYawInput(LookAxisVector.X);
		// AddControllerPitchInput(LookAxisVector.Y);
		// }
		// else
		// {
			// // Create a delta rotation from the input. 
			// // FRotator params are (Pitch, Yaw, Roll). 
			// // Note: You may need to negate Y (Pitch) depending on your Input Mapping Context settings.
			// FRotator DeltaRotation(LookAxisVector.Y, LookAxisVector.X, LookAxisVector.Z);
			//
			// // TODO: can use this place to clamp / smooth rotation speed
			// // Apply the rotation relative to the Actor's current local transform
			// AddActorLocalRotation(DeltaRotation);
			//
			// // Update the Controller rotation to match the Actor so the Camera remains synced
			// Controller->SetControlRotation(GetActorRotation());
		// }
	}
}

// void AOrbitCharacter::MoveInGravity(const FVector& Delta)
// {
// 	GetCapsuleComponent()->AddForce(Delta, NAME_None, true);
// }

void AOrbitCharacter::DirectInGravity()
{
	FVector AttractorPos = GravityAttractor->GetOrbitComponent()->GetOrbitalPosition();
	FVector CurrentPos = GetActorLocation();
	FVector Direction = (AttractorPos - CurrentPos).GetSafeNormal();
	
	// DrawDebugSphere(GetWorld(), AttractorPos, 100, 4, FColor::Purple, false, 0);
	// DrawDebugSphere(GetWorld(), CurrentPos, 100, 4, FColor::Purple, false, 0);
	
	//float GravityStrength = (AttractorPos - CurrentPos).Length() / GravityAttractor->GetOrbitComponent()->GetGravityFieldRadius() * GravityAttractor->GetOrbitComponent()->GetGravityStrength();
	
	GetCharacterMovement()->SetGravityDirection(Direction);
	// rotate character "upward" relatively to current local gravity direction
	FQuat Align = FQuat::FindBetweenNormals(
		GetActorUpVector(),
		-Direction
		);
	SetActorRotation(Align * GetActorQuat());
	
	// FRotator NewControllerRotation = Controller->GetControlRotation();
	// NewControllerRotation.Roll = GetActorRotation().Roll;
	// Controller->SetControlRotation(NewControllerRotation);
	
	UE_LOG(LogTemp, Log, TEXT("Directing in gravity: %s"), *Direction.ToString());
	DrawDebugDirectionalArrow(GetWorld(), CurrentPos, CurrentPos + Direction*200, 5, FColor::Blue, false, 0.5);
	
	// if (Controller)
	// {
	// 	AddControllerRollInput((Align * GetActorQuat()).Rotator().Roll);
	// }
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
			if (bOnSurface)
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

void AOrbitCharacter::OnEnteringGravityField(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ACelestialBody* Body = Cast<ACelestialBody>(OtherActor))
	{
		GravityAttractor = Body;
		//GetWorld()->GetTimerManager().SetTimer(GravityDirectionTimerHandle, this, &UOrbitDynamicObjectComponent::DirectInGravity, 0.f, true);
		// ClearPrediction();
		// TODO: for character, make OnEnteringGravity delegate, so its SkelMesh began to "fall" (animation variable)
		//OnGravityChanged.Broadcast(false);
		
		UE_LOG(LogTemp, Log, TEXT("Entering grav field."));
		DrawDebugSphere(GetWorld(), GetActorLocation(), 100, 4, FColor::Green, false, 2);
	}
}

void AOrbitCharacter::OnLeavingGravityField(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != GravityAttractor)
	{
		return;
	}
	
	// reset camera back to "look forward"
	CameraYaw = 0;
	CameraPitch = 0;
	CameraRoot->SetRelativeRotation(FRotator(0, CameraYaw, 0));
	PitchPivot->SetRelativeRotation(FRotator(CameraPitch, 0, 0));
	
	// TODO: calculate from kepler attractor its velocity at given SimTime
	// TODO: move into Jump() for Character
	//UpdateOrbitalMovement(GetOwner()->GetActorLocation(), GravityAttractor->GetOrbitComponent()->GetInstantVelocity());
	
	GravityAttractor = nullptr;
	//GetWorld()->GetTimerManager().ClearTimer(GravityDirectionTimerHandle);
	
	// TODO: call Character's switch to zerogravity controls (input mapping)
	
	OrbitComponent->CalculatePrediction();
	//OnGravityChanged.Broadcast(false);
	
	UE_LOG(LogTemp, Log, TEXT("Leaving grav field."));
	DrawDebugSphere(GetWorld(), GetActorLocation(), 100, 4, FColor::Orange, false, 2);
}

bool AOrbitCharacter::TryLand(double Speed)
{
	if (Speed <= ThrustersAcceleration)
	{
		// GetCapsuleComponent()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetEnableGravity(true);
		GetCharacterMovement()->GravityScale = 1;
		bOnSurface = true;
		ToggleInputMode(false);
		return true;
	}
	
	return false;
}

void AOrbitCharacter::Jump()
{
	// Super::Jump();
	if (bOnSurface)
	{
		// GetCapsuleComponent()->SetSimulatePhysics(false);
		GetCapsuleComponent()->SetEnableGravity(false);
		GetCharacterMovement()->GravityScale = 0;
		bOnSurface = false;
		ToggleInputMode(true);
	
		OrbitComponent->MarkAsMovable();
		OrbitComponent->UpdateOrbitalMovement(
			GetActorLocation(),
			GravityAttractor->GetOrbitComponent()->GetInstantVelocity() + ThrustersAcceleration
			);
	}
}
