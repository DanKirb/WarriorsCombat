// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "AIController.h"
#include "Weapon.h"
#include "Warrior.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	TargetCollision = CreateDefaultSubobject<USphereComponent>(TEXT("TargetCollision"));
	TargetCollision->SetupAttachment(GetRootComponent());
	TargetCollision->SetSphereRadius(1000.f);

	MaxHealth = 10.f;
	bCanBeLaunched = true;
	JuggleTimer = 1.f;
	bIsLaunchedAway = false;
	BaseDamage = 5.f;

	PushDistance = 85.f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Health = MaxHealth;

	// Attach weapon to weapon socket
	if (WeaponToEquip && !WeaponSocketName.IsNone())
	{
		Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponToEquip);
		if (Weapon)
		{
			const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName(WeaponSocketName);
			WeaponSocket->AttachActor(Weapon, GetMesh());
			Weapon->BaseDamage = BaseDamage;
		}
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bBeingPushed)
	{
		InterpToPushedLocation(DeltaTime);
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::MoveEnemyInHitDirection(FVector DirectionOfHit)
{
	GetCapsuleComponent()->AddImpulse(DirectionOfHit * 150.f, FName("NAME_None"), true);
}

void AEnemy::PlayHitEffects()
{
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), FRotator(0.f));
	}

	if (HitSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, HitSoundCue);
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	GetCharacterMovement()->StopMovementImmediately();
	Health -= DamageAmount;
	if (!IsAlive())
	{
		Health = 0.f;

		if (!GetCharacterMovement()->IsFalling())
		{
			Die(FName("Death"));
		}
	}
	
	PlayHitEffects();

	return DamageAmount;
}

void AEnemy::Die(FName MontageSection)
{
	GetCharacterMovement()->StopMovementImmediately();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (DeathMontage && AnimInstance)
	{
		AnimInstance->StopAllMontages(false);
		AnimInstance->Montage_Play(DeathMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName(MontageSection), DeathMontage);
	}
}

void AEnemy::DisableCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DeathEnd()
{
	Destroy();

	if (Weapon)
	{
		Weapon->Destroy();
	}
}

void AEnemy::FreezeInAir()
{
	// Freeze enemy in place
	GetCharacterMovement()->Velocity = FVector(0.f);
	GetCharacterMovement()->GravityScale = 0.f;

	// Turn gravity back on after timer
	FTimerHandle TimeHandle;
	GetWorldTimerManager().SetTimer(TimeHandle, this, &AEnemy::StartGravity, JuggleTimer);
}

void AEnemy::StartGravity()
{
	GetCharacterMovement()->GravityScale = 1.f;
}

void AEnemy::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
		
	bIsLaunchedAway = false;
	if (!IsAlive())
	{
		Die(FName("DeathLanded"));
	}
}

void AEnemy::LaunchEnemy(FVector LaunchVelocity)
{
	StopDeathAnimations();
	LaunchCharacter(LaunchVelocity, true, true);
}

void AEnemy::StopDeathAnimations()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (DeathMontage && AnimInstance)
	{
		AnimInstance->Montage_Stop(0.5f, DeathMontage);
	}
}

void AEnemy::Push(FVector Direction)
{
	PushDirection = Direction;
	bBeingPushed = true;
}

void AEnemy::InterpToPushedLocation(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FVector EndPoint = CurrentLocation + (PushDirection * PushDistance);

	if (StartLocation == FVector(0.f))
	{
		StartLocation = GetActorLocation();
	}

	FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, 5.f);
	SetActorLocation(Interp);

	float DistanceTraveled = (GetActorLocation() - StartLocation).Size();
	if ((PushDistance - DistanceTraveled) <= 1.f)
	{
		bBeingPushed = false;
		StartLocation = FVector(0.f);
	}
}