// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class THELASTKOMPIR_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnSeePlayer(APawn* InPawn);


	/* Last time the player was spotted */
	float LastSeenTime;

	/* Last time the player was heard */
	float LastHeardTime;

	/* Last time we attacked something */
	float LastMeleeAttackTime;

	/* Time-out value to clear the sensed position of the player. Should be higher than Sense interval in the PawnSense component not never miss sense ticks. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SenseTimeOut;

	/* Resets after sense time-out to avoid unnecessary clearing of target each tick */
	bool bSensedTarget;


	/*Hearing function - will be executed when we hear a Pawn*/
	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Triggers, meta = (AllowPrivateAccess = "true"))  
	class USphereComponent* bodySphereTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Triggers, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* leftHandTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Triggers, meta = (AllowPrivateAccess = "true"))  
	class USphereComponent* rightHandTrigger;

	UFUNCTION()
		void OnEnteredInBodySphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnExitedInBodySphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	//The health of the enemy 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Behavior") 
	float TotalHealth;
	//The range for the enemy attack 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Behavior") 
	float AttackRange;
	//The power of the enemy attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Behavior") 
	float AttackDamage;
	//Check if the enemy is dead or alive 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Behavior") 
	bool IsDead;
	//Check if the enemy is dead or alive 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Behavior") 
	bool IsAttacking; 

public:
	// Sets default values for this character's properties
	AEnemyCharacter();


	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree *BotBehavior;

	/*The Component which is used for the "seeing" sense of the AI, , responsible for sensing other Pawns*/
	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	/*The Behavior Tree of the Character*/
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;


	//Perform attack
	UFUNCTION(BlueprintCallable, Category = "AI") 
	void OnPerformAttack();
	//Applied before performing an attack
	UFUNCTION(BlueprintCallable, Category = "AI") 
	void OnPreAttack();
	//Perform attack done 
	UFUNCTION(BlueprintCallable, Category = "AI") 
	void OnPostAttack(); 

	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);


	UFUNCTION() 
	void OnHandTriggerOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, 
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
		
	FORCEINLINE class USphereComponent* GetBodySphereTrigger() const { return bodySphereTrigger; }


	//Override the PostInitializeComponents() 
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;	
	
};
