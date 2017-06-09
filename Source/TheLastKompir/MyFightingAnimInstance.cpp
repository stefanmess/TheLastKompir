// Fill out your copyright notice in the Description page of Project Settings.

#include "TheLastKompir.h"
#include "MyFightingAnimInstance.h"




void UMyFightingAnimInstance::UpdateMovementSpeed()
{
	//Get the pawn and if it's valid, update the movement speed to update
	//the movement animation
	APawn* Pawn = TryGetPawnOwner();

	if (Pawn)
	{
		MovementSpeed = Pawn->GetVelocity().Size();
	}
}

void UMyFightingAnimInstance::Attack()
{

	if (MeleeAttackMontage)
	{

		FName CurrentSection = Montage_GetCurrentSection(MeleeAttackMontage);

		//Determine which section is currently playing and jump to the next section
		//is possible

		if (CurrentSection.IsNone())
		{
			Montage_Play(MeleeAttackMontage);
		}
		else if (CurrentSection.IsEqual("FirstAttack") && bAcceptsSecondAttackInput)
		{
			bAcceptsSecondAttackInput = false;
			Montage_JumpToSection(FName("SecondAttack"), MeleeAttackMontage);
		}
		else if (CurrentSection.IsEqual("SecondAttack") && bAcceptsThirdAttackInput)
		{
			bAcceptsThirdAttackInput = false;
			//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.4);
			Montage_JumpToSection(FName("ThirdAttack"), MeleeAttackMontage);
		}

	}

}