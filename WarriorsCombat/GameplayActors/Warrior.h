// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Warrior.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting")
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	EAT_Normal UMETA(DisplayName = "Normal"),
	EAT_LaunchUp UMETA(DisplayName = "LaunchUp"),
	EAT_LaunchAway UMETA(DisplayName = "LaunchAway")
};

UENUM(BlueprintType)
enum class EAttackStrength : uint8
{
	EAS_Light UMETA(DisplayName = "Light"),
	EAS_Heavy UMETA(DisplayName = "Heavy")
};

USTRUCT(BlueprintType)
struct FComboNextAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttackStrength AttackStrength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MontageSection;
};

USTRUCT(BlueprintType)
struct FComboAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttackType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FComboNextAttack> NextAttack;
};

UCLASS()
class WARRIORSCOMBAT_API AWarrior : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWarrior();

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Player follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	/** Collision box that will deal damage to enemies that overlap it */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CombatCollision;

	/** Players max health */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	/** Players current health */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float Health;

	/** Base damage each attack will do */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float BaseDamage;

	/** Base turn rate to scale turning functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/** Whether or not the weapon is attached to the characters hand */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped;

	/** The weapon the character will equip */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> WeaponToEquip;

	/** The name of the socket for when the weapon is not equiped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName UnequippedWeaponSocket;

	/** The name of the socket for when the weapon is equiped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName EquippedWeaponSocket;

	/** The anim montage containing the equiping animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimMontages")
	class UAnimMontage* EquipMontage;

	/** The anim montage containing the attack animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimMontages")
	UAnimMontage* CombatMontage;

	/** The anim montage containing the death ttack animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimMontages")
	UAnimMontage* DeathMontage;

	/** The anim montage containing the death ttack animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimMontages")
	UAnimMontage* RollMontage;

	/** Whether the character is currently attacking */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;

	/** Whether pressing an attack button will continue the combo */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bEnableComboInput;

	/** Whether combo should continue to next attack */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bContinueCombo;
	
	/** The characters attacks and their types, and what attacks they can combo into */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TArray<FComboAttack> ComboAttacks;

	/** The montage section name of the first attack to play when LightAttack() is called */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName FirstLightAttack;

	/** The montage section name of the first attack to play when HeavyAttack() is called */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName FirstHeavyAttack;

	/** The movement state of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EMovementState MovementState;	

	/** The max walk speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;
	
	/** The max sprint speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	/** The max sprint speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsRolling;

	/** The max sprint speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FRotator DesiredRollRotation;

protected:

	/** The current equipped weapon that is spawned into the world */
	AWeapon* Weapon;

	/** The current attack the character is doing */
	FComboAttack CurrentAttack;

	/** The montage section name of the next attack */
	FName NextMontageSectionName;

	/** Enemies that have already been damaged in the current attack, these actors are then ignored for the rest of the attack to avoid duplicate damage */
	TArray<AActor*> EnemiesToIgnore;

	bool bShiftButtonDown;

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:	
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for forward and backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void PlayEquipAnimation();

	/** Equips and unequips weapon and toggles bWeaponEquipped */
	UFUNCTION(BlueprintCallable)
	void ToggleWeaponEquip();

	/** Called when the light attack button is pressed */
	void LightAttack();

	/** Called when the heavy attack button is pressed */
	void HeavyAttack();

	/** Sets bEnableComboInput to true allowing for the next attack input to continue the combo */
	UFUNCTION(BlueprintCallable)
	void EnableInputForNextComboAttack();

	/** Sets bEnableComboInput to false, mean next attack inputs won't continue the combo */
	UFUNCTION(BlueprintCallable)
	void DisableInputForNextComboAttack();

	/** Plays the next attack in the combo */
	UFUNCTION(BlueprintCallable)
	void MoveToNextAttack();

	/** Sets bIsAttacking to false */
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	/** Enables the collision for the combat collision box */
	UFUNCTION(BlueprintCallable)
	void ActivateCombatCollision();

	/** Disables the collision for the combat collision box */
	UFUNCTION(BlueprintCallable)
	void DeactivateCombatCollision();

	/**  */
	UFUNCTION(BlueprintCallable)
	void ToggleIsRolling();

	/** Called when the shift button is pressed or released */
	void SprintKeyDown();
	void SprintKeyUp();

private:

	/** Attaches the Weapon to the passed in socket */
	void AttachWeaponToSocket(const FName SocketName);

	/** Plays the current attack or sets the NextMontageSectionName to the next attack in the combo if bEnableComboInput is true */
	void Attack(EAttackStrength AttackStrength);

	/** Plays the montage section for the CurrentAttack */
	void PlayCurrentAttack();

	/** Find the FComboAttack in ComboAttacks by it's MontageSection property */
	FComboAttack FindComboAttackByMontageSection(FName MontageSection);

	/** Find the FComboNextAttack in CurrentAttack.NextAttack by AttackStrength */
	FComboNextAttack FindNextAttackWithAttackStrength(EAttackStrength AttackStrength);

	/** Sets the NextMontageSectionName to the next attack with the same strength in CurrentAttack.NextAttack */
	void SetNextAttack(EAttackStrength AttackStrength);

	/** Called when an actor overlaps with the CombatCollision */
	UFUNCTION()
	void CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Subtracts DamageAmount from Health, calls Die() if health reaches 0 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Die();

	void ToggleSprint();

	void Roll();

	FORCEINLINE void SetIsAttacking(bool IsAttacking) { bIsAttacking = IsAttacking; }
	FORCEINLINE void SetEnableComboInput(bool EnableComboInput) { bEnableComboInput = EnableComboInput; }
	FORCEINLINE void SetContinueCombo(bool ContinueCombo) { bContinueCombo = ContinueCombo; }

	FORCEINLINE bool IsAlive() { return Health > 0.f; }
};
