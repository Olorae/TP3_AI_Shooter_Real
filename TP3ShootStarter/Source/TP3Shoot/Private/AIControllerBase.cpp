// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AICharacter.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
{
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));

    // Create BehaviorTree Component
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));

    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SetPerceptionComponent(*PerceptionComponent);
    
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->SightRadius = 3000.0f;
    SightConfig->SetMaxAge(8.0f);
    SightConfig->AutoSuccessRangeFromLastSeenLocation = 200.0f;
    SightConfig->LoseSightRadius = 3500;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // Cast your Character/Pawn to get access to the attributes
    if (auto AICharactere = Cast<AAICharacter>(InPawn))
    {
        // Check if the assets has been selected in the editor
        if (AICharactere->TreeAsset && AICharactere->TreeAsset->BlackboardAsset) {

            BlackboardComponent->InitializeBlackboard(*AICharactere->TreeAsset->BlackboardAsset);

            // Run the behavior tree
            BehaviorTreeComponent->StartTree(*AICharactere->TreeAsset);
        }
    }

    /*
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Perception component existes?"));

    if (PerceptionComponent)
    {
        PerceptionComponent = GetPerceptionComponent();
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Perception component existes!"));
    }
    */
}