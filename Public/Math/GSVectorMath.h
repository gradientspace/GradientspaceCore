// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSVector3.h"

// try to avoid including this in other headers...

namespace GS
{


template<typename RealType>
inline Vector3<RealType> ClosestUnitVector(const Vector3<RealType>& Vec)
{
	// dot each vector w/ unit axis
	Vector3<RealType> AxisDots( GS::Abs(Vec.X), GS::Abs(Vec.Y), GS::Abs(Vec.Z) );
	int MaxAxisIndex = 2;
	if (AxisDots.X > AxisDots.Y && AxisDots.X > AxisDots.Z)
		MaxAxisIndex = 0;
	else if (AxisDots.Y > AxisDots.X && AxisDots.Y > AxisDots.Z)
		MaxAxisIndex = 1;
	Vector3<RealType> Result(0, 0, 0);
	Result[MaxAxisIndex] = (GS::Sign(Vec[MaxAxisIndex]) >= 0) ? 1 : -1;
	return Result;
}


}  // end namespace GS
