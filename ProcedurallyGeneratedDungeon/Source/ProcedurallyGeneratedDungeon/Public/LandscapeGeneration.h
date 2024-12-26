// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "LandscapeGeneration.generated.h"

UCLASS()
class GROUPE_7_PROJET_API ALandscapeGeneration : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALandscapeGeneration();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int XVertex = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int YVertex = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Size = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int NbrSectionX = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int NbrSectionY = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SectionIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool randomizeMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D PerlinOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MountainHeight = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoulderHeight = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MountainScale = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoulderScale = 1000;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UProceduralMeshComponent* ProceduralMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInterface* TerrainMaterial = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool GeneratorBusy = false;

	UPROPERTY(BlueprintReadOnly)
	bool TileDataReady = false;

	UPROPERTY(BlueprintReadWrite)
	TMap<FIntPoint, int> QueuedTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TileReplaceableDistance;

	int SectionIndexX = 0;
	int SectionIndexY = 0;

	TArray<int32> Triangles;

	TArray<FVector> SubVerticles;
	TArray<FVector2D> SubUvs;
	TArray<int32> SubTriangles;
	TArray<FVector> SubNormals;
	TArray<FProcMeshTangent> SubTangents;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CreateTriangles();

	void CreateVerticlesAndUVs(const int SectionIndexX, const int SectionIndexY, TArray<FVector>& Verticles, TArray<FVector2D>& Uvs);

	void CreateSubsets(const TArray<FVector>& Verticles, const TArray<FVector2D>& Uvs, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void GenerateMap(const int InSectionIndexX, const int InSectionIndexY);

	UFUNCTION(BlueprintCallable)
	void GenerateMapAsync(const int InSectionIndexX, const int InSectionIndexY);

	UFUNCTION(BlueprintCallable)
	void DrawTile();

	UFUNCTION(BlueprintCallable)
	FVector GetPlayerLocation();

	UFUNCTION(BlueprintCallable)
	FVector2D GetTileLocation(FIntPoint TileCoordinate);

	UFUNCTION(BlueprintCallable)
	FIntPoint GetClosestQueuedTile();

	UFUNCTION(BlueprintCallable)
	int GetFurthestUpdateableTile();

	UFUNCTION(BlueprintCallable)
	float GetMaxSubVerticlesHeight();

	float GetHeight(const FVector2D Location);
	float PerLinNoiseExtended(const FVector2D Location, const float Scale, const float Amplitude, const double XOffset, const double YOffset);
};

class FAsyncLandscapeGeneration : public FNonAbandonableTask {
public:
	FAsyncLandscapeGeneration(ALandscapeGeneration* InLandScapeGenerator) : LandScapeGenerator(InLandScapeGenerator) {}
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncLandscapeGeneration, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork();
private:
	ALandscapeGeneration* LandScapeGenerator;
};