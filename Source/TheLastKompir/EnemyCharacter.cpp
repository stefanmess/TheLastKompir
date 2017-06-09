// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastKompir.h"
#include "EnemyAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"
#include "TheLastKompirCharacter.h"
#include "BasicEnemyAnimInstance.h"
#include "EnemyCharacter.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set the enemy behavior values  
	TotalHealth = 100;  
	AttackRange = 100; 
	AttackDamage = 10;  
	IsDead = false;  
	IsAttacking = false;
	
	AIControllerClass = AEnemyAI::StaticClass();

	// time interval for searching the player
	SenseTimeOut = 2.5f;

	//Because the enemy have to rotate to face the running direction, The Yaw rotation needed!  
	bUseControllerRotationYaw = true;

	//Initializing the pawn sensing component
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	//Set the peripheral vision angle to 85 degrees
	PawnSensingComp->SetPeripheralVisionAngle(85.f);
	PawnSensingComp->SensingInterval = .25f; // 4 times per second  
	PawnSensingComp->bOnlySensePlayers = true;  

	bodySphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("BodyTriggerSphere"));

	bodySphereTrigger->SetSphereRadius(100.f);  
	bodySphereTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "pelvis");

	FColor handsTriggersColor = FColor(0, 0, 255, 255);
	leftHandTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHandTriggerSphere"));  
	leftHandTrigger->SetSphereRadius(15.f);  
	leftHandTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "hand_l");
	leftHandTrigger->ShapeColor = handsTriggersColor;  
	leftHandTrigger->bGenerateOverlapEvents = 0;
	
	rightHandTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandTriggerSphere"));  
	rightHandTrigger->SetSphereRadius(15.f);  
	rightHandTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "hand_r");
	rightHandTrigger->ShapeColor = handsTriggersColor;  
	rightHandTrigger->bGenerateOverlapEvents = 0;
	//Just make sure to not use the Enemy::Character capsule as the navigation collision, and use the agent and set its radius to something fits the enemy size  
	//the main goal is to avoid as much as possible the cases when the enemy meshes intersecting with each other, or with the environment  
	//GetCharacterMovement()->NavAgentProps.AgentRadius = 400.f;  
	//GetCharacterMovement()->SetUpdateNavAgentWithOwnersCollisions(false);

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Register the function that is going to fire when the character sees a Pawn
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSeePlayer);
	}
}

void AEnemyCharacter::OnSeePlayer(APawn* InPawn)
{
	AEnemyAI* AIController = Cast<AEnemyAI>(GetController());
	//Set the seen target on the blackboard
	if (AIController)
	{

		LastSeenTime = GetWorld()->GetTimeSeconds();
		bSensedTarget = true;

		GLog->Log("Oh hello there");
		AIController->SetSeenTarget(InPawn);
	}
}

void AEnemyCharacter::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	AEnemyAI* Con = Cast<AEnemyAI>(GetController());

	//We don't want to hear ourselves
	if (Con && PawnInstigator != this)
	{
		LastSeenTime = GetWorld()->GetTimeSeconds();
		bSensedTarget = true;

		//Updates our target based on what we've heard.
		//Con->SetSensedTarget(PawnInstigator);
	}
}

// Called every frame
void AEnemyCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bSensedTarget && (GetWorld()->TimeSeconds - LastSeenTime) > SenseTimeOut
		&& (GetWorld()->TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		AEnemyAI* AIController = Cast<AEnemyAI>(GetController());
		if (AIController)
		{
			bSensedTarget = false;
			/* Reset */
			AIController->SetSeenTarget(nullptr);

			/* Stop playing the hunting sound */
			//BroadcastUpdateAudioLoop(false);
		}
	}

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();  
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSeePlayer);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AEnemyCharacter::OnHearNoise);

	// Register to the delegate of OnComponentBeginOverlap  
	leftHandTrigger->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnHandTriggerOverlap);
	rightHandTrigger->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnHandTriggerOverlap); 
	bodySphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnEnteredInBodySphere);
	bodySphereTrigger->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnExitedInBodySphere);
}

void AEnemyCharacter::OnPerformAttack() {}

void AEnemyCharacter::OnPreAttack()
{
	leftHandTrigger->bGenerateOverlapEvents = 1;
	rightHandTrigger->bGenerateOverlapEvents = 1;
	
	FString message = TEXT(">>>>>>>>>>> Attack just going to start <<<<<<<<<<<< ");  
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, message);
}

void AEnemyCharacter::OnPostAttack() 
{
	IsAttacking = false;
	leftHandTrigger->bGenerateOverlapEvents = 0;  
	rightHandTrigger->bGenerateOverlapEvents = 0;
	
	FString message = TEXT(">>>>>>>>>>> Attack just finished <<<<<<<<<<<< ");  
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, message);
} 

void AEnemyCharacter::OnHandTriggerOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATheLastKompirCharacter* const _tempGladiator = Cast<ATheLastKompirCharacter>(OtherActor);  
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, "ATTACK made !");
	if (_tempGladiator) 
	{
		FString message = TEXT("=== HIT PLAYER WITH HAND ==== ");

		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, message);
		//in case it hit the player, it is good idea to disable the triggers, this way we'll make sure that the triggers will not over calculate with each single hit    
		leftHandTrigger->bGenerateOverlapEvents = 0;
		rightHandTrigger->bGenerateOverlapEvents = 0;
		_tempGladiator->OnChangeHealthByAmount(5.f);
	}
}

void AEnemyCharacter::OnEnteredInBodySphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ENEMY ATTACK NEEDS TO START !"));

	UBasicEnemyAnimInstance *AnimInstanceRef = Cast<UBasicEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstanceRef)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ENEMY ATTACK NEEDS TO START !"));
		AnimInstanceRef->Attack();
		//isAttacking = true;
	}
}

void AEnemyCharacter::OnExitedInBodySphere(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("ENEMY ATTACK NEEDS TO STOP TO EXECUTING"));
}

float AEnemyCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	/***********************/
	/*** Get Hit details ***/
	/***********************/
	FHitResult hitRes;
	FVector impulseDir;
	DamageEvent.GetBestHitInfo(this, DamageCauser, hitRes, impulseDir);
	impulseDir.Normalize();

	/*******************/
	/*** Camera Anim ***/
	/*******************/
	//TakingDamageDirection = impulseDir;
	//float takingDamagePct = FMath::Min(1.0f, Damage / (Stats->GetTakeDamageCameraAnim_MaxAdjustmentDamage()));
	//TakingDamageCameraAnimInterpolator->Init(true, Stats->GetTakeDamageCameraAnim_GoUpDuration(), Stats->GetTakeDamageCameraAnim_GoDownDuration(), 0.0f, takingDamagePct * Stats->GetTakeDamageCameraAnim_MaxOffset(), false, true);
	//TakingDamageCameraAnimInterpolator->GoUp();
	/*******************/
	TotalHealth -= Damage;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "IDI NAHUY " + FString::SanitizeFloat(Damage));
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}