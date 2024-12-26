// Fill out your copyright notice in the Description page of Project Settings.


#include "LandscapeGeneration.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values
ALandscapeGeneration::ALandscapeGeneration()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
	TileReplaceableDistance = Size / 4 * (NbrSectionX + NbrSectionY) * (XVertex + YVertex);
}

// Called when the game starts or when spawned
void ALandscapeGeneration::BeginPlay()
{
	Super::BeginPlay();

	if (randomizeMap)
	{
		PerlinOffset = FVector2D(FMath::RandRange(0.f, 5000.f), FMath::RandRange(0.f, 5000.f));
		MountainHeight = MountainHeight * FMath::FRandRange(.1f, 3.f);
		BoulderHeight = BoulderHeight * FMath::FRandRange(.1f, 2.f);
		MountainScale = MountainScale * FMath::FRandRange(.4f, 2.f);
		BoulderScale = BoulderScale * FMath::FRandRange(.4f, 2.f);
	}

}

// Called every frame
void ALandscapeGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALandscapeGeneration::GenerateMap(const int InSectionIndexX, const int InSectionIndexY)
{
	TArray<FVector> Verticles;
	TArray<FVector2D> Uvs;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;

	CreateVerticlesAndUVs(InSectionIndexX, InSectionIndexY, Verticles, Uvs);

	if (Triangles.IsEmpty()) {
		CreateTriangles();
	}

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Verticles, Triangles, Uvs, Normals, Tangents);

	CreateSubsets(Verticles, Uvs, Normals, Tangents);
}

void ALandscapeGeneration::GenerateMapAsync(const int InSectionIndexX, const int InSectionIndexY)
{
	GeneratorBusy = true;
	SectionIndexX = InSectionIndexX;
	SectionIndexY = InSectionIndexY;
	QueuedTiles.Add(FIntPoint(InSectionIndexX, InSectionIndexY), SectionIndex);

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&]()
		{
			auto LandscapeGenTask = new FAsyncTask<FAsyncLandscapeGeneration>(this);
			LandscapeGenTask->StartBackgroundTask();
			LandscapeGenTask->EnsureCompletion();
			delete LandscapeGenTask;
		});
}

void ALandscapeGeneration::DrawTile()
{
	TileDataReady = false;
	int FurthestTileIndex = GetFurthestUpdateableTile();
	if (FurthestTileIndex > -1) {
		TArray<int> ValueArray;
		TArray<FIntPoint> KeyArray;
		QueuedTiles.GenerateKeyArray(KeyArray);
		QueuedTiles.GenerateValueArray(ValueArray);
		int repleaceableSectionIndex = ValueArray[FurthestTileIndex];

		ProceduralMesh->UpdateMeshSection(repleaceableSectionIndex, SubVerticles, SubNormals, SubUvs, TArray<FColor>(), SubTangents);
		QueuedTiles.Add(FIntPoint(SectionIndexX, SectionIndexY), repleaceableSectionIndex);
		QueuedTiles.Remove(KeyArray[FurthestTileIndex]);
	}
	else {
		ProceduralMesh->CreateMeshSection(SectionIndex, SubVerticles, SubTriangles, SubNormals, SubUvs, TArray<FColor>(), SubTangents, true);

		if (TerrainMaterial)
			ProceduralMesh->SetMaterial(SectionIndex, TerrainMaterial);

		++SectionIndex;
	}

	SubVerticles.Empty();
	SubNormals.Empty();
	SubTriangles.Empty();
	SubUvs.Empty();
	SubTangents.Empty();

	GeneratorBusy = false;
}

FVector ALandscapeGeneration::GetPlayerLocation()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (Player) {
		return Player->GetActorLocation();
	}

	return FVector();
}

FVector2D ALandscapeGeneration::GetTileLocation(FIntPoint TileCoordinate)
{
	return FVector2D(TileCoordinate * FIntPoint(XVertex - 1, YVertex - 1) * Size) + FVector2D(XVertex - 1, YVertex - 1) * Size / 2;
}

FIntPoint ALandscapeGeneration::GetClosestQueuedTile()
{
	float ClosestDistance = TNumericLimits<float>::Max();
	FIntPoint ClosestTile;
	for (const auto& Entry : QueuedTiles)
	{
		const FIntPoint& Key = Entry.Key;
		int Value = Entry.Value;

		if (Value == -1) {
			float Distance = FVector2D::Distance(GetTileLocation(Key), FVector2D(GetPlayerLocation()));

			if (Distance < ClosestDistance) {
				ClosestDistance = Distance;
				ClosestTile = Key;
			}
		}
	}

	return ClosestTile;
}

int ALandscapeGeneration::GetFurthestUpdateableTile()
{
	float FurthestDistance = -1;
	int FurthestTileIndex = -1;
	int CurrentIndex = 0;

	for (const auto& Entry : QueuedTiles)
	{
		if (Entry.Value != -1) {
			float Distance = FVector2D::Distance(GetTileLocation(Entry.Key), FVector2D(GetPlayerLocation()));

			if (Distance > FurthestDistance && Distance > TileReplaceableDistance) {
				FurthestDistance = Distance;
				FurthestTileIndex = CurrentIndex;
			}
		}
		++CurrentIndex;
	}

	return FurthestTileIndex;
}

float ALandscapeGeneration::GetMaxSubVerticlesHeight()
{
	float MaxHeight = 0;
	
	for (const FVector& V : SubVerticles)
	{
		if (V.Z > MaxHeight)
			MaxHeight = V.Z;
	}

	return MaxHeight;
}

float ALandscapeGeneration::GetHeight(FVector2D Location)
{
	return PerLinNoiseExtended(Location, 1/ MountainScale, MountainHeight, .1f, .1f)
		+ PerLinNoiseExtended(Location, 1/ BoulderScale, BoulderHeight, .2f, .2f)
		+ PerLinNoiseExtended(Location, .001f, 500, .3f, .3f)
		+ PerLinNoiseExtended(Location, .01f, 100, .4f, .4f);
}

float ALandscapeGeneration::PerLinNoiseExtended(const FVector2D Location, const float Scale, const float Amplitude, const double XOffset, const double YOffset)
{
	return FMath::PerlinNoise2D(Location * Scale + FVector2D(.1f + XOffset, .1f + YOffset) + PerlinOffset) * Amplitude;
}

void ALandscapeGeneration::CreateTriangles()
{
	int32 Buffer = XVertex + 2;

	for (int32 y = 0; y <= YVertex; ++y)
	{
		int YBuffer = y * Buffer;
		int YBuffer2 = YBuffer + Buffer;

		for (int32 x = 0; x <= XVertex; ++x)
		{
			Triangles.Add(x + YBuffer);
			Triangles.Add(x + YBuffer2);
			Triangles.Add(x + YBuffer + 1);

			Triangles.Add(x + YBuffer2);
			Triangles.Add(x + YBuffer2 + 1);
			Triangles.Add(x + YBuffer + 1);
		}
	}
}

void ALandscapeGeneration::CreateVerticlesAndUVs(const int InSectionIndexX, const int InSectionIndexY, TArray<FVector>& Verticles, TArray<FVector2D>& Uvs)
{
	double XScaleOffset = InSectionIndexX * (XVertex - 1) * Size;
	double YScaleOffset = InSectionIndexY * (YVertex - 1) * Size;

	for (int32 y = -1; y <= YVertex; ++y)
	{
		float OptiY = y * Size + YScaleOffset;
		for (int32 x = -1; x <= XVertex; ++x)
		{
			float OptiX = x * Size + XScaleOffset;

			Verticles.Add(FVector(OptiX, OptiY, GetHeight(FVector2d(OptiX, OptiY))));

			Uvs.Add(FVector2D(OptiX / 100, OptiY / 100));
		}
	}
}

void ALandscapeGeneration::CreateSubsets(const TArray<FVector>& Verticles, const TArray<FVector2D>& Uvs, const TArray<FVector>& Normals, const TArray<FProcMeshTangent>& Tangents)
{
	int VertextIndex = 0;

	for (int32 y = -1; y <= YVertex; ++y)
	{
		for (int32 x = -1; x <= XVertex; ++x)
		{
			if (-1 < y && y < YVertex && -1 < x && x < XVertex) {
				SubVerticles.Add(Verticles[VertextIndex]);
				SubUvs.Add(Uvs[VertextIndex]);
				SubNormals.Add(Normals[VertextIndex]);
				SubTangents.Add(Tangents[VertextIndex]);
			}

			++VertextIndex;
		}
	}

	if (SubTriangles.IsEmpty())
	{
		int32 YLength = YVertex - 2;
		int32 XLength = XVertex - 2;

		for (int32 y = 0; y <= YLength; ++y)
		{
			int YBuffer = y * XVertex;
			int YBuffer2 = YBuffer + XVertex;

			for (int32 x = 0; x <= XLength; ++x)
			{
				SubTriangles.Add(x + YBuffer);
				SubTriangles.Add(x + YBuffer2);
				SubTriangles.Add(x + YBuffer + 1);

				SubTriangles.Add(x + YBuffer2);
				SubTriangles.Add(x + YBuffer2 + 1);
				SubTriangles.Add(x + YBuffer + 1);
			}
		}
	}

	TileDataReady = true;
}

void FAsyncLandscapeGeneration::DoWork()
{
	LandScapeGenerator->GenerateMap(LandScapeGenerator->SectionIndexX, LandScapeGenerator->SectionIndexY);
}
