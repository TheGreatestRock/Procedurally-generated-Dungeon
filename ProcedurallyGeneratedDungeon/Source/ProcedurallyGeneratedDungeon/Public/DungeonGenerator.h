// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterRoom.h"
#include "WallDoorActor.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

/**
 *  ADungeonGenerator
 *  Responsible for procedurally generating a dungeon with interconnected rooms,
 *  managing their placement, ensuring no overlap, and adding doors or walls to finalize the layout.
 */
UCLASS()
class GROUPE_7_PROJET_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGenerator();

	/** List of exit points for connecting rooms */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<USceneComponent*> ExitsList;

	/** List of door points for placing doors in the dungeon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<USceneComponent*> DoorsList;

	/** List of door actor classes for spawning doors */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<TSubclassOf<AActor>> DoorsActorList;

	/** List of base room classes for dungeon generation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<TSubclassOf<AMasterRoom>> BaseRoomsList;

	/** List of special room classes (e.g., treasure rooms, stairs, shops,...) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<TSubclassOf<AMasterRoom>> SpecialRoomsList;

	/** List of starting room classes for dungeon generation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<TSubclassOf<AMasterRoom>> StartRoomsList;

	/** List of boss room classes for dungeon generation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<TSubclassOf<AMasterRoom>> BossRoomsList;

	/** Number of rooms to generate in the dungeon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	int RoomAmount;

	/** Maximum time allowed for dungeon generation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	float MaxDungeonTime;

	/** Seed for random generation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	int Seed;

protected:
	/** List of all spawned rooms */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<TSubclassOf<AMasterRoom>> RoomsList;

	/** Pointer to the most recently spawned room */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	AMasterRoom* LatestRoom;

	/** List of available spawn points for the next room */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<USceneComponent*> SpawnPointsList;

	/** List of actors that are currently overlapping */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	TArray<AActor*> OverlappedList;
	
	/** Flag indicating whether dungeon generation is complete */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	bool DungeonComplete;

	/** Timer handle for managing timed events during dungeon generation */
	FTimerHandle TimerHandle;

	/** Currently selected exit point for spawning the next room */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeon Generator")
	USceneComponent* SelectedExitPoint;

public:
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Sets the random seed for procedural generation */
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void SetSeed();

	/** Sets the initial room for the dungeon */
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	bool SetStartRoom();

	// /** Processes wall doors in the current room to manage exits */
	// void ProcessRoomWallDoors(AMasterRoom* MasterRoom);
	
	/** Spawns the next room in the dungeon */
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	bool SpawnNextRoom();

	/** Spawns the boss room at the end of the dungeon */
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void SpawnBossRoom();

	/** Closes open holes in the dungeon by spawning walls */
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	void CloseHoles();

	/** Checks whether the latest room overlaps with existing actors */
	UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	bool IsOverlapping();

	/** Spawns doors at designated door points */
	//UFUNCTION(BlueprintCallable, Category = "Dungeon Generator")
	//void SpawnDoors();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Initializes and starts the dungeon generation process */
	void InitializeDungeonGeneration();

	/** Finalizes the dungeon generation process */
	void FinalizeDungeonGeneration();

	/** Attempts to spawn a room at the given exit point */
	AActor* TrySpawnRoomAtExit();
	void ConvertWallToDoor(UArrowComponent* ExitPoint);
	void ProcessRoomWalls(AMasterRoom* Room);

	/** Populates the RoomsList array from BaseRoomsList */
	void PopulateRoomsList(TArray<TSubclassOf<AMasterRoom>> ListOfRooms);
};
