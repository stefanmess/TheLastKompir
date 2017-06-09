// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyCharacter.h"
#include "EnemyAI.generated.h"

/**
 * 
 */
UCLASS()
class THELASTKOMPIR_API AEnemyAI : public AAIController
{
	GENERATED_BODY()

protected:

	UPROPERTY(transient)
	class UBlackboardComponent *BlackboardCom;

	UPROPERTY(transient)
	class UBehaviorTreeComponent *BehaviorCom;	

	/*I use the AActor class as the Template class so I can use the GetAllActorsOfClass function.
	However, in this collection I'm going to store all the Available ABotTargetPoint instances.*/
	TArray<AActor*> PatrolTargetPoints;

	virtual void Possess(APawn *InPawn) override;

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	
public:

	AEnemyAI();
	
	/*This property is used to find a certain key for our blackboard.
	We will create the blackboard later in this tutorial*/
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetKey = "SensedPawn";

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName BlackboardKey = "Target";

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName LocationToGoKey = "LocationGoTo";

	void SetSeenTarget(APawn *InPawn);

	UFUNCTION(BlueprintCallable, Category = Behaviour) 
	bool PawnCanBeSeen(APawn * target);

	UFUNCTION(BlueprintCallable, Category = Behaviour)
	void OnSearchForEnemy();

	/*Sets the new sensed target value inside our Blackboard values*/
	void SetSensedTarget(APawn* NewTarget);

	class ATheLastKompirCharacter*	GetEnemy() const;

	uint8 EnemyKeyID;

	FORCEINLINE UBlackboardComponent *GetBlackboardComp() const { return BlackboardCom; }

	FORCEINLINE TArray<AActor*> GetAvailableTargetPoints() { return PatrolTargetPoints; }

};
