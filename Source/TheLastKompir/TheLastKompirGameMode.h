// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "TheLastKompirGameMode.generated.h"

UCLASS(minimalapi)
class ATheLastKompirGameMode : public AGameMode
{
	GENERATED_BODY()

		virtual void BeginPlay() override; // Override beginplay from the base class

public:
	ATheLastKompirGameMode();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", Meta = (BlueprintProtected = " true"))
		TSubclassOf<class UUserWidget> PlayerHUDClass;

	UPROPERTY()
		class UUserWidget* CurrentWidget;
};



