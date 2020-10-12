// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class WARRIORSCOMBAT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USceneComponent* Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMeshComponent* Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* Collision;
	
	float BaseDamage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	TArray<AActor*> ActorsToIgnore;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/** Enables the collision for the combat collision box */
	UFUNCTION(BlueprintCallable)
	void ActivateCombatCollision();

	/** Disables the collision for the combat collision box */
	UFUNCTION(BlueprintCallable)
	void DeactivateCombatCollision();

private:

	/** Called when an actor overlaps with the Collision */
	UFUNCTION()
	void CollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
