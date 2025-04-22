// Copyright Gradientspace Corp. All Rights Reserved.
#include "Grid/GSGridUtil.h"

using namespace GS;

const Vector3i* GS::GetGrid4NeighbourOffsetsPerpToAxis(int UnitAxisIndex)
{
	const static Vector3i GridNeighbours4ByAxis[3][4] =
	{
		{ Vector3i{0,-1,0}, Vector3i{0,1,0}, Vector3i{0,0,-1}, Vector3i{0,0,1} },
		{ Vector3i{-1,0,0}, Vector3i{1,0,0}, Vector3i{0,0,-1}, Vector3i{0,0,1} },
		{ Vector3i{-1,0,0}, Vector3i{1,0,0}, Vector3i{0,-1,0}, Vector3i{0,1,0} }
	};
	return GridNeighbours4ByAxis[ GS::Clamp(UnitAxisIndex, 0, 3) ];
}


const Vector3i* GS::GetGrid8NeighbourOffsetsPerpToAxis(int UnitAxisIndex)
{
	const static Vector3i GridNeighbours8ByAxis[3][8] =
	{
		{ Vector3i{0,-1,0}, Vector3i{0,1,0}, Vector3i{0,0,-1}, Vector3i{0,0,1}, Vector3i{0,-1,-1}, Vector3i{0,1,-1}, Vector3i{0,1,1}, Vector3i{0,-1,1} },
		{ Vector3i{-1,0,0}, Vector3i{1,0,0}, Vector3i{0,0,-1}, Vector3i{0,0,1}, Vector3i{-1,0,-1}, Vector3i{1,0,-1}, Vector3i{1,0,1}, Vector3i{-1,0,1} },
		{ Vector3i{-1,0,0}, Vector3i{1,0,0}, Vector3i{0,-1,0}, Vector3i{0,1,0}, Vector3i{-1,-1,0}, Vector3i{1,-1,0}, Vector3i{1,1,0}, Vector3i{-1,1,0} }
	};
	return GridNeighbours8ByAxis[ GS::Clamp(UnitAxisIndex, 0, 3) ];
}

