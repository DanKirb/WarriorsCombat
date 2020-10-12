// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayActors/Warrior.h"

void UWarriorAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Warrior = Cast<AWarrior>(Pawn);
		}
	}
}

void UWarriorAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		if (Warrior == nullptr)
		{
			Warrior = Cast<AWarrior>(Pawn);
		}

		// Interp to the characters roll direction if rolling
		if (Warrior->bIsRolling)
		{	
			FRotator CurrentRotation = Warrior->GetActorRotation();

			if (!CurrentRotation.Equals(Warrior->DesiredRollRotation))
			{
				Warrior->SetActorRotation(FMath::RInterpTo(CurrentRotation, Warrior->DesiredRollRotation, GetWorld()->DeltaTimeSeconds, 25.f));
			}
		}
	}
}
