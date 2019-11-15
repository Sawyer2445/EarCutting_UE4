// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PolygonTriangulationBPLibrary.h"
#include "PolygonTriangulation.h"

UPolygonTriangulationBPLibrary::UPolygonTriangulationBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UPolygonTriangulationBPLibrary::isClockwise(const FVector& _A, const FVector& _B, const FVector& _C)
{
	FVector _AB = _B - _A;

	FVector _BC = _C - _B;

	FVector NormalCrossProduct = FVector::CrossProduct(_AB, _BC);
	NormalCrossProduct = NormalCrossProduct / NormalCrossProduct.Size();

	return true ? (NormalCrossProduct.Z > 0) : false;
}

bool UPolygonTriangulationBPLibrary::isTriangleContainPoint(const FVector& _A, const FVector& _B, const FVector& _C, const FVector& _P)
{
	bool _PisLeft_BA;
	bool _PisLeft_CB;
	bool _PisLeft_CA;
	bool _return;
	if (isClockwise(_A, _B, _C))
	{
		_PisLeft_BA = isLeftPoint(_B, _A, _P);
		_PisLeft_CB = isLeftPoint(_C, _B, _P);
		_PisLeft_CA = isLeftPoint(_A, _C, _P);

		_return = (_PisLeft_BA && _PisLeft_CB && _PisLeft_CA);
	}
	else
	{
		_PisLeft_BA = isLeftPoint(_A, _B, _P);
		_PisLeft_CB = isLeftPoint(_B, _C, _P);
		_PisLeft_CA = isLeftPoint(_C, _A, _P);

		_return = (_PisLeft_BA && _PisLeft_CB && _PisLeft_CA);
	}
	return _return;
}

bool UPolygonTriangulationBPLibrary::isLeftPoint(const FVector& _A, const FVector& _B, const FVector& _P)
{
	float result = (_P.X - _A.X)*(_B.Y - _A.Y) - (_P.Y - _A.Y)*(_B.X - _A.X);
	return true ? (result >= 0) : false;
}

bool UPolygonTriangulationBPLibrary::isConvexPoint(const FVector& _A, const FVector& _B, const FVector& _C)
{
	return !isClockwise(_A, _B, _C);
}

void UPolygonTriangulationBPLibrary::UpdateConvexAndReflexList(TArray<PolyVertx>& Poly, TArray<int32>& Convex, TArray<int32>& Reflex, TArray<int32>& Ears)
{
	Convex.Empty();
	Reflex.Empty();
	Ears.Empty();

	auto mod = [](int a, int b) {return (b + (a % b)) % b; };

	TArray<PolyVertx> vertex_vector;
	for (auto& v : Poly)
	{
		vertex_vector.Add(v);
	}

	for (auto i = 0; i != vertex_vector.Num(); i++)
	{
		int prev_index = mod(i - 1, vertex_vector.Num());
		int index = i;
		int next_index = mod(index + 1, vertex_vector.Num());
		if (isConvexPoint(vertex_vector[prev_index].vertex_coord, vertex_vector[index].vertex_coord, vertex_vector[next_index].vertex_coord))
		{
			Convex.Add(vertex_vector[index].vertex_index);
			FVector _A = vertex_vector[prev_index].vertex_coord;
			FVector _B = vertex_vector[index].vertex_coord;
			FVector _C = vertex_vector[next_index].vertex_coord;
			int32 check_counter = 0;
			for (auto& p : Poly)
			{
				if (!isTriangleContainPoint(_A, _B, _C, p.vertex_coord))
				{
					check_counter++;
				}
			}
			if (check_counter == (Poly.Num() - 3))
			{
				Ears.Add(vertex_vector[index].vertex_index);
			}
		}
		else
		{
			Reflex.Add(vertex_vector[index].vertex_index);
		}
	}
}

void UPolygonTriangulationBPLibrary::GenerateTringles(TArray<FVector>& InVertices, bool bDoubleSided, TArray<int32>& Triangles)
{
	auto mod = [](int a, int b) {return (b + (a % b)) % b; };

	TArray<PolyVertx> TempVertices;

	for (int32 i = 0; i < InVertices.Num(); i++)
	{
		TempVertices.Add(PolyVertx(i, InVertices[i]));
	}

	TArray<int32> ConvexVertices;
	TArray<int32> ReflexVertices;
	TArray<int32> EarsVertices;			//vertexes for cutting
	TArray<int32> TempTriangles;		

	UpdateConvexAndReflexList(TempVertices, ConvexVertices, ReflexVertices, EarsVertices);


	while (EarsVertices.Num() != 0)
	{
		auto index = TempVertices.Find(PolyVertx(EarsVertices[0], FVector()));
		auto prev_index = mod(index - 1, TempVertices.Num());
		auto next_index = mod(index + 1, TempVertices.Num());

		TempTriangles.Add(TempVertices[prev_index].vertex_index);
		TempTriangles.Add(TempVertices[index].vertex_index);
		TempTriangles.Add(TempVertices[next_index].vertex_index);

		// add additional triangles
		if (bDoubleSided)
		{
			TempTriangles.Add(TempVertices[next_index].vertex_index);
			TempTriangles.Add(TempVertices[index].vertex_index);
			TempTriangles.Add(TempVertices[prev_index].vertex_index);
		}

		TempVertices.RemoveAt(index);

		UpdateConvexAndReflexList(TempVertices, ConvexVertices, ReflexVertices, EarsVertices);
	}
	//ConvexVertices.Empty();
	//ReflexVertices.Empty();
	//EarsVertices.Empty();


	Triangles = TempTriangles;
	//TempTriangles.Empty();
}

