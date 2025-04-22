// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSIntVector2.h"
#include "Math/GSIntVector3.h"

namespace GS
{

template<typename Index3Type>
int64_t ToLinearGridIndex(const Index3Type& GridIndex, const Index3Type& GridDimensions)
{
	return (((int64_t)GridIndex.Z * (int64_t)GridDimensions.Y + (int64_t)GridIndex.Y) * (int64_t)GridDimensions.X) + (int64_t)GridIndex.X;
}

template<typename Index3Type>
Index3Type ToVectorGridIndex(int64_t LinearIndex, const Index3Type& GridDimensions)
{
	// todo maybe can do faster
	int64_t X = LinearIndex % (int64_t)GridDimensions.X;
	int64_t Y = (LinearIndex / (int64_t)GridDimensions.X) % (int64_t)GridDimensions.Y;
	int64_t Z = LinearIndex / ((int64_t)GridDimensions.X * (int64_t)GridDimensions.Y);
	return Index3Type((int)X, (int)Y, (int)Z);
}



template<typename FuncType>
void EnumerateCellsInRangeInclusive(Vector2i Min, Vector2i Max, FuncType CellFunc)
{
	for (int yi = Min.Y; yi <= Max.Y; ++yi)
	{
		for (int xi = Min.X; xi <= Max.X; ++xi)
		{
			CellFunc(Vector2i(xi, yi));
		}
	}
}


template<typename FuncType>
void EnumerateCellsInRangeInclusive(Vector3i Min, Vector3i Max, FuncType CellFunc)
{
	for (int zi = Min.Z; zi <= Max.Z; zi++)
	{
		for (int yi = Min.Y; yi <= Max.Y; ++yi)
		{
			for (int xi = Min.X; xi <= Max.X; ++xi)
			{
				CellFunc(Vector3i(xi, yi, zi));
			}
		}
	}
}


template<typename FuncType>
void EnumerateDimensionsExclusive(Vector3i Dimensions, FuncType IndexFunc)
{
	for (int zi = 0; zi < Dimensions.Z; zi++)
	{
		for (int yi = 0; yi < Dimensions.Y; ++yi)
		{
			for (int xi = 0; xi < Dimensions.X; ++xi)
			{
				IndexFunc(Vector3i(xi, yi, zi));
			}
		}
	}
}


//! return array of 4 in-plane grid-neighbour-index-offsets, eg for 0 (x-axis) it returns (0,-1,0), (0,1,0), (0,0,-1), (0,0,1). Valid inputs are 0,1,2
GRADIENTSPACECORE_API
const Vector3i* GetGrid4NeighbourOffsetsPerpToAxis(int UnitAxisIndex);

//! return array of 8 in-plane grid-neighbour-index-offsets. Valid inputs are 0,1,2
GRADIENTSPACECORE_API
const Vector3i* GetGrid8NeighbourOffsetsPerpToAxis(int UnitAxisIndex);



}

