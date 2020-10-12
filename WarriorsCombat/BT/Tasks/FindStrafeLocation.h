// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FindStrafeLocation.generated.h"

/**
 * 
 */
UCLASS()
class WARRIORSCOMBAT_API UFindStrafeLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	/** The location that will be set in this task */
	UPROPERTY(EditAnywhere, Category = Task)
	FBlackboardKeySelector MoveToLocation;

	/** How far to move */
	UPROPERTY(EditAnywhere, Category = Task)
	float Distance;
};
