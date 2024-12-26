// Fill out your copyright notice in the Description page of Project Settings.


#include "Square.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
ASquare::ASquare()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
}


void ASquare::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

// Called when the game starts or when spawned
void ASquare::BeginPlay()
{
	Super::BeginPlay();

	Vertices.Reset();
	Triangles.Reset();
	UVs.Reset();
	Normals.Reset();
	Tangents.Reset();

	/*CreateVertices();
	CreateTriangles();

	// calcul les normals et les tangentes
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
	ProceduralMesh->SetMaterial(0, Material);
	*/
}

// Called every frame
void ASquare::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASquare::CreateVertices() 
{
	for (int x=0; x<=Xsize; ++x)
	{
		for (int y = 0; y <= Ysize; ++y)
		{
			float z = FMath::PerlinNoise2D(FVector2D(x * NoiseScale + 0.1, y * NoiseScale + 0.1)) * ZMultiplier;

			Vertices.Add(FVector(x * Scale, y * Scale, z));
			UVs.Add(FVector2D(x * UVScale, y * UVScale));

			// Debug
			//DrawDebugSphere(GetWorld(), FVector(x * Scale, y * Scale, 0), 25.0f, 16, FColor::Red, true, -1.0f, 0U, 0.0f);
		}
	}
}


void ASquare::CreateTriangles() 
{
	int Vertex = 0;

	for (int y = 0; y < Ysize; y++)
	{
		for (int x = 0; x < Xsize; x++)
		{
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + Ysize + 1);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + Ysize + 2);
			Triangles.Add(Vertex + Ysize + 1);

			++Vertex;
		}
		++Vertex;
	}

}

