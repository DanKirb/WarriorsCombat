// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusOnPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UFocusOnPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (bFocusOnPlayer)
	{
		AIController->SetFocus(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
	else
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}

	return EBTNodeResult::Succeeded;
}