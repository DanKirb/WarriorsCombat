// Fill out your copyright notice in the Description page of Project Settings.


#include "FindActorLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UFindActorLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* Blackboard = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	
	Blackboard->SetValueAsVector("TargetLocation", UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());

	return EBTNodeResult::Succeeded;
}