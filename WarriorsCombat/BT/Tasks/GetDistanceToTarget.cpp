// Fill out your copyright notice in the Description page of Project Settings.


#include "GetDistanceToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

EBTNodeResult::Type UGetDistanceToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* Blackboard = OwnerComp.GetAIOwner()->GetBlackboardComponent();

	FVector PawnLocation = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
	FVector TargetLocation = Blackboard->GetValueAsVector("TargetLocation");

	Blackboard->SetValueAsFloat("DistanceToTarget", UKismetMathLibrary::Vector_Distance(PawnLocation, TargetLocation));

	return EBTNodeResult::Succeeded;
}