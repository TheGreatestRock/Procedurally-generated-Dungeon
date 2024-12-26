// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "MasterRoom.generated.h"

UCLASS()
class GROUPE_7_PROJET_API AMasterRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasterRoom();
	//GeometryFolder
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Master Room")
	USceneComponent* GeometryFolder;
	// DoorsFolderWallOrDoorFolder
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Master Room")
	USceneComponent* WallOrDoorFolder;
	//SpawnPointsFolder
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Master Room")
	USceneComponent* SpawnPointsFolder;
	//ExitsFolder
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Master Room")
	USceneComponent* ExitsFolder;
	//OverlapFolder
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Master Room")
	USceneComponent* OverlapFolder;
	//Objects
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Master Room")
	USceneComponent* Objects;
	//	Lightsources
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Master Room")
	USceneComponent* Lightsources;

	//BaseOverlapCollision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Master Room")
	UBoxComponent* BaseOverlapCollision;

	//ArrowComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Master Room")
	UArrowComponent* ArrowComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
