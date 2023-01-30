// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "SetAndSpikeCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"

// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	RootComponent = StaticMesh;

	StaticMesh->SetConstraintMode(EDOFMode::None);

}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector TossVelocity;

	// UGameplayStatics::BlueprintSuggestProjectileVelocity(
	// 	this,
	// 	TossVelocity,
	// 	GetActorLocation(),
	// 	BallTarget->GetActorLocation(),
	// 	0,
	// 	0,
	// 	ESuggestProjVelocityTraceOption::TraceFullPath,
	// 	10.f,
	// 	true,
	// 	true
	// );
	//
	// UGameplayStatics::SuggestProjectileVelocity_CustomArc(
	// 	this,
	// 	TossVelocity,
	// 	GetActorLocation(),
	// 	BallTarget->GetActorLocation()
	// );

	FHitResult HitResult;
	TArray<FVector> OutPathPositions;
	FVector LastTraceDestination;
	const TArray<AActor*> ActorsToIgnore;

	UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(
		this,
		HitResult,
		OutPathPositions,
		LastTraceDestination,
		GetActorLocation(),
		GetVelocity(),
		true,
		10,
		ECollisionChannel::ECC_WorldDynamic,
		true,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		1
		);
	
	DrawDebugLine(
		GetWorld(),
		GetActorLocation(),
		Target->GetActorLocation(),
		FColor::Red,
		false
	);

	// UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *GetVelocity().ToString());

	// UE_LOG(LogTemp, Warning, TEXT("%s"), *BallTarget->GetActorLocation().ToString());

}

void ABall::InitiateSet(ACharacter* Targeted)
{
	Target = Targeted;
	StaticMesh->PutAllRigidBodiesToSleep();
	// StaticMesh->SetConstraintMode(EDOFMode::Default);
	StaticMesh->SetConstraintMode(EDOFMode::None);
	// StaticMesh->AddImpulse(GetVelocity() * -1 * StaticMesh->GetMass());
	UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *GetVelocity().ToString());
	SetUsingSuggestProjectileVelocity();
}

void ABall::Stop()
{
	StaticMesh->PutAllRigidBodiesToSleep();
	// StaticMesh->SetConstraintMode(EDOFMode::Default);
	// StaticMesh->AddImpulse(GetVelocity() * -1 * StaticMesh->GetMass());
	// StaticMesh->SetConstraintMode(EDOFMode::None);
	UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *GetVelocity().ToString());
}

void ABall::Spike(ACharacter* Targeted)
{
	Target = Targeted;

	FVector SpikeDirection = Target->GetActorLocation() - GetActorLocation();

	StaticMesh->AddImpulse(SpikeDirection * 45);
}

void ABall::ApplyImpulse(FVector TossVelocity)
{
	StaticMesh->PutAllRigidBodiesToSleep();
	StaticMesh->SetConstraintMode(EDOFMode::None);
	StaticMesh->AddImpulse(TossVelocity * StaticMesh->GetMass());
}

void ABall::SetForSpike(ACharacter* Targeted)
{
	Target = Targeted;

	StaticMesh->PutAllRigidBodiesToSleep();
	// StaticMesh->SetConstraintMode(EDOFMode::Default);
	StaticMesh->SetConstraintMode(EDOFMode::None);

	FVector TossVelocity;

	ASetAndSpikeCharacter* SpikingTarget = Cast<ASetAndSpikeCharacter>(Targeted);

	if(SpikingTarget)
	{
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,
		TossVelocity,
		GetActorLocation(),
		SpikingTarget->GetSpikePoint()->GetComponentLocation(),
		0,
		0.5
	);

		bBeingSet = true;

		StaticMesh->AddImpulse(TossVelocity * 10);
	}
	
}

void ABall::SetUsingSuggestProjectileVelocity()
{
	FVector TossVelocity;
	

	UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,
		TossVelocity,
		GetActorLocation(),
		Target->GetActorLocation(),
		0,
		0.5
	);

	bBeingSet = true;

	FVector SetDirection = Target->GetActorLocation() - GetActorLocation();
	
	UE_LOG(LogTemp, Warning, TEXT("Impulse: %s"), *(SetDirection * 10).ToString());

	StaticMesh->AddImpulse(TossVelocity * 10);

	
}
