// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

UCLASS()
class TP3SHOOT_API AAICharacter : public ACharacter
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Stimuli
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionStimuliSourceComponent* StimuliSource;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UAIPerceptionComponent* PerceptionComponent;
	//// Sight sense config
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UAISenseConfig_Sight* SightConfig;



public:
	UPROPERTY(EditAnywhere, Category = "AI TreeAsset")
	class UBehaviorTree* TreeAsset;

public:
	// Sets default values for this character's properties
	AAICharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	float TurnRateGamepad;

protected:

	// Add a gun skeletal mesh component
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* SK_Gun;

	// Particle Start
	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UParticleSystem* ParticleStart;

	// Particle Impact
	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UParticleSystem* ParticleImpact;

	// Fire animation
	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	// Timer for Boost Speed
	FTimerHandle BoostSpeedTimer;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// Aiming function
	void Aim();
	void StopAiming();

	// Firing function
	void Fire();

	void BoostSpeed();
	void RemoveSpeedBoost();

	void FireParticle(FVector Start, FVector Impact);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:

	// Is Aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
	bool IsAiming;

	// Is Firing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firing")
	bool IsFiring;
};
