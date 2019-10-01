// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_EarCutting.h"
#include "EngineUtils.h"
#include <list>
#include <iterator>
#include <algorithm>

bool UBFL_EarCutting::isClockwise(FVector _A, FVector _B, FVector _C)
{
	FVector _AB = _B - _A;
	
	FVector _BC = _C - _B;
	
	FVector NormalCrossProduct = FVector::CrossProduct(_AB, _BC);
	NormalCrossProduct = NormalCrossProduct / NormalCrossProduct.Size();

	return true ? (NormalCrossProduct.Z > 0) : false;
}

bool UBFL_EarCutting::isTriangleContainPoint(FVector _A, FVector _B, FVector _C, FVector _P)
{
	bool test1;// = isLeftPoint(_B, _A, _P);
	bool test2;// = isLeftPoint(_C, _B, _P);
	bool test3;// = isLeftPoint(_A, _C, _P);
	bool _return;
	if (isClockwise(_A, _B, _C))
	{
		test1 = isLeftPoint(_B, _A, _P);
		test2 = isLeftPoint(_C, _B, _P);
		test3 = isLeftPoint(_A, _C, _P);
		_return = (test1 && test2 && test3);
	}
	else
	{
		test1 = isLeftPoint(_A, _B, _P);
		test2 = isLeftPoint(_B, _C, _P);
		test3 = isLeftPoint(_C, _A, _P);
		_return = (test1 && test2 && test3);
	}
	//bool _return = !(test1 && test2 && test3) && isClockwise(_A, _B, _C);
	return _return;
}

bool UBFL_EarCutting::isLeftPoint(FVector _A, FVector _B, FVector _P)
{
	float result = (_P.X - _A.X)*(_B.Y - _A.Y) - (_P.Y - _A.Y)*(_B.X - _A.X);
	return true ? (result >= 0) : false;
}


bool UBFL_EarCutting::isConvexPoint(FVector _A, FVector _B, FVector _C)
{
	FVector _BA = _A - _B; 
	FVector _BC = _C - _B; 
	
	float _alpha = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(_BA, _BC) / (_BA.Size() * _BC.Size())));
	return !isClockwise(_A, _B, _C);
	//return true ? _alpha > 6.283185 : false;
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
		if (isConvexPoint(std::prev(i,1)->vertex_coord, i->vertex_coord, std::next(i,1)->vertex_coord))
		{
			ConvexList.Add(i->vertex_index);
			FVector _A = std::prev(i)->vertex_coord;
			FVector _B = i->vertex_coord;
			FVector _C = std::next(i)->vertex_coord;
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


void UBFL_EarCutting::FindEars(TMap<int32, FVector>& Poly, TArray<int32>& ConvexList, TDoubleLinkedList<int32>& Ears)
{
	Ears.Empty();

	std::list<int32> convex_list;
	for (auto i = 0; i != ConvexList.Num(); i++)
	{
		convex_list.push_back(ConvexList[i]);
	}

	std::list<PolyVertx> vertex_list;
	for (auto& v : Poly)
	{
		vertex_list.push_back(PolyVertx(v.Key, v.Value));
	}

	//analys 
	for (auto i : convex_list)
	{
		
		FVector _A;
		FVector _B;
		FVector _C;
		for (auto j : vertex_list)
		{
			
		}

	}
}

void UBFL_EarCutting::test(TMap<int32, FVector>& Vertexes,  TArray<int32>& IBO, UPARAM(ref) TArray<FVector>& InArray)
{
	IBO.Empty();

	for (int32 i = 0; i < InArray.Num(); i++)
	{
		Vertexes.Add(i, InArray[i]);
	}

	TArray<int32> C;
	TArray<int32> R;
	TArray<int32> E;
	TArray<int32> _I;

	UpdateConvexAndReflexList(Vertexes, C, R,E);
	
	//////////////////////////////////////////////////////////////////////////
	std::list<PolyVertx> vertex_list;
	for (auto& v : Vertexes)
	{
		vertex_list.push_back(PolyVertx(v.Key, v.Value));
	}

	while (E.Num() > 0)
	{
		UpdateConvexAndReflexList(Vertexes, C, R, E);


		auto it = std::find(vertex_list.begin(), vertex_list.end(), PolyVertx(E[0], *Vertexes.Find(E[0])) );
		auto prev_it = std::prev(it, 1);
		auto next_it = std::next(it, 1);

		_I.Add(prev_it->vertex_index);
		_I.Add(it->vertex_index);
		_I.Add(next_it->vertex_index);

		vertex_list.remove(PolyVertx(it->vertex_index, it->vertex_coord));
		Vertexes.Remove(it->vertex_index);


		UpdateConvexAndReflexList(Vertexes, C, R, E);
	}
	//////////////////////////////////////////////////////////////////////////
	IBO = _I;
	C.Empty();
	R.Empty();
	E.Empty();
	_I.Empty();

}
