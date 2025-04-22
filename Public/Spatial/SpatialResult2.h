// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSVector2.h"
#include "Math/GSIndex2.h"

namespace GS
{

template<typename RealType>
struct DistanceResult2
{
	int ElementID = -1;
	RealType DistanceSqr = (RealType)0;
	Vector2<RealType> Point = Vector2<RealType>::Zero();
	Vector2<RealType> Parameters = Vector2<RealType>::Zero();
	Index2i Indices = Index2i::NegOne();

	DistanceResult2() {}
	DistanceResult2(int element_id) 
		: ElementID(element_id) {}

	DistanceResult2(int element_id, RealType distanceSqr) 
		: ElementID(element_id), DistanceSqr(distanceSqr) {}

	DistanceResult2(int element_id, RealType distanceSqr, Vector2<RealType> point, int index0, RealType parameter0)
		: ElementID(element_id), DistanceSqr(distanceSqr), Point(point), Parameters(parameter0,0), Indices(index0,-1) {}

	operator bool() const { return ElementID >= 0; }
	bool IsValid() const { return ElementID >= 0; }

	RealType Distance() const { return GS::Sqrt(DistanceSqr); }
};
typedef DistanceResult2<float> DistanceResult2f;
typedef DistanceResult2<double> DistanceResult2d;


}