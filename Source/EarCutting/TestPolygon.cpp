// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPolygon.h"
#include "ProceduralMeshComponent.h"
#include "PolygonTriangulationBPLibrary.h"

// Sets default values
ATestPolygon::ATestPolygon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Polygon"));
	ProceduralMesh->SetupAttachment(RootComponent);

	ProceduralMesh->bUseAsyncCooking = true;
}

void ATestPolygon::CretePolygone()
{
	UPolygonTriangulationBPLibrary::GenerateTringles(VBO, false, IBO);
	for (auto i = 0; i != VBO.Num(); i++)
	{
		VertexColors.Add(FColor(0, 255, 0));
	}
	ProceduralMesh->CreateMeshSection(1, VBO, IBO, TArray<FVector>(), TArray<FVector2D>(),VertexColors , TArray<FProcMeshTangent>(), true);
}

void ATestPolygon::PostActorCreated()
{
	Super::PostActorCreated();
	CretePolygone();
}
