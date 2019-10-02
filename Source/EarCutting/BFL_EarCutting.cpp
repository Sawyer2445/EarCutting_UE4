// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_EarCutting.h"
#include "EngineUtils.h"
#include <list>
#include <iterator>
#include <algorithm>

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

void UBFL_EarCutting::UpdateConvexAndReflexList(TMap<int32, FVector>& Poly, TArray<int32>& ConvexList, TArray<int32>&  ReflexList, TArray<int32>& Ears)
{
	ConvexList.Empty();
	ReflexList.Empty();
	Ears.Empty();

	std::list<PolyVertx> vertex_list;
	for (auto& v : Poly)
	{
		vertex_list.push_back(PolyVertx(v.Key, v.Value));
	}

	for (auto i = vertex_list.begin(); i != vertex_list.end(); i++)
	{
		if (isConvexPoint(std::prev(i, 1)->vertex_coord, i->vertex_coord, std::next(i, 1)->vertex_coord))
		{
			ConvexList.Add(i->vertex_index);
			FVector _A = std::prev(i, 1)->vertex_coord;
			FVector _B = i->vertex_coord;
			FVector _C = std::next(i, 1)->vertex_coord;
			int32 check_counter = 0;
			for (auto& p : Poly)
			{
				if (!isTriangleContainPoint(_A, _B, _C, p.Value))
				{
					check_counter++;
				}
			}
			if (check_counter == (Poly.Num() - 3))
			{
				Ears.Add(i->vertex_index);
			}
		}
		else
		{
			ReflexList.Add(i->vertex_index);
		}
	}
}

void UBFL_EarCutting::GetIBO(TArray<FVector>& InVertexes, TArray<int32>& IBO)
{	
	IBO.Empty(); 
	TMap<int32, FVector> Vertexes;

	for (int32 i = 0; i < InVertexes.Num(); i++)
	{
		Vertexes.Add(i, InVertexes[i]);
	}

	TArray<int32> ConvexVertexes;
	TArray<int32> ReflexVertexes;
	TArray<int32> EarsVertexes;		//vertexes for cutting
	TArray<int32> _IBO_temp;		//temp IBO array
	_IBO_temp.Empty();


	UpdateConvexAndReflexList(Vertexes, ConvexVertexes, ReflexVertexes, EarsVertexes);
	
	
	std::list<PolyVertx> vertex_list;	//using std::list for STL functions
	for (auto& v : Vertexes)			//filling list
	{
		vertex_list.push_back(PolyVertx(v.Key, v.Value));
	}

	while (EarsVertexes.Num() != 0) 
	{
		UpdateConvexAndReflexList(Vertexes, ConvexVertexes, ReflexVertexes, EarsVertexes);
		auto it = std::find(vertex_list.begin(), vertex_list.end(), PolyVertx(EarsVertexes[0], FVector()));
		auto prev_it = std::prev(it, 1);	//неверное значение. проверить работу 
		auto next_it = std::next(it, 1);
			
		_IBO_temp.Add(prev_it->vertex_index);
		_IBO_temp.Add(it->vertex_index);
		_IBO_temp.Add(next_it->vertex_index);

		vertex_list.remove(*it);
		//vertex_list.remove(PolyVertx(it->vertex_index, it->vertex_coord));
		Vertexes.Remove(it->vertex_index);

		UpdateConvexAndReflexList(Vertexes, ConvexVertexes, ReflexVertexes, EarsVertexes);
	
	}
	TArray<int32> values;
	Vertexes.GetKeys(values);

	//_IBO_temp.Add(values[0]);
	//_IBO_temp.Add(values[1]);
	//_IBO_temp.Add(values[2]);
	//values.Empty();
	//clearing resources
	ConvexVertexes.Empty();
	ReflexVertexes.Empty();
	EarsVertexes.Empty();

	IBO.Empty();
	for (auto& i : _IBO_temp)
	{
		IBO.Add(i);
	}
	//return array
	_IBO_temp.Empty();
}
