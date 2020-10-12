// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FocusOnPlayer.generated.h"

/**
 * 
 */
UCLASS()
class WARRIORSCOMBAT_API UFocusOnPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	/** Whether AI should focus or clear focus */
	UPROPERTY(EditAnywhere, Category = Task)
	bool bFocusOnPlayer;
};
