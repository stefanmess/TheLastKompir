// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackPlayer.generated.h"

/**
 * 
 */
UCLASS()
class THELASTKOMPIR_API UBTTask_AttackPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};
