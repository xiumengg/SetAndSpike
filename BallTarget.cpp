// Fill out your copyright notice in the Description page of Project Settings.


#include "BallTarget.h"

// Sets default values
ABallTarget::ABallTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	RootComponent = StaticMesh;

}

// Called when the game starts or when spawned
void ABallTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABallTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

