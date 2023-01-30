// Fill out your copyright notice in the Description page of Project Settings.


#include "PracticePartner.h"

// Sets default values
APracticePartner::APracticePartner()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APracticePartner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APracticePartner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BallInRange())
	{
		if(SetSpike)
		{
			SetForSpike();
			SetSpike = false;
		}
		else
		{
			SetBackToPlayer();
			SetSpike = true;
		}
	}

	
}

// Called to bind functionality to input
void APracticePartner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APracticePartner::SetBackToPlayer()
{
	Ball->InitiateSet(Player);
}

void APracticePartner::SetForSpike()
{
	Ball->SetForSpike(Player);
}

bool APracticePartner::BallInRange()
{
	float Distance = FVector::Distance(GetActorLocation(), Ball->GetActorLocation());

	return Distance < 250.f;
}


