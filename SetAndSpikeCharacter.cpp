// Copyright Epic Games, Inc. All Rights Reserved.

#include "SetAndSpikeCharacter.h"

#include "DrawDebugHelpers.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//////////////////////////////////////////////////////////////////////////
// ASetAndSpikeCharacter

ASetAndSpikeCharacter::ASetAndSpikeCharacter()
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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	SpikePoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spike Point"));
	SpikePoint->SetupAttachment(GetMesh());

	SetFromPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Set From Point"));
	SetFromPoint->SetupAttachment(GetMesh());

	ActionQualityDisplay = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActionQualityDisplay"));
	ActionQualityDisplay->SetupAttachment(RootComponent);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASetAndSpikeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FillBar", IE_Pressed, this, &ASetAndSpikeCharacter::StartFillBar);
	PlayerInputComponent->BindAction("FillBar", IE_Released, this, &ASetAndSpikeCharacter::ResetBar);

	PlayerInputComponent->BindAction("Set", IE_Pressed, this, &ASetAndSpikeCharacter::Set);
	PlayerInputComponent->BindAction("Spike", IE_Pressed, this, &ASetAndSpikeCharacter::SpikeBall);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASetAndSpikeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASetAndSpikeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ASetAndSpikeCharacter::RotateSetTrajectory);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASetAndSpikeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ASetAndSpikeCharacter::TestMouseY);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASetAndSpikeCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASetAndSpikeCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASetAndSpikeCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASetAndSpikeCharacter::OnResetVR);
}


USceneComponent* ASetAndSpikeCharacter::GetSpikePoint()
{
	return SpikePoint;
}

void ASetAndSpikeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bAdvanceSetting)
	{
		AdvanceSetting();
	}

	if(bFillBar)
	{
		FillBar();
	}

	// UE_LOG(LogTemp, Warning, TEXT("%s"), *ActionQualityDisplay->GetComponentLocation().ToString());


	
	
	
}

void ASetAndSpikeCharacter::BeginPlay()
{
	Super::BeginPlay();

	bAdvanceSetting = true;

	if (ActionQualityDisplay)
	{
		ActionQuality = Cast<UActionQuality>(ActionQualityDisplay->GetUserWidgetObject());
		if(ActionQuality)
		{
			UE_LOG(LogTemp, Warning, TEXT("Got Progress bar"));
		}
	}
	
	
	if(PracticePartner)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASDas"));
		
	}

	if(Floor)
	{
		UE_LOG(LogTemp, Warning, TEXT("asdasdasdasdasd"));
		FloorPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(Floor->GetActorLocation(), FVector::UpVector);
		CalculateTossLocation();
	}
}

void ASetAndSpikeCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASetAndSpikeCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ASetAndSpikeCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ASetAndSpikeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASetAndSpikeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	
}

void ASetAndSpikeCharacter::MoveForward(float Value)
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

void ASetAndSpikeCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
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

void ASetAndSpikeCharacter::Set()
{
	FVector TossVelocity;
	
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,
		TossVelocity,
		SetFromPoint->GetComponentLocation(),
		TossLocation,
		0,
		0.35
	);
	
	Ball->SetActorLocation(SetFromPoint->GetComponentLocation());
	Ball->ApplyImpulse(TossVelocity);
}

void ASetAndSpikeCharacter::SpikeBall()
{
	
	
	Ball->Spike(PracticePartner);
}

void ASetAndSpikeCharacter::DrawSetTrajectory()
{
	FVector TossVelocity;
	
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,
		TossVelocity,
		SetFromPoint->GetComponentLocation(),
		TossLocation,
		0,
		0.35
	);

	FHitResult HitResult;
	TArray<FVector> OutPathPositions;
	FVector LastTraceDestination;
	const TArray<AActor*> ActorsToIgnore;

	UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(
		this,
		HitResult,
		OutPathPositions,
		LastTraceDestination,
		SetFromPoint->GetComponentLocation(),
		TossVelocity,
		true,
		10,
		ECollisionChannel::ECC_Camera,
		true,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		1
		);
}

void ASetAndSpikeCharacter::DrawTossLocation()
{
	
	DrawDebugSphere(
		GetWorld(),
		TossLocation,
		50,
		1,
		FColor::Purple,
		false
		);
}

void ASetAndSpikeCharacter::CalculateTossLocation()
{
	FVector SetFromPointProjection = FVector::PointPlaneProject(SetFromPoint->GetComponentLocation(), FloorPlane);

	FVector ActorProjection = FVector::PointPlaneProject(GetActorLocation(), FloorPlane);

	FVector TossLine = SetFromPointProjection - ActorProjection;

	FVector UnrotatedTossLocation = (SetDistanceMultiplier  * TossLine) + SetFromPointProjection;

	FVector Radius = FVector(0, FVector::Distance(ActorProjection, UnrotatedTossLocation) , 0);

	FVector RotateValue = Radius.RotateAngleAxis(CurrAngle, FVector (0,0,1));

	TossLocation = ActorProjection + RotateValue;
}

void ASetAndSpikeCharacter::RotateTossLocation(float DeltaSeconds, float Rate)
{
	CurrAngle += (SetRotateSensitivity * Rate * DeltaSeconds);
	
	if(CurrAngle > 360.f)
	{
		CurrAngle = 0.f;
	}

	// FRotator CurrRotation = FRotator {0,CurrAngle,0};
	//
	FVector SetFromPointProjection = FVector::PointPlaneProject(SetFromPoint->GetComponentLocation(), FloorPlane);

	FVector ActorProjection = FVector::PointPlaneProject(GetActorLocation(), FloorPlane);

	FVector TossLine = SetFromPointProjection - ActorProjection;

	FVector ExtendedVector = (5 * TossLine) + SetFromPointProjection;

	FVector Radius = FVector(0, FVector::Distance(ActorProjection, TossLocation) , 0);

	FVector RotateValue = Radius.RotateAngleAxis(CurrAngle, FVector (0,0,1));

	FVector TurnedVector = ActorProjection + RotateValue;

	TurnedVector.X += RotateValue.X;
	TurnedVector.Y += RotateValue.Y;
	TurnedVector.Z += RotateValue.Z;
	//
	// DrawDebugSphere(
	// 	GetWorld(),
	// 	SetFromPointProjection,
	// 	50,
	// 	1,
	// 	FColor::Black,
	// 	false
	// );
	//
	// FVector Normal = SetFromPointProjection - SetFromPoint->GetComponentLocation();

	DrawDebugSphere(
		GetWorld(),
		SetFromPointProjection,
		50,
		1,
		FColor::Red,
		false
	);

	DrawDebugSphere(
		GetWorld(),
		ActorProjection,
		50,
		1,
		FColor::Green,
		false
	);

	DrawDebugSphere(
		GetWorld(),
		ExtendedVector,
		50,
		1,
		FColor::Orange,
		false
	);

	DrawDebugSphere(
		GetWorld(),
		TurnedVector,
		50,
		1,
		FColor::Black,
		false
	);


	// TossLocation = UKismetMathLibrary::GreaterGreater_VectorRotator(TossLocation, CurrRotation);
	
	
	// FVector RotationRadius = TossLocation - SetFromPointProjection;
	//
	// FVector RotateValue = RotationRadius.RotateAngleAxis(CurrAngle, FVector::UpVector);
	//
	// TossLocation = TossLocation + RotateValue;
	//
	// UE_LOG(LogTemp, Warning, TEXT("TurnedVector: %s"), *TurnedVector.ToString());
	
}

void ASetAndSpikeCharacter::AdvanceSetting()
{
	CalculateTossLocation();

	DrawTossLocation();

	DrawSetTrajectory();
}

void ASetAndSpikeCharacter::FillBar()
{
	if (ActionQualityPercent < 1.05)
	{
		ActionQuality->SetActionQualityBarPercent(ActionQualityPercent);
		ActionQualityPercent += 0.05;
	}
}

void ASetAndSpikeCharacter::StartFillBar()
{
	FVector FloorEnd;
	float TimeLeft;
	
	if(Ball->CanSetBall(this, FloorEnd, TimeLeft))
	{
		UE_LOG(LogTemp, Warning, TEXT("Greed is good"));
	}
	bFillBar = true;
}

void ASetAndSpikeCharacter::ResetBar()
{
	bFillBar = false;
	ActionQualityPercent = 0;
	ActionQuality->SetActionQualityBarPercent(ActionQualityPercent);
}


void ASetAndSpikeCharacter::TestMouseY(float Rate)
{
	if (SetDistanceMultiplier - Rate > 0)
	{
		SetDistanceMultiplier = SetDistanceMultiplier - Rate * UGameplayStatics::GetWorldDeltaSeconds(this) * SetDistanceSensitivity;
	}
	
	
}

void ASetAndSpikeCharacter::RotateSetTrajectory(float Rate)
{

	
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

	RotateTossLocation(DeltaTime, Rate);
	
	

	
}

