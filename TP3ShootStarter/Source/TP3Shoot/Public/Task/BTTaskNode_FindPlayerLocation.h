// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FindPlayerLocation.generated.h"

/**
 * 
 */
UCLASS()
class TP3SHOOT_API UBTTaskNode_FindPlayerLocation : public UBTTaskNode
{
    GENERATED_BODY()

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    // Variables
    FVector PlayerPosition;
    FVector TargetLocation;
    FNavLocation RandomLocation;

    class UNavigationSystemV1* NavSystem;
};