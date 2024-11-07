// Fill out your copyright notice in the Description page of Project Settings.


#include "Task/BTTaskNode_FindLocationBehindAI.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include <Perception/AIPerceptionComponent.h>
#include "AIControllerBase.h"

EBTNodeResult::Type UBTTaskNode_FindLocationBehindAI::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Get AI Controller and the controlled Pawn
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* AIPawn = AIController->GetPawn();

	// Get the navigation Area in the current level
    NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()); //NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

    if (NavArea)
    {
        // Get the current AI position & direction
        FVector AIPosition = AIPawn->GetActorLocation();
        FVector AIDirection = AIPawn->GetActorForwardVector();

        // Set the location in the blackboard
        BehindLocation = AIPosition - (3 * AIDirection);
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("BehindLocation"), BehindLocation);

        return EBTNodeResult::Succeeded;
    }
    return EBTNodeResult::Failed;
}