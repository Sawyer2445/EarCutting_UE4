// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_EarCutting.h"
#include "EngineUtils.h"

bool UBFL_EarCutting::isClockwise(const FVector& _A, const FVector& _B, const FVector& _C) 
{
	FVector _AB = _B - _A;
	
	FVector _BC = _C - _B;
	
	FVector NormalCrossProduct = FVector::CrossProduct(_AB, _BC);
	NormalCrossProduct = NormalCrossProduct / NormalCrossProduct.Size();

	return true ? (NormalCrossProduct.Z > 0) : false;
}

bool UBFL_EarCutting::isTriangleContainPoint(const FVector& _A, const FVector& _B, const FVector& _C, const FVector& _P) 
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

bool UBFL_EarCutting::isLeftPoint(const FVector& _A, const FVector& _B, const FVector& _P) 
{
	float result = (_P.X - _A.X)*(_B.Y - _A.Y) - (_P.Y - _A.Y)*(_B.X - _A.X);
	return true ? (result >= 0) : false;
}

bool UBFL_EarCutting::isConvexPoint(const FVector& _A, const FVector& _B, const FVector& _C) 
{
	return !isClockwise(_A, _B, _C);
}

void UBFL_EarCutting::UpdateConvexAndReflexList(TArray<PolyVertx>& Poly, TArray<int32>& ConvexList, TArray<int32>&  ReflexList, TArray<int32>& Ears)
{
	ConvexList.Empty();
	ReflexList.Empty();
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
			ConvexList.Add(vertex_vector[index].vertex_index);
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
			ReflexList.Add(vertex_vector[index].vertex_index);
		}
	}
}

void UBFL_EarCutting::GetIBO(TArray<FVector>& InVertexes, TArray<int32>& IBO)
{	
	IBO.Empty();
	auto mod = [](int a, int b) {return (b + (a % b)) % b; };
	TArray<PolyVertx> Vertexes;

	for (int32 i = 0; i < InVertexes.Num(); i++)
	{
		Vertexes.Add(PolyVertx(i, InVertexes[i]));
	}

	TArray<int32> ConvexVertexes;
	TArray<int32> ReflexVertexes;
	TArray<int32> EarsVertexes;		//vertexes for cutting
	TArray<int32> _IBO_temp;		//temp IBO array
	_IBO_temp.Empty();


	UpdateConvexAndReflexList(Vertexes, ConvexVertexes, ReflexVertexes, EarsVertexes);
	

	while (EarsVertexes.Num() != 0) 
	{
		UpdateConvexAndReflexList(Vertexes, ConvexVertexes, ReflexVertexes, EarsVertexes);
		auto index = Vertexes.Find(PolyVertx(EarsVertexes[0], FVector()));
		auto prev_index = mod(index - 1, Vertexes.Num());
		auto next_index = mod(index + 1, Vertexes.Num());
			
		_IBO_temp.Add(Vertexes[prev_index].vertex_index);
		_IBO_temp.Add(Vertexes[index].vertex_index);
		_IBO_temp.Add(Vertexes[next_index].vertex_index);

		Vertexes.RemoveAt(index);

		UpdateConvexAndReflexList(Vertexes, ConvexVertexes, ReflexVertexes, EarsVertexes);
	}
	ConvexVertexes.Empty();
	ReflexVertexes.Empty();
	EarsVertexes.Empty();

	IBO = _IBO_temp;
	_IBO_temp.Empty();
}
