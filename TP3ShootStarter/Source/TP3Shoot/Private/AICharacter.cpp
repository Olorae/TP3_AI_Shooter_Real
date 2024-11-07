// Fill out your copyright notice in the Description page of Project Settings.

#include "AICharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include <AIControllerBase.h>
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


// Sets default values
AAICharacter::AAICharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create SK_Gun
	SK_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	SK_Gun->SetupAttachment(GetMesh());
	// Set parent socket
	SK_Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("GripPoint"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Stimuli
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));

	//PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	// Create the sight config
	//SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	/*
	// Enregistrement du stimulus pour la perception de la vision et de l'ouïe
	StimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimuliSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());

	StimuliSource->RegisterWithPerceptionSystem();


	// Composant de perception
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	//SetPerceptionComponent(*AIPerceptionComponent);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->PeripheralVisionAngleDegrees = 90.0f;                 // Angle de vision
	SightConfig->SightRadius = 3000.0f;                                // Rayon de détection
	SightConfig->SetMaxAge(8.0f);                                      // Durée de la mémorisation de la cible
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 200.0f;        // Distance d'auto-détection
	SightConfig->LoseSightRadius = 3500.0f;                            // Rayon de perte de la vue

	// Détection par affiliation
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Ajouter la configuration de la vision au composant de perception
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	*/
}

void AAICharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AAICharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AAICharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AAICharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AAICharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AAICharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AAICharacter::Aim()
{
	IsAiming = true;
}

void AAICharacter::StopAiming()
{
	IsAiming = false;
}

void AAICharacter::Fire()
{
	FVector Start, LineTraceEnd, ForwardVector;

	if (IsAiming)
	{

		Start = FollowCamera->GetComponentLocation();

		ForwardVector = FollowCamera->GetForwardVector();

		LineTraceEnd = Start + (ForwardVector * 10000);
	}
	else {

		// Get muzzle location
		Start = SK_Gun->GetSocketLocation("MuzzleFlash");

		// Get Rotation Forward Vector
		ForwardVector = FollowCamera->GetForwardVector();

		// Get End Point
		LineTraceEnd = Start + (ForwardVector * 10000);
	}
}

void AAICharacter::BoostSpeed()
{
	// Set Max walking speed to 800
	GetCharacterMovement()->MaxWalkSpeed = 800.f;

	GetWorld()->GetTimerManager().SetTimer(BoostSpeedTimer, [&]()
		{
			// Set Max walking speed to 500
			GetCharacterMovement()->MaxWalkSpeed = 500.f;

			// Clear existing timer boost speed
			GetWorldTimerManager().ClearTimer(BoostSpeedTimer);

		}, 4, false);
}

void AAICharacter::RemoveSpeedBoost()
{
	// Set Max walking speed to 500
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void AAICharacter::FireParticle(FVector Start, FVector Impact)
{
	if (!ParticleStart || !ParticleImpact) return;

	FTransform ParticleT;

	ParticleT.SetLocation(Start);

	ParticleT.SetScale3D(FVector(0.25, 0.25, 0.25));

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleStart, ParticleT, true);

	// Spawn particle at impact point
	ParticleT.SetLocation(Impact);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleImpact, ParticleT, true);
}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AAICharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AAICharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AAICharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AAICharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AAICharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AAICharacter::TouchStopped);

	// Aiming 
	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &AAICharacter::Aim);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &AAICharacter::StopAiming);

	// Fire
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AAICharacter::Fire);

	// Boost Speed
	PlayerInputComponent->BindAction("BoostSpeed", IE_Pressed, this, &AAICharacter::BoostSpeed);
	PlayerInputComponent->BindAction("BoostSpeed", IE_Released, this, &AAICharacter::RemoveSpeedBoost);
}

