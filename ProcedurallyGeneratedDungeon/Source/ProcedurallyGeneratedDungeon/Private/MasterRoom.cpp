// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterRoom.h"

#include "Components/BoxComponent.h"

// Sets default values
AMasterRoom::AMasterRoom()
{
	// Set this actor to call Tick() every frame. Can turn this off for performance optimization.
	PrimaryActorTick.bCanEverTick = true;

	// Create the root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Initialize and attach components to their intended parents
	GeometryFolder = CreateDefaultSubobject<USceneComponent>(TEXT("GeometryFolder"));
	GeometryFolder->SetupAttachment(RootComponent); // Attach to the root component

	WallOrDoorFolder = CreateDefaultSubobject<USceneComponent>(TEXT("DoorsFolder"));
	WallOrDoorFolder->SetupAttachment(GeometryFolder); // Attach to GeometryFolder

	SpawnPointsFolder = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPointsFolder"));
	SpawnPointsFolder->SetupAttachment(RootComponent); // Attach to the root component

	ExitsFolder = CreateDefaultSubobject<USceneComponent>(TEXT("ExitsFolder"));
	ExitsFolder->SetupAttachment(RootComponent); // Attach to the root component

	OverlapFolder = CreateDefaultSubobject<USceneComponent>(TEXT("OverlapFolder"));
	OverlapFolder->SetupAttachment(RootComponent); // Attach to the root component

	Objects = CreateDefaultSubobject<USceneComponent>(TEXT("Objects"));
	Objects->SetupAttachment(RootComponent); // Attach to the root component

	Lightsources = CreateDefaultSubobject<USceneComponent>(TEXT("Lightsources"));
	Lightsources->SetupAttachment(Objects); // Attach to Objects

	// Initialize the BaseOverlapCollision and attach it to OverlapFolder
	BaseOverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BaseOverlapCollision"));
	BaseOverlapCollision->SetupAttachment(OverlapFolder);

	// ArrowComponent is not attached; attach it to the root or a suitable parent
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent); // Attach to the root component
}


// Called when the game starts or when spawned
void AMasterRoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMasterRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

