// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastKompir.h"
#include "BasicEnemyAnimInstance.h"




void UBasicEnemyAnimInstance::UpdateMovementSpeed()
{
	//Get the pawn and if it's valid, update the movement speed to update
	//the movement animation
	APawn* Pawn = TryGetPawnOwner();

	if (Pawn)
	{
		MovementSpeed = Pawn->GetVelocity().Size();
	}
}

void UBasicEnemyAnimInstance::Attack()
{

	if (BasicEnemyAttackMontage)
	{

		FName CurrentSection = Montage_GetCurrentSection(BasicEnemyAttackMontage);

		if (CurrentSection.IsNone())
		{
			Montage_Play(BasicEnemyAttackMontage);
		}

	}

}