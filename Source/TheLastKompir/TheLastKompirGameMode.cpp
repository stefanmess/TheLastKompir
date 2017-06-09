// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TheLastKompir.h"
#include "TheLastKompirGameMode.h"
#include "TheLastKompirCharacter.h"
#include "Blueprint/UserWidget.h"

void ATheLastKompirGameMode::BeginPlay()
{
	Super::BeginPlay();

	ATheLastKompirCharacter* MyCharacter = Cast<ATheLastKompirCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerHUDClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);

		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

ATheLastKompirGameMode::ATheLastKompirGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
