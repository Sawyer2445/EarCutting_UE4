// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PolygonTriangulationBPLibrary.generated.h"

/**
 * Struct for storage Vertex coordinate and its index in polygon
 */
struct PolyVertx
{
	int32	vertex_index;
	FVector vertex_coord;
	
	PolyVertx(int32 _i, FVector _v) : vertex_index(_i), vertex_coord(_v) {}
	PolyVertx() : vertex_index(0), vertex_coord(FVector(0.f, 0.f, 0.f)) {}
	
	friend bool operator== (const PolyVertx& a, const PolyVertx& b)
	{
		return a.vertex_index == b.vertex_index;
	}

};

/**
 *	Implementation Ear cutting(clipping) algorithm
 */
UCLASS()
class POLYGONTRIANGULATION_API UPolygonTriangulationBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

private:
	/**
	 * True is triangle ABC is clockwise
	 *
	 * @param _A	First triangle vertex
	 * @param _B	Second triangle vertex
	 * @param _C	Third triangle vertex
	 * @return		Is ABC clockwise ?
	 */
	static bool isClockwise(const FVector& _A, const FVector& _B, const FVector& _C);

	/**
	 * True if point P is inside triangle ABC
	 *
	 * @param _A	First triangle vertex
	 * @param _B	Second triangle vertex
	 * @param _C	Third triangle vertex
	 * @param _P	Point to test
	 * @return		ABC contain point ?
	 */
	static bool isTriangleContainPoint(const FVector& _A, const FVector& _B, const FVector& _C, const FVector& _P);

	/**
	 * True if point P is left of AB line
	 *
	 * @param _A	First line vertex
	 * @param _B	Second line vertex
	 * @param _P	Point to test
	 * @return		Is Point to the left of the line?
	 */
	static bool isLeftPoint(const FVector& _A, const FVector& _B, const FVector& _P);

	/**
	 * True if Point B is convex (angel > 180)
	 *
	 * @param _A	First adjacent vertex
	 * @param _B	Vertex to test
	 * @param _Ñ	Second adjacent vertex
	 * @return		Is point convex ?
	 */
	static bool isConvexPoint(const FVector& _A, const FVector& _B, const FVector& _C);

	/**
	 * Update arrays with  convex, reflex and ear vertices
	 *
	 * @param Polygon		Array of tuple  with polygon key - vertex index in polygon value - vertex coordinate
	 * @param ConvexList	Array with convex vertexes
	 * @param ReflexList	Array with reflex vertexes
	 * @param Ears			Array with vertexes for cutting
	 */
	static void UpdateConvexAndReflexList(TArray<PolyVertx>& Polygon, TArray<int32>& Convex, TArray<int32>&  Reflex, TArray<int32>& Ears);
public:
	/**
	 * Return triangles (index buffer object).
	 *
	 * @param Vertices		Polygon's vertexes (must be counter-clockwise !!!)
	 * @param IBO			Integer out array contains indexes 
	 * @param bTwosides		Create double sided triangles for  polygon
	 */
	UFUNCTION(BlueprintCallable, Category = "EarCutting")
	static void GenerateTringles(UPARAM(ref, DisplayName = "Vertices") TArray<FVector>& InVertices, bool  bDoubleSided, TArray<int32>& Triangles);

	/**
	 * Reverse vector array
	 *
	 * @param Vertices	
	 * @param ReversedVertices		
	 */
	UFUNCTION(BlueprintCallable, Category = "EarCutting|Helpers")
	static void ReverseVertices(UPARAM(ref, DisplayName = "Vertices") TArray<FVector>& Vertices, TArray<FVector>& ReversedVertices);

};
