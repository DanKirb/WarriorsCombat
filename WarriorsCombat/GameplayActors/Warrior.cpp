// Fill out your copyright notice in the Description page of Project Settings.


#include "Warrior.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Weapon.h"
#include "Enemy.h"

// Sets default values
AWarrior::AWarrior()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls towards to the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera follows at this distance 
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm base on controller

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->bUsePawnControlRotation = false;
	// Set our turn rates for inputs
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	GetCapsuleComponent()->SetCapsuleSize(34.f, 88.f);

	// Dont rotate the character when the controller rotates. Only rotate the camera boom
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Turn character to face the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	bWeaponEquipped = false;
	bIsAttacking = false;
	bContinueCombo = false;	

	BaseDamage = 10.f;
	MaxHealth = 10.f;

	MovementState = EMovementState::EMS_Normal;
	WalkSpeed = 600.f;
	SprintSpeed = 1000.f;
	bIsRolling = false;

	DesiredRollRotation = FRotator(0.f, 0.f, 360.f);

	bShiftButtonDown = false;
}

// Called when the game starts or when spawned
void AWarrior::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	if (WeaponToEquip)
	{	
		AActor* SpawnedWeaponActor = GetWorld()->SpawnActor(WeaponToEquip);
		AWeapon* SpawnedWeapon = Cast<AWeapon>(SpawnedWeaponActor);

		if (SpawnedWeapon)
		{
			Weapon = SpawnedWeapon;
			AttachWeaponToSocket(UnequippedWeaponSocket);
		}
	}

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWarrior::CombatCollisionOnOverlapBegin);
}

// Called every frame
void AWarrior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWarrior::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AWarrior::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWarrior::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Equip", EInputEvent::IE_Pressed, this, &AWarrior::PlayEquipAnimation);
	PlayerInputComponent->BindAction("LightAttack", EInputEvent::IE_Pressed, this, &AWarrior::LightAttack);
	PlayerInputComponent->BindAction("HeavyAttack", EInputEvent::IE_Pressed, this, &AWarrior::HeavyAttack);

	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AWarrior::SprintKeyDown);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AWarrior::SprintKeyUp);

	PlayerInputComponent->BindAction("Roll", EInputEvent::IE_Pressed, this, &AWarrior::Roll);
}

void AWarrior::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		// find which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AWarrior::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		// find which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AWarrior::PlayEquipAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (bIsAttacking || AnimInstance->IsAnyMontagePlaying())
		return;
		
	if (EquipMontage && AnimInstance)
	{
		if (!AnimInstance->Montage_IsPlaying(EquipMontage))
		{
			FName MontageSection = "Equip";
			if (bWeaponEquipped)
				MontageSection = "Unequip";

			AnimInstance->Montage_Play(EquipMontage, 1.f);
			AnimInstance->Montage_JumpToSection(MontageSection, EquipMontage);
		}
	}
}

void AWarrior::ToggleWeaponEquip()
{
	if (bWeaponEquipped)
	{
		AttachWeaponToSocket(UnequippedWeaponSocket);
	}
	else 
	{
		AttachWeaponToSocket(EquippedWeaponSocket);
	}

	bWeaponEquipped = !bWeaponEquipped;
}

void AWarrior::AttachWeaponToSocket(const FName SocketName)
{
	const USkeletalMeshSocket* Socket = GetMesh()->GetSocketByName(SocketName);

	if (Socket)
	{
		Socket->AttachActor(Weapon, GetMesh());
	}
}

void AWarrior::LightAttack()
{
	Attack(EAttackStrength::EAS_Light);
}

void AWarrior::HeavyAttack()
{
	Attack(EAttackStrength::EAS_Heavy);
}

void AWarrior::EnableInputForNextComboAttack()
{
	SetEnableComboInput(true);
}

void AWarrior::DisableInputForNextComboAttack()
{
	SetEnableComboInput(false);
}

void AWarrior::MoveToNextAttack()
{	
	if (bContinueCombo && !NextMontageSectionName.IsNone())
	{		
		CurrentAttack = FindComboAttackByMontageSection(NextMontageSectionName);

		PlayCurrentAttack();
	}

	SetContinueCombo(false);
}

void AWarrior::AttackEnd()
{
	SetIsAttacking(false);
}

void AWarrior::Attack(EAttackStrength AttackStrength)
{
	if (!ComboAttacks.Num() || bIsRolling)
		return;

	if (!bWeaponEquipped)
	{
		PlayEquipAnimation();
		return;
	}

	// If not already attacking do first attack
	if (!bIsAttacking)
	{
		SetIsAttacking(true);

		auto FirstAttack = FirstLightAttack;
		switch (AttackStrength)
		{
		case EAttackStrength::EAS_Light:
			FirstAttack = FirstLightAttack;
			break;
		case EAttackStrength::EAS_Heavy:
			FirstAttack = FirstHeavyAttack;
			break;
		default:
			break;
		}

		CurrentAttack = FindComboAttackByMontageSection(FirstAttack);
		PlayCurrentAttack();
	}
	// If attacking and button is pressed in the window to do next move
	else if (bIsAttacking && bEnableComboInput)
	{
		SetNextAttack(AttackStrength);
	}
}

void AWarrior::PlayCurrentAttack()
{
	if (CurrentAttack.MontageSection.IsNone())
		return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection(CurrentAttack.MontageSection, CombatMontage);
	}
}

FComboAttack AWarrior::FindComboAttackByMontageSection(FName MontageSection)
{
	for (FComboAttack Attack : ComboAttacks)
	{
		if (Attack.MontageSection == MontageSection)
		{
			return Attack;
		}
	}

	return FComboAttack();
}

void AWarrior::SetNextAttack(EAttackStrength AttackStrength)
{
	FComboNextAttack NextAttackMove = FindNextAttackWithAttackStrength(AttackStrength);

	if (!NextAttackMove.MontageSection.IsNone())
	{
		NextMontageSectionName = NextAttackMove.MontageSection;
		SetContinueCombo(true);
	}
	else
	{
		NextMontageSectionName = FName();
		CurrentAttack = FComboAttack();
	}
}

FComboNextAttack AWarrior::FindNextAttackWithAttackStrength(EAttackStrength AttackStrength)
{
	for (FComboNextAttack Attack : CurrentAttack.NextAttack)
	{
		if (Attack.AttackStrength == AttackStrength)
		{
			return Attack;
		}
	}

	return FComboNextAttack();
}

void AWarrior::CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ignore actor if it's already been damaged during this attack
	if (EnemiesToIgnore.Contains(OtherActor))
		return;

	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	if (Enemy)
	{
		// Damage the enemy
		UGameplayStatics::ApplyDamage(Enemy, BaseDamage, GetController(), this, UDamageType::StaticClass());

		switch (CurrentAttack.AttackType)
		{
		case EAttackType::EAT_Normal:
			if (Enemy->GetMovementComponent()->IsFalling())
			{					
				// if enemy in the air freeze them in place when hit
				Enemy->FreezeInAir();
				break;
			}

			Enemy->Push(GetActorForwardVector());
			break;
		case EAttackType::EAT_LaunchUp:
			if (Enemy->bCanBeLaunched)
			{
				// Launch enemy directly up
				Enemy->LaunchEnemy(FVector(0.f, 0.f, 500.f));
			}
			break;
		case EAttackType::EAT_LaunchAway:
			if (Enemy->bCanBeLaunched)
			{
				// Launch enemy away from the centre of the character
				FVector LaunchDirection = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), Enemy->GetActorLocation()) * 800.f;
				FVector Velocity = FVector(LaunchDirection.X, LaunchDirection.Y, 800.f);
				Enemy->bIsLaunchedAway = true;
				Enemy->LaunchEnemy(Velocity);				
			}
		default:
			break;
		}

		EnemiesToIgnore.Add(Enemy);
	}
}

void AWarrior::ActivateCombatCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWarrior::DeactivateCombatCollision()
{
	EnemiesToIgnore.Empty();
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float AWarrior::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	
	if (!IsAlive())
	{
		Health = 0.f;
		Die();
	}

	return DamageAmount;
}

void AWarrior::Die()
{
	GetCharacterMovement()->StopMovementImmediately();
	
	// Disable all player inputs
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (DeathMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(DeathMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("Death"), DeathMontage);
	}
}

void AWarrior::ToggleSprint()
{
	if (GetVelocity().IsNearlyZero())
		return;

	switch (MovementState)	
	{
	case EMovementState::EMS_Normal:
		MovementState = EMovementState::EMS_Sprinting;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		break;
	case EMovementState::EMS_Sprinting:
		MovementState = EMovementState::EMS_Normal;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;
	default:
		break;
	}
}

void AWarrior::Roll()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (RollMontage && AnimInstance && !bIsRolling && !bIsAttacking)
	{
		ToggleIsRolling();
		FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();

		if (!LastInputVector.IsZero())
		{
			DesiredRollRotation = LastInputVector.Rotation();

			// Play roll animation faster if player is sprinting
			float PlayRate = MovementState == EMovementState::EMS_Sprinting ? 1.2f : 1.f;

			AnimInstance->Montage_Play(RollMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(FName("Roll"), RollMontage);
		}
		else
		{
			DesiredRollRotation = GetActorRotation();
			AnimInstance->Montage_Play(RollMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("RollBack"), RollMontage);
		}
	}
}

void AWarrior::ToggleIsRolling() 
{ 
	bIsRolling = !bIsRolling; 
}

void AWarrior::SprintKeyDown()
{
	if (GetVelocity().IsNearlyZero())
		return;

	bShiftButtonDown = true;
	

	MovementState = EMovementState::EMS_Sprinting;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AWarrior::SprintKeyUp()
{
	bShiftButtonDown = false;
	MovementState = EMovementState::EMS_Normal;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
