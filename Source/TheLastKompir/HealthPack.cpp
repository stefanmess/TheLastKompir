// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastKompir.h"
#include "TheLastKompirCharacter.h"
#include "HealthPack.h"



// Sets default values

AHealthPack::AHealthPack(const FObjectInitializer& ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Amount = 40.f;

	// Collision set up

	collisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Collisioncomp"));
	FVector boxSize = FVector(100.f, 100.f, 100.f);
	collisionComp->SetBoxExtent(boxSize);
	collisionComp->SetEnableGravity(true);
	collisionComp->SetSimulatePhysics(false);
	RootComponent = collisionComp;


}

// Called when the game starts or when spawned
void AHealthPack::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHealthPack::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AHealthPack::NotifyActorBeginOverlap(class AActor* Other)
{
	if (Other != NULL)
	{
		if (CanBePickedUp(Cast<ATheLastKompirCharacter>(Other)))
		{
			HandlePickUp(Cast<ATheLastKompirCharacter>(Other));
			HandleEfect();
			RemoveFromWorld();
		}
	}
}

void AHealthPack::RemoveFromWorld()
{
	RootComponent->GetChildComponent(0)->SetHiddenInGame(true);

	K2_DestroyActor();
}

bool AHealthPack::CanBePickedUp(class ATheLastKompirCharacter* Other)
{
	if ( Other->IsAlive() && Other != NULL )
	{
		return true;
	}
	return false;
}

void AHealthPack::HandlePickUp(class ATheLastKompirCharacter *Other)
{
	if (Other != NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Health has beed added"));

		Other->AddHealthFromPickUp(this->Amount);
	}

}


void AHealthPack::HandleEfect()
{
	// FVector PlayerLocation = this->GetActorLocation;
	// FRotator PlayerRotation = this->GetActorRotation;

	// Particle spawn when the palyer picks the health will add later when created particle 
	// UGameplayStatics::SpawnEmitterAtLocation(this, Particle_Health, PlayerLocation, PlayerRotation);

}