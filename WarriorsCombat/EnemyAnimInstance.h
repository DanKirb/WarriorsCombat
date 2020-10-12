// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WARRIORSCOMBAT_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class APawn* Pawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class AEnemy* Enemy;

public:

	UFUNCTION(BlueprintCallable, Category = "Animation Properties")
	void UpdateAnimationProperties();
};
