// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallTarget.h"
#include "Ball.generated.h"

UCLASS()
class SETANDSPIKE_API ABall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitiateSet(ACharacter* Target);

	void Stop();

	void Spike(ACharacter* Targeted);

	void ApplyImpulse(FVector TossVelocity);

	void SetForSpike(ACharacter* Targeted);


private:

	UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	ACharacter* Target;
	
	bool bBeingSet = false;

	void SetUsingSuggestProjectileVelocity();

};
