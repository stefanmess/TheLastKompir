// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "TheLastKompirCharacter.generated.h"

UCLASS(config=Game)
class ATheLastKompirCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	FTimerHandle SlowDownTimerHandle;

	UCapsuleComponent* WeponCollision;
	USphereComponent* RightElbowCollision;
	USphereComponent* LeftElbowCollision;
	USphereComponent* RightFootCollision;
	USphereComponent* LeftFootCollision;

	UFUNCTION()
		void OnHit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnHitEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

//---------------------------------------HUD Interface logic-------------------------------------------------
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface



	//-----------------------------------------------------------------------------------------------------------------------
	// Defining current stamina and initial stamina																			-
	//-----------------------------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Stamina")
		float InitialStamina;

	UPROPERTY(EditAnywhere, Category = "Stamina")
		float CurrentStamina;

	//-----------------------------------------------------------------------------------------------------------------------
	// Defining current health and initial health																			-
	//-----------------------------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Health")
		float InitialHealth;

	UPROPERTY(EditAnywhere, Category = "Health")
		float CurrentHealth;

	//-----------------------------------------------------------------------------------------------------------------------
	// Attributes																											-
	//-----------------------------------------------------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Attributes")
		int Strenght;

	UPROPERTY(EditAnywhere, Category = "Attributes")
		int Agility;

	UPROPERTY(EditAnywhere, Category = "Attributes")
		int Intelegence;

public:

	ATheLastKompirCharacter();

	//ATheLastKompirCharacter(const FObjectInitializer objectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	bool IsAlive();

	/*The function that is going to play the sound and report it to our game*/
	UFUNCTION(BlueprintCallable, Category = AI)
		void ReportNoise(USoundBase* SoundToPlay, float Volume);

	/*A Pawn Noise Emitter component which is used in order to emit the sounds to nearby AIs*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UPawnNoiseEmitterComponent* PawnNoiseEmitterComp;


	//-----------------------------------fighting system props-------------------------------------------------

	//Is the player dead or not 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Attributes") 
		bool IsStillAlive; 

	//is the player attacking right now? 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Attributes")
		bool IsAttacking;

	//the index of the current active weapon. 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Attributes") 
		int32 WeaponIndex;

	//To be able to disable the player during cutscenes, menus, death.... etc 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Attributes") 
		bool IsControlable; 
	
	//Return if the player dead or alive 
	UFUNCTION(BlueprintCallable, Category = "Player Attributes") 
		bool GetIsStillAlive() const { return IsStillAlive; } 

	//Enable or disable inputs 
	/*UFUNCTION(BlueprintCallable, Category = "Player Attributes") 
		void OnSetPlayerController(bool status); */

	UFUNCTION(BlueprintCallable, Category = "Player Actions") 
		void OnPostAttack();

	UFUNCTION(BlueprintCallable, Category = "Player Actions") 
		void OnAttack();

	UFUNCTION(BlueprintCallable, Category = "Player Actions") 
		void OnChangeWeapon();

	//UFUNCTION(BlueprintCallable, Category = "Fighting")
	void Attack();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent,
		class AController * EventInstigator, AActor * DamageCauser) override;

	// Function to access the stamina //

	UFUNCTION(BlueprintPure, Category = "Stamina")
		float GetInitialStamina();

	UFUNCTION(BlueprintPure, Category = "Stamina")
		float GetCurrentStamina();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
		void UpdateCurrentStamina(float Stamina);

	// Function to access the health //

	UFUNCTION(BlueprintPure, Category = "Health")
		float GetInitialHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
		float GetCurrentHealth();

	UFUNCTION(BlueprintCallable, Category = "Health")
		void AddHealthFromPickUp(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
		void OnChangeHealthByAmount(float Amount);


	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION()
		void onTimerEnd();

	//Override the PostInitializeComponents() 
	virtual void PostInitializeComponents() override;

};

