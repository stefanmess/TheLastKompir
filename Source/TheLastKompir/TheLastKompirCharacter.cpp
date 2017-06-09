// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TheLastKompir.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Perception/PawnSensingComponent.h"
#include "MyFightingAnimInstance.h"
#include "TheLastKompirCharacter.h"
#include "Engine.h"

//////////////////////////////////////////////////////////////////////////
// ATheLastKompirCharacter

ATheLastKompirCharacter::ATheLastKompirCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//GetCharacterMovement()->MaxWalkSpeed = 100;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	PawnNoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitterComp"));

	//------------------------------------------------------------------------------------------------------------//

	// Stamina Inputs
	InitialStamina = 100.f;
	CurrentStamina = InitialStamina;

	// Health Inputs
	InitialHealth = 100.f;
	CurrentHealth = 60.f;

	// Attributes
	Strenght = 0;
	Agility = 0;
	Intelegence = 0;

	// Alive - Attack - Weapon inputs
	IsStillAlive = true;
	IsAttacking = false;
	WeaponIndex = 1;

	//------------------------------------------------------------------------------------------------------------//

	//WeponCollision = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(TEXT("WeponCollision")));
	WeponCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WeponCollision"));
	WeponCollision->AttachTo(GetMesh(), TEXT("hand_r"), EAttachLocation::SnapToTargetIncludingScale, true);
	WeponCollision->SetCapsuleRadius(15.f);
	WeponCollision->SetCapsuleHalfHeight(30.f);
	WeponCollision->SetRelativeRotation(FRotator(28.f, -20.f, 0.f));
	// za setiranje na lokacija x oskata e za visina y e za napred nazad z e za ?
	WeponCollision->SetRelativeLocation(FVector(-25.f, 15.f, 10.f));
	WeponCollision->bGenerateOverlapEvents = 0;

	WeponCollision->OnComponentBeginOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHit);
	WeponCollision->OnComponentEndOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHitEnd);

	RightElbowCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightElbowCollision"));
	RightElbowCollision->SetSphereRadius(15.f);
	RightElbowCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "lowerarm_r");
	RightElbowCollision->bGenerateOverlapEvents = 0;

	LeftElbowCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftElbowCollision"));
	LeftElbowCollision->SetSphereRadius(15.f);
	LeftElbowCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "lowerarm_l");
	LeftElbowCollision->bGenerateOverlapEvents = 0;

	RightFootCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightFootCollision"));
	RightFootCollision->SetSphereRadius(20.f);
	RightFootCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "foot_r");
	RightFootCollision->bGenerateOverlapEvents = 0;

	LeftFootCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftFootCollision"));
	LeftFootCollision->SetSphereRadius(20.f);
	LeftFootCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "foot_l");
	LeftFootCollision->bGenerateOverlapEvents = 0;

}

//ATheLastKompirCharacter::ATheLastKompirCharacter(const FObjectInitializer objectInitializer):Super(objectInitializer)
//{
//	
//}

void ATheLastKompirCharacter::ReportNoise(USoundBase* SoundToPlay, float Volume)
{
	//If we have a valid sound to play, play the sound and
	//report it to our game
	if (SoundToPlay)
	{
		//Play the actual sound
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, GetActorLocation(), Volume);

		//Report that we've played a sound with a certain volume in a specific location
		MakeNoise(Volume, this, GetActorLocation());
	}

}



void ATheLastKompirCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATheLastKompirCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATheLastKompirCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATheLastKompirCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATheLastKompirCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATheLastKompirCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATheLastKompirCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATheLastKompirCharacter::OnResetVR);

	// fighting input devices
	InputComponent->BindAction("PerformMeleeHit", IE_Pressed, this, &ATheLastKompirCharacter::Attack);

	WeponCollision->OnComponentBeginOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHit);
	WeponCollision->OnComponentEndOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHitEnd);
}


void ATheLastKompirCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATheLastKompirCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ATheLastKompirCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ATheLastKompirCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATheLastKompirCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATheLastKompirCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATheLastKompirCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


//----------------------------------------------------------------------------------------------------------------------//

void ATheLastKompirCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCurrentStamina(-DeltaTime * 0.01f * (InitialStamina));

	//UE_LOG(LogTemp, Warning, TEXT("MyCharacter's Health is %f"), GetCurrentHealth());
}

void ATheLastKompirCharacter::BeginPlay()
{
	Super::BeginPlay();
	//CurrentHealth = InitialHealth;

	WeponCollision->OnComponentBeginOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHit);
	WeponCollision->OnComponentEndOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHitEnd);
}


void ATheLastKompirCharacter::Attack()
{
	//Get a reference to our custom anim instance and tell it to update our character's animation
	UMyFightingAnimInstance *AnimInstanceRef = Cast<UMyFightingAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstanceRef)
	{
		AnimInstanceRef->Attack();
		//isAttacking = true;
	}
}

//-----------------------------------------------------------------------------------------------------------------------
// Defining the Stamina Fuctions																						-
//-----------------------------------------------------------------------------------------------------------------------

float ATheLastKompirCharacter::GetInitialStamina()
{
	return InitialStamina;
}

float ATheLastKompirCharacter::GetCurrentStamina()
{
	return CurrentStamina;
}

void ATheLastKompirCharacter::UpdateCurrentStamina(float Stamina)
{
	CurrentStamina = CurrentStamina + Stamina;
}

//-----------------------------------------------------------------------------------------------------------------------
// Defining the Health Fuctions																							-
//-----------------------------------------------------------------------------------------------------------------------

float ATheLastKompirCharacter::GetInitialHealth()
{
	return InitialHealth;
}

float ATheLastKompirCharacter::GetCurrentHealth()
{
	return CurrentHealth;
}


//-----------------------------------------------------------------------------------------------------------------------
// Health packs																											-
//-----------------------------------------------------------------------------------------------------------------------
void ATheLastKompirCharacter::AddHealthFromPickUp(float Amount)
{
	if (Amount > 0.f)
	{
		if (CurrentHealth <= InitialHealth)
		{
			CurrentHealth = CurrentHealth + Amount;
			//UE_LOG(LogTemp, Error, TEXT("MyCharacter's Health is %f"), CurrentHealth);
			if (CurrentHealth > InitialHealth)
			{
				CurrentHealth = InitialHealth;
				//UE_LOG(LogTemp, Error, TEXT("MyCharacter's Health is %f"), CurrentHealth);
			}
		}
	}
}


void ATheLastKompirCharacter::OnChangeHealthByAmount(float usedAmount)
{
	if (CurrentHealth <= 0.f)
		IsStillAlive = false;

	CurrentHealth -= usedAmount;
	FOutputDeviceNull ar;
	this->CallFunctionByNameWithArguments(TEXT("ApplyGetDamageEffect"), ar, NULL, true);
}

//-----------------------------------------------------------------------------------------------------------------------
// Check if the player is alive																							-
//-----------------------------------------------------------------------------------------------------------------------
bool ATheLastKompirCharacter::IsAlive()
{
	if (CurrentHealth <= 0.f)
	{
		return false;
	}
	return true;

}

//-----------------------------------------------------------------------------------------------------------------------
// Check if player is attacking																						-
//-----------------------------------------------------------------------------------------------------------------------
void ATheLastKompirCharacter::OnAttack()
{
	if (IsControlable)
	{
		IsAttacking = true;
	}
}

void ATheLastKompirCharacter::OnPostAttack()
{
	IsAttacking = false;
}

float ATheLastKompirCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent,
	class AController * EventInstigator, AActor * DamageCauser)
{
	float ActualDemage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDemage > 0.f) {
		CurrentHealth = CurrentHealth - ActualDemage;
		if (CurrentHealth <= 0.f)
		{
			//DeathLogic
		}
	}


	return CurrentHealth;
}

void ATheLastKompirCharacter::OnChangeWeapon()
{
	if (IsControlable)
	{
		// wepon change logic 
		// ... in Development still ...

		/*if (WeaponIndex < TablesInstance->AllWeaponsData.Num())
		{
			WeaponIndex++;
		} else {
			WeaponIndex = 1;
		} */
	}
}


void ATheLastKompirCharacter::onTimerEnd()
{

}

void ATheLastKompirCharacter::OnHit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("ATTACK"));
	GLog->Log("HIT MADE");
	UE_LOG(LogTemp, Error, TEXT("HIT MADE !"));

	if (OtherActor != nullptr)
	{
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);
		const float DamageAmount = 60.0f;
		OtherActor->TakeDamage(DamageAmount, DamageEvent, GetController(), this);
	}
	//UGameplayStatics::ApplyDamage(OtherActor,20.f,this,this,); 
}

void ATheLastKompirCharacter::OnHitEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GLog->Log("HIT ENDED");
	UE_LOG(LogTemp, Error, TEXT("HIT END !"));
}

void ATheLastKompirCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WeponCollision->OnComponentBeginOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHit);
	WeponCollision->OnComponentEndOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHitEnd);

	/*RightElbowCollision->OnComponentBeginOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHit);
	RightElbowCollision->OnComponentEndOverlap.AddDynamic(this, &ATheLastKompirCharacter::OnHitEnd);*/
}
