// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastKompir.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyAI.h"
#include "TheLastKompirCharacter.h"
#include "EnemyCharacter.h"
#include "BTTask_MoveToPlayer.h"


EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	AEnemyAI *EnemyCharPC = Cast<AEnemyAI>(OwnerComp.GetAIOwner());

	ATheLastKompirCharacter *Enemy =
		Cast<ATheLastKompirCharacter>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(EnemyCharPC->EnemyKeyID));

	if (Enemy)
	{
		EnemyCharPC->MoveToActor(Enemy, 5.f, true, true, true, 0, true);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;

}
