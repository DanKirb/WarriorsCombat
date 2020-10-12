// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MoveToTargetLocation.generated.h"

/**
 * 
 */
UCLASS()
class WARRIORSCOMBAT_API UMoveToTargetLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	/** The location to move to */
	UPROPERTY(EditAnywhere, Category = Task)
	FBlackboardKeySelector TargetLocation;

	/** The accptance radius for the move to location */
	UPROPERTY(EditAnywhere, Category = Task)
	FBlackboardKeySelector AcceptanceRadius;
};
