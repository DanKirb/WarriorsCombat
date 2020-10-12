// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToTargetLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UMoveToTargetLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	OwnerComp.GetAIOwner()->MoveToLocation(Blackboard->GetValueAsVector(TargetLocation.SelectedKeyName), Blackboard->GetValueAsFloat(AcceptanceRadius.SelectedKeyName), false);

	return EBTNodeResult::Succeeded;
}