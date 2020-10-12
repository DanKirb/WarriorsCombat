// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class WARRIORSCOMBAT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();

	/** The max health of the enemy */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth;

	/** The current health of the enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Health;

	/** A montage of the enemy death animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	class UAnimMontage* CombatMontage;

	/** A montage of the enemy death animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	class UAnimMontage* DeathMontage;

	/** The particles that will play when the enemy is attacked */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Particles)
	class UParticleSystem* HitParticles;

	/** The sound that will play when the enemy is attacked */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	class USoundCue* HitSoundCue;

	/** Whether the enemy can be launch in the air by an attack */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	bool bCanBeLaunched;

	/** The length of time the enemy freezes for when hit mid-air */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float JuggleTimer;

	/** The length of time the enemy freezes for when hit mid-air */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	bool bIsLaunchedAway;

	/** The collision sphere used to aggros the enemy to the player, implemented in Blueprints */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	class USphereComponent* TargetCollision;

	/** The base damage the enemies attacks will do */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float BaseDamage;

	/** The weapon that will be equipped on BeginPlay() */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TSubclassOf<class AWeapon> WeaponToEquip;

	/** The socket WeaponToEquip will be attached to on BeginPlay() */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	FName WeaponSocketName;

	/** The equipped weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	AWeapon* Weapon;

private:

	/** Wheather the actor is interping to a new location */
	bool bBeingPushed;

	/** The location of the actor before interping */
	FVector StartLocation;

	/** The direction the actor should be pushed */
	FVector PushDirection;

	/** The distance the actor should be pushed */
	float PushDistance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Add impuse to enemy in DirectionOfHit ignoring it's mass */
	void MoveEnemyInHitDirection(FVector DirectionOfHit);

	/** Plays enemy hit effects such as partiles and sounds */
	void PlayHitEffects();

	/** Subtracts DamageAmount from Health, calls Die() if health reaches 0 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Plays the death anim MontageSection from DeathMontage */
	void Die(FName MontageSection);

	/** Disables collision so enemy cannot be hit again */
	UFUNCTION(BlueprintCallable)
	void DisableCollision();

	/** Destroy the actor */
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	/** Freezes actor in it's current place for a length of time set in JuggleTimer */
	void FreezeInAir();

	/** Turns gravity for this actor back on */
	void StartGravity();

	/** Stops any death animations and launches actor with the LaunchVelocity */
	void LaunchEnemy(FVector LaunchVelocity);

	/** Stops any death animtions currently playing */
	void StopDeathAnimations();

	/** Override of Landed in Character.h. If enemy is dead plays death animation when the actor hits the floor */
	virtual void Landed(const FHitResult& Hit) override;

	/** Push enemy in the direction passed in */
	void Push(FVector Direction);

	FORCEINLINE bool IsAlive() { return Health > 0; }

private:

	void InterpToPushedLocation(float DeltaTime);

};
