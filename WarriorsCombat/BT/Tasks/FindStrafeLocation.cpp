// Fill out your copyright notice in the Description page of Project Settings.


#include "FindStrafeLocation.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UFindStrafeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	FVector StartLocation = Pawn->GetActorLocation();
	FVector EndLocation;

	bool bShouldStrafeRight = UKismetMathLibrary::RandomBoolWithWeight(0.5f);

	if (bShouldStrafeRight)
	{
		EndLocation = (Pawn->GetActorRightVector() * Distance) + Pawn->GetActorLocation();
	}
	else
	{
		EndLocation = ((Pawn->GetActorRightVector() * Distance) * -1) + Pawn->GetActorLocation();
	}
	
	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

	FVector LocationToMoveTo = EndLocation;
	if (HitResult.bBlockingHit)
		LocationToMoveTo = HitResult.Location;	

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(MoveToLocation.SelectedKeyName, EndLocation);

	return EBTNodeResult::Succeeded;
}
