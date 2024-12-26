// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Square.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class GROUPE_7_PROJET_API ASquare : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASquare();

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), BlueprintReadWrite)
	int Xsize = 50; // nombre de carre pour l axe X

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), BlueprintReadWrite)
	int Ysize = 50; // nombre de carre pour l axe Y

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), BlueprintReadWrite)
	float ZMultiplier = 1000.0f; // multiplier le Perlin Noise

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), BlueprintReadWrite)
	float NoiseScale = 0.05f; // permet d adoucir les pentes

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.0001), BlueprintReadWrite)
	float Scale = 100; 

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.0001), BlueprintReadWrite)
	float UVScale = 1.0f / Xsize;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

	UProceduralMeshComponent* ProceduralMesh;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	

	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UVs;
	TArray<FVector> Normals;
	TArray<struct FProcMeshTangent> Tangents;

	UFUNCTION(BlueprintCallable)
	void CreateVertices();
	UFUNCTION(BlueprintCallable)
	void CreateTriangles();
};
