// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase.generated.h"

UCLASS()
class TP3SHOOT_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* SightConfig;
	
public:

    AAIControllerBase(const FObjectInitializer& ObjectInitializer);

    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(Transient)
    class UBehaviorTreeComponent* BehaviorTreeComponent;

    UPROPERTY(Transient)
    class UBlackboardComponent* BlackboardComponent;
};