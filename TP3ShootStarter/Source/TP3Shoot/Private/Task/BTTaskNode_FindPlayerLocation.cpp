// Fill out your copyright notice in the Description page of Project Settings.


#include "Task/BTTaskNode_FindPlayerLocation.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTaskNode_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

    if (NavSystem)
    {
        PlayerPosition = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

        // Find random point within a 100 radius from player
        if (NavSystem->GetRandomReachablePointInRadius(PlayerPosition, 100.0f, RandomLocation))
        {
            // Set value in the blackboard
            TargetLocation = RandomLocation.Location;
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("TargetLocation"), TargetLocation);

            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}