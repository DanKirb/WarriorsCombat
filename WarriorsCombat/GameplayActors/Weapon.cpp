// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Warrior.h"
#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Root);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetRootComponent());	

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CollisionOnOverlapBegin);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::CollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (ActorsToIgnore.Contains(OtherActor) || OtherActor == GetAttachParentActor())
		return;

	if (Cast<AEnemy>(GetAttachParentActor()))
	{
		if (Cast<AWarrior>(OtherActor))
		{
			UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, nullptr, this, UDamageType::StaticClass());
		}
	}
	else if (Cast<AWarrior>(GetAttachParentActor()))
	{
		if (Cast<AEnemy>(OtherActor))
		{
			UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, nullptr, this, UDamageType::StaticClass());
		}
	}

	ActorsToIgnore.Add(OtherActor);
}

void AWeapon::ActivateCombatCollision()
{
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCombatCollision()
{
	ActorsToIgnore.Empty();
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}