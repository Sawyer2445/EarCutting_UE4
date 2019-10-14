// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPolygon.h"
#include "ProceduralMeshComponent.h"
#include "BFL_EarCutting.h"


// Sets default values
ATestPolygon::ATestPolygon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	UBFL_EarCutting::GenerateIndexBufferObject(VBO, IBO);

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Polygon"));
	ProceduralMesh->SetupAttachment(RootComponent);

	ProceduralMesh->bUseAsyncCooking = true;
}

void ATestPolygon::CretePolygone()
{
	UBFL_EarCutting::GenerateIndexBufferObject(VBO, IBO);
	ProceduralMesh->CreateMeshSection(0, VBO, IBO, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

void ATestPolygon::PostActorCreated()
{
	Super::PostActorCreated();
	CretePolygone();
}
