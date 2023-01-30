// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ball.h"
#include "PracticePartner.generated.h"

UCLASS()
class SETANDSPIKE_API APracticePartner : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APracticePartner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	ACharacter* Player;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	ABall* Ball;

	void SetBackToPlayer();

	void SetForSpike();
	
	bool BallInRange();

	bool SetSpike = false;

};
