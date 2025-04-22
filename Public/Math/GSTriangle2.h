// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSHash.h"
#include "Math/GSMath.h"
#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSAxisBox2.h"
#include "Math/GSSegment2.h"

namespace GS
{

// triangle utility functions...maybe somewhere else?

//! calculate barycentric coords of Point relative to Triangle ABC
GRADIENTSPACECORE_API
Vector3d GetTriBaryCoords2d(const Vector2d& A, const Vector2d& B, const Vector2d& C, const Vector2d& Point);

//! test if Point is strictly inside Triangle ABC
GRADIENTSPACECORE_API
bool IsPointInsideTriangle(const Vector2d& A, const Vector2d& B, const Vector2d& C, const Vector2d& Point);

//! test if Point is inside or on Triangle ABC
// NOTE: still have to implement this function. It is not as simple as doing >= comparisons, as degenerate
// edges will produce 0 from Fast2DPointSideTest(), so if A=B=C then all points are "inside". 
//GRADIENTSPACECORE_API
//bool IsPointInsideOrOnTriangle(const Vector2d& A, const Vector2d& B, const Vector2d& C, const Vector2d& Point);


//! calculate Signed Area of Triangle ABC
GRADIENTSPACECORE_API
inline double TriangleSignedArea2d(const Vector2d& A, const Vector2d& B, const Vector2d& C) {
	// https://en.wikipedia.org/wiki/Area_of_a_triangle	
	return 0.5 * ((A.X-C.X)*(B.Y-A.Y) - (A.X-B.X)*(C.Y-A.Y));
}


/**
 *
 */
template<typename RealType>
struct Triangle2
{
	union {
		struct {
			Vector2<RealType> A;
			Vector2<RealType> B;
			Vector2<RealType> C;
		};
		Vector2<RealType> ABC[3] = { {}, {}, {} };
	};

	Triangle2();
	Triangle2(const Vector2<RealType>& Ain, const Vector2<RealType>& Bin, const Vector2<RealType>& Cin);

	Vector2<RealType>& operator[](int index) {
		return ABC[index];
	}
	Vector2<RealType> operator[](int index) const {
		return ABC[index];
	}

	Vector2<RealType> Centroid() const;
	RealType SignedArea() const;
	RealType Area() const;
	AxisBox2<RealType> Bounds() const;

	Vector2<RealType> BaryPoint(RealType WeightA, RealType WeightB, RealType WeightC) const;
	Vector2<RealType> BaryPoint(const Vector3<RealType>& BaryCoords) const;
	Vector2<RealType> EdgePoint(int WhichEdge, RealType EdgeUnitParamT) const;

	Vector3<RealType> GetBaryCoords(const Vector2<RealType>& Point) const;

	bool IsPointInside(const Vector2<RealType>& Point) const;

	RealType EdgeDistanceSquared(const Vector2<RealType>& Point, int& WhichEdge, RealType& EdgeUnitParamT) const;
	RealType DistanceSquared(const Vector2<RealType>& Point, int& WhichEdge, RealType& EdgeUnitParamT) const;
	RealType SignedDistance(const Vector2<RealType>& Point, int& WhichEdge, RealType& EdgeUnitParamT) const;

	// float/double conversion cast
	template<typename RealType2>
	explicit operator Triangle2<RealType2>() const {
		return Triangle2<RealType2>((Vector2<RealType2>)A, (Vector2<RealType2>)B, (Vector2<RealType2>)C);
	}

};
typedef Triangle2<float> Triangle2f;
typedef Triangle2<double> Triangle2d;

template<typename RealType>
Triangle2<RealType>::Triangle2()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<Triangle2<RealType>>());
	static_assert(std::is_trivially_copyable<Triangle2<RealType>>());
	static_assert(std::is_floating_point<RealType>());
}

template<typename RealType>
Triangle2<RealType>::Triangle2(const Vector2<RealType>& Ain, const Vector2<RealType>& Bin, const Vector2<RealType>& Cin)
	: A(Ain), B(Bin), C(Cin)
{
}

template<typename RealType>
Vector2<RealType> Triangle2<RealType>::Centroid() const
{
	return (A + B + C) / (RealType)3;
}

template<typename RealType>
RealType Triangle2<RealType>::SignedArea() const
{
	return (RealType)TriangleSignedArea2d((Vector2d)A, (Vector2d)B, (Vector2d)C);
}

template<typename RealType>
RealType Triangle2<RealType>::Area() const
{
	return (RealType)GS::Abs(TriangleSignedArea2d((Vector2d)A, (Vector2d)B, (Vector2d)C));
}

template<typename RealType>
AxisBox2<RealType> Triangle2<RealType>::Bounds() const
{
	return AxisBox2<RealType>(A, B, C);
}

template<typename RealType>
Vector2<RealType> Triangle2<RealType>::BaryPoint(RealType WeightA, RealType WeightB, RealType WeightC) const 
{
	return WeightA * A + WeightB * B + WeightC * C;
}

template<typename RealType>
Vector2<RealType> Triangle2<RealType>::BaryPoint(const Vector3<RealType>& BaryCoords) const
{
	return BaryCoords.X * A + BaryCoords.Y * B + BaryCoords.Z * C;
}

template<typename RealType>
Vector2<RealType> Triangle2<RealType>::EdgePoint(int WhichEdge, RealType EdgeUnitParamT) const
{
	WhichEdge = GS::Clamp(WhichEdge, 0, 2);
	return ((RealType)1-EdgeUnitParamT)*ABC[WhichEdge] + (EdgeUnitParamT)*ABC[(WhichEdge+1)%3];
}


template<typename RealType>
Vector3<RealType> Triangle2<RealType>::GetBaryCoords(const Vector2<RealType>& Point) const
{
	Vector3d result = GetTriBaryCoords2d((Vector2d)A, (Vector2d)B, (Vector2d)C, (Vector2d)Point);
	return (Vector3<RealType>)result;
}

template<typename RealType>
bool Triangle2<RealType>::IsPointInside(const Vector2<RealType>& Point) const
{
	return IsPointInsideTriangle((Vector2d)A, (Vector2d)B, (Vector2d)C, (Vector2d)Point);
}

template<typename RealType>
RealType Triangle2<RealType>::EdgeDistanceSquared(const Vector2<RealType>& Point, int& WhichEdge, RealType& EdgeUnitParamT) const
{
	Segment2d Edges[3] = { Segment2d(A,B), Segment2d(B,C), Segment2d(C,A) };
	RealType EdgeParams[3], EdgeDistSqrs[3];
	EdgeDistSqrs[0] = Edges[0].DistanceSquared(Point, EdgeParams[0]);
	EdgeDistSqrs[1] = Edges[1].DistanceSquared(Point, EdgeParams[1]);
	EdgeDistSqrs[2] = Edges[2].DistanceSquared(Point, EdgeParams[2]);
	WhichEdge = GS::Min3Index(EdgeDistSqrs[0], EdgeDistSqrs[1], EdgeDistSqrs[2]);
	EdgeUnitParamT = Edges[WhichEdge].ToUnitParameter(EdgeParams[WhichEdge]);
	return EdgeDistSqrs[WhichEdge];
}

template<typename RealType>
RealType Triangle2<RealType>::DistanceSquared(const Vector2<RealType>& Point, int& WhichEdge, RealType& EdgeUnitParamT) const
{
	if (IsPointInsideTriangle(Point)) {
		WhichEdge = 0, EdgeUnitParamT = (RealType)0; return (RealType)0;
	}
	return EdgeDistanceSquared(Point, WhichEdge, EdgeUnitParamT);
}


template<typename RealType>
RealType Triangle2<RealType>::SignedDistance(const Vector2<RealType>& Point, int& WhichEdge, RealType& EdgeUnitParamT) const
{
	RealType Inside = IsPointInside(Point) ? (RealType)-1 : (RealType)1;
	return Inside*GS::Sqrt(EdgeDistanceSquared(Point, WhichEdge, EdgeUnitParamT));
}




} // end namespace GS