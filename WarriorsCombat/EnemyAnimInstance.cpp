// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayActors/Enemy.h"
#include "Kismet/KismetMathLibrary.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Enemy = Cast<AEnemy>(Pawn);
		}
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	
	if (Pawn)
	{
		MovementSpeed = UKismetMathLibrary::InverseTransformDirection(Pawn->GetTransform(), Pawn->GetVelocity());
		Velocity = Pawn->GetVelocity().Size();

		if (Enemy == nullptr)
		{
			Enemy = Cast<AEnemy>(Pawn);	
		}

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
}