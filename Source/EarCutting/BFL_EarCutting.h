// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_EarCutting.generated.h"


/**
 * 
 */

struct PolyVertx
{
	int32 vertex_index;
	FVector vertex_coord;
	
	PolyVertx(int32 _i, FVector _v) : vertex_index(_i), vertex_coord(_v) {}
	PolyVertx() : vertex_index(-1), vertex_coord(FVector(0.f, 0.f, 0.f)) {}
	//PolyVertx(int32 _i) : vertex_index(_i), vertex_coord(FVector(0.f, 0.f, 0.f)) {}

	friend bool operator== (const PolyVertx& a, const PolyVertx& b)
	{
		return a.vertex_index == b.vertex_index;
	}

};

UCLASS()
class EARCUTTING_API UBFL_EarCutting : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// return true if triangle ABC is Clockwise [WORK]
	static bool isClockwise(FVector _A, FVector _B, FVector _C);

	//return true if point P is inside triangle ABC
	UFUNCTION(BlueprintCallable, Category = "EarCutting")
	static bool isTriangleContainPoint(FVector _A, FVector _B, FVector _C, FVector _P);

	//return true if point P is left of AB line [WORK]
	UFUNCTION(BlueprintCallable, Category = "EarCutting")
	static bool isLeftPoint(FVector _A, FVector _B, FVector _P);

	//return true if Point B is convex (angel > 180)
	UFUNCTION(BlueprintCallable, Category = "EarCutting")
	static bool isConvexPoint(FVector _A, FVector _B, FVector _C);
	

	static void UpdateConvexAndReflexList(TMap<int32, FVector>& Poly, TArray<int32>& ConvexList, TArray<int32>&  ReflexList, TArray<int32>& Ears);

	static void FindEars(TMap<int32, FVector>& Poly, TArray<int32>& ConvexList, TDoubleLinkedList<int32>& Ears);

public:
	
	UFUNCTION(BlueprintCallable, Category = "EarCutting")
	static void test(TMap<int32, FVector>& Vertexes, TArray<int32>& IBO, UPARAM(ref) TArray<FVector>& InArray);
};
