// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HealthPack.generated.h"

UCLASS()
class THELASTKOMPIR_API AHealthPack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthPack();

	AHealthPack(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void NotifyActorBeginOverlap(class AActor* Other) override;

	void RemoveFromWorld();

private:
	bool CanBePickedUp(class ATheLastKompirCharacter* Other);

	void HandlePickUp(class ATheLastKompirCharacter* Other);

	void HandleEfect();


protected:
	
	UPROPERTY(Editanywhere, Category = "Health")
		float Amount;

	//UPROPERTY(EditDefaultsOnly, Category = "Particle_Health")
		//UParticleSystem* Particle_Health;

	UBoxComponent* collisionComp;
	
	
};
