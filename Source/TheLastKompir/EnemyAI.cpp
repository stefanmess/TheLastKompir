// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastKompir.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "PatrolTargetPoint.h"
#include "TheLastKompirCharacter.h"
#include "EnemyCharacter.h"
#include "EnemyAI.h"



AEnemyAI::AEnemyAI()
{
	BlackboardCom = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardCom"));

	BehaviorCom = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorCom"));

	//LocationToGoKey = "LocationToGo";
}

void AEnemyAI::Possess(APawn *InPawn)
{
	Super::Possess(InPawn);

	AEnemyCharacter *EnemyChar = Cast<AEnemyCharacter>(InPawn);

	if (EnemyChar && EnemyChar->BotBehavior)
	{
		BlackboardCom->InitializeBlackboard(*EnemyChar->BotBehavior->BlackboardAsset);

		//Target is the object created in the blackboard
		//EnemyKeyID = BlackboardCom->GetKeyID("Target");


		/*Populate the array of available bot target points
		The following function needs a TArray of AActors, that's why I declared the
		BotTargetPoints as such. When I will need to get an exact point and compare it,
		I will cast it to the corresponding class (ABotTargetPoint)*/
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APatrolTargetPoint::StaticClass(), PatrolTargetPoints);

		BehaviorCom->StartTree(*EnemyChar->BotBehavior);
	}

}

void AEnemyAI::SetSeenTarget(APawn* InPawn)
{
	//Registers the Pawn that the AI has seen in the blackboard
	if (BlackboardCom)
	{
		BlackboardCom->SetValueAsObject(BlackboardKey, InPawn);
	}
}

void AEnemyAI::SetSensedTarget(APawn* NewTarget)
{
	//Set a new target to follow
	if (BlackboardCom) BlackboardCom->SetValueAsObject(TargetKey, NewTarget);
}

bool AEnemyAI::PawnCanBeSeen(APawn * target)
{
	// implement for seen pawn
	return false;
}

class ATheLastKompirCharacter* AEnemyAI::GetEnemy() const 
{
	if (BlackboardCom) 
	{ 
		return Cast<ATheLastKompirCharacter>(BlackboardCom->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));
	}
	return NULL;
}

void AEnemyAI::UpdateControlRotation(float DeltaTime, bool bUpdatePawn) 
{
	// Look toward focus  
	FVector TheCenter = GetFocalPoint();  
	if (!TheCenter.IsZero() && GetPawn())  
	{    
		FVector Direction = TheCenter - GetPawn()->GetActorLocation();    
		FRotator TheNewRotation = Direction.Rotation();
		TheNewRotation.Yaw = FRotator::ClampAxis(TheNewRotation.Yaw);
		SetControlRotation(TheNewRotation);
		APawn* const _tempPawn = GetPawn();    
		if (_tempPawn &&bUpdatePawn) 
		{ 
			_tempPawn->FaceRotation(TheNewRotation, DeltaTime); 
		}
	} 
}

void AEnemyAI::OnSearchForEnemy()
{
	APawn* _tempPawn = GetPawn();  
	if (_tempPawn == NULL) { return; }
	const FVector _tempLocation = _tempPawn->GetActorLocation();  
	float BestDistSq = MAX_FLT;
	
	ATheLastKompirCharacter* PlayerPawn = NULL;
	//foreach all pawns in world  
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)  
	{    
		if (PawnCanBeSeen(*It))    
		{      
			ATheLastKompirCharacter* TestPawn = Cast<ATheLastKompirCharacter>(*It);
			AEnemyCharacter* const _testEnemy = Cast<AEnemyCharacter>(TestPawn);
			if (_testEnemy) 
			{        
				//it is just another enemy, not player      
			}      
			else     
			{        
				if (TestPawn && TestPawn->GetIsStillAlive())        
				{          
					UE_LOG(LogClass, Log, TEXT(" ===================>>>>>  ENEMY SEEN %s "), *GetNameSafe(*It));
					const float _distanceSq = (TestPawn->GetActorLocation() - _tempLocation).SizeSquared();
					if (_distanceSq < BestDistSq)          
					{            
						BestDistSq = _distanceSq;            
						PlayerPawn = TestPawn;          
					}        
				}      
			}
		}
	}
	if (PlayerPawn) 
	{    
		// We saw someone, so set him as target.    
		SetSensedTarget(PlayerPawn);
		UE_LOG(LogClass, Log, TEXT(" ===================>>>>>  Set Target"));
	}
}