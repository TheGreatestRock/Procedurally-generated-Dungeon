// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "WallDoorActor.generated.h"

UCLASS()
class GROUPE_7_PROJET_API AWallDoorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallDoorActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void ConvertToDoor();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallDoorActor")
	bool IsDoor;
	
	//void SetSeed(int32 NewSeed);
	//void RandomizeWallOrDoor();

public:
	//mesh door
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallDoorActor")
	UStaticMeshComponent* DoorMesh;

	//mesh wall
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallDoorActor")
	UStaticMeshComponent* WallMesh;

	int32 Seed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallDoorActor")
	UArrowComponent* ArrowComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallDoorActor")
	UBoxComponent* BoxComp;
};
