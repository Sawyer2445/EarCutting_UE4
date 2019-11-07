// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestPolygon.generated.h"


UCLASS()
class EARCUTTING_API ATestPolygon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestPolygon();

protected:
	/** Vertex Buffer Object. Store vertexes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vars")
	TArray<FVector> VBO;

	/** Index Buffer Object. Store indexes of triangls*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vars")
	TArray<int32> IBO;

	/** Array of vertex' color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vars")
	TArray<FColor> VertexColors;
	
	/** Polygon */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	class UProceduralMeshComponent* ProceduralMesh;

	void CretePolygone();

	void PostActorCreated() override;
};
