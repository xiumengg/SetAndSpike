// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ball.h"
#include "PracticePartner.h"
#include "Engine/StaticMeshActor.h"
#include "SetAndSpikeCharacter.generated.h"

UCLASS(config=Game)
class ASetAndSpikeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ASetAndSpikeCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	USceneComponent* GetSpikePoint();

protected:

	/** Resets HMD orientation in VR. */
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
	
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

	void Set();

	void SpikeBall();

	void TestMouseY(float Rate);

	void RotateSetTrajectory(float Rate);
	
	void DrawSetTrajectory();

	void DrawTossLocation();

	void CalculateTossLocation();

	void RotateTossLocation(float DeltaSeconds, float Rate);

	void AdvanceSetting();

	bool bAdvanceSetting = true;


	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float SetDistanceSensitivity = 1000.f;

	float SetDistanceMultiplier = 2.f;

	float CurrAngle = 0.f;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float SetRotateSensitivity = 50.f;

	FVector TossLocation;

	UPROPERTY(EditAnywhere, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	ACharacter* PracticePartner;

	UPROPERTY(EditAnywhere, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	ABall* Ball;

	UPROPERTY(EditAnywhere, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SpikePoint;

	UPROPERTY(EditAnywhere, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SetFromPoint;

	UPROPERTY(EditAnywhere, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	AStaticMeshActor* Floor;

	FPlane FloorPlane;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

