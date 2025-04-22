// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSHash.h"
#include "Math/GSMath.h"
#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSAxisBox2.h"

namespace GS
{
template<typename RealType>
struct Segment2
{
	Vector2<RealType> Origin = Vector2<RealType>::Zero();
	Vector2<RealType> Direction = Vector2<RealType>::UnitX();
	RealType Extent = (RealType)0;

	Segment2();
	Segment2(const Vector2<RealType>& Start, const Vector2<RealType>& End);
	Segment2(const Vector2<RealType>& Origin, const Vector2<RealType>& Direction, RealType Extent);

	RealType Length() const;
	RealType LengthSquared() const;
	AxisBox2<RealType> Bounds() const;
	RealType ToUnitParameter(RealType ParameterT) const;
	RealType FromUnitParameter(RealType UnitParameterT) const;

	Vector2<RealType> Endpoint(int Index) const;
	Vector2<RealType> PointAt(RealType ParameterT) const;
	Vector2<RealType> PointBetween(RealType UnitParameterT) const;

	RealType Distance(const Vector2<RealType>& Point, RealType& ParameterT) const;
	RealType DistanceSquared(const Vector2<RealType>& Point, RealType& ParameterT) const;
	RealType DistanceSquared(const Vector2<RealType>& Point) const;

	RealType GetProjection(const Vector2<RealType>& Point) const;
	Vector2<RealType> NearestPoint(const Vector2<RealType>& Point, RealType& ParameterT) const;
};
typedef Segment2<float> Segment2f;
typedef Segment2<double> Segment2d;

template<typename RealType>
Segment2<RealType>::Segment2()
{
	static_assert(std::is_standard_layout<Segment2<RealType>>());
	static_assert(std::is_trivially_copyable<Segment2<RealType>>());
	static_assert(std::is_floating_point<RealType>());
}

template<typename RealType>
Segment2<RealType>::Segment2(const Vector2<RealType>& Start, const Vector2<RealType>& End)
{
	Origin = (RealType)0.5 * (Start + End);
	Direction = (End - Start);
	Extent = (RealType)0.5 * Direction.Normalize();
}

template<typename RealType>
Segment2<RealType>::Segment2(const Vector2<RealType>& OriginIn, const Vector2<RealType>& DirectionIn, RealType ExtentIn)
	: Origin(OriginIn), Direction(DirectionIn), Extent(ExtentIn)
{
}

template<typename RealType>
RealType Segment2<RealType>::Length() const
{
	return (RealType)2 * Extent;
}

template<typename RealType>
RealType Segment2<RealType>::LengthSquared() const
{
	return (RealType)4 * Extent * Extent;
}

template<typename RealType>
AxisBox2<RealType> Segment2<RealType>::Bounds() const
{
	Vector2<RealType> A = Origin - Extent * Direction;
	Vector2<RealType> B = Origin + Extent * Direction;
	return AxisBox2<RealType>{ {GS::Min(A.X, B.X), GS::Min(A.Y, B.Y)}, {GS::Max(A.X, B.X), GS::Max(A.Y, B.Y)} };
}

template<typename RealType>
Vector2<RealType> Segment2<RealType>::Endpoint(int Index) const
{
	RealType Sign = (Index == 1) ? (RealType)1 : (RealType)-1;
	return Origin + (Sign*Extent)*Direction;
}

template<typename RealType>
Vector2<RealType> Segment2<RealType>::PointAt(RealType ParameterT) const
{
	return Origin + ParameterT * Direction;
}

template<typename RealType>
Vector2<RealType> Segment2<RealType>::PointBetween(RealType UnitParameterT) const
{
	return Origin + ((RealType)2*UnitParameterT - (RealType)1) * Extent * Direction;
}

template<typename RealType>
RealType Segment2<RealType>::ToUnitParameter(RealType ParameterT) const
{
	return GS::Clamp( ((ParameterT / Extent) + (RealType)1) * (RealType)0.5, (RealType)0, (RealType)1);
}

template<typename RealType>
RealType Segment2<RealType>::FromUnitParameter(RealType UnitParameterT) const
{
	return ((RealType)2 * UnitParameterT - (RealType)1) * Extent;
}


template<typename RealType>
RealType Segment2<RealType>::Distance(const Vector2<RealType>& Point, RealType& ParameterT) const
{
	return GS::Sqrt(DistanceSquared(Point, ParameterT));
}

template<typename RealType>
RealType Segment2<RealType>::DistanceSquared(const Vector2<RealType>& Point, RealType& ParameterT) const
{
	ParameterT = (Point - Origin).Dot(Direction);
	ParameterT = GS::Clamp(ParameterT, -Extent, Extent);
	return GS::DistanceSquared(Point, Origin + ParameterT*Direction);
}

template<typename RealType>
RealType Segment2<RealType>::DistanceSquared(const Vector2<RealType>& Point) const
{
	RealType ParameterT;
	return DistanceSquared(Point, ParameterT);
}

template<typename RealType>
RealType Segment2<RealType>::GetProjection(const Vector2<RealType>& Point) const
{
	return (Point - Origin).Dot(Direction);
}

template<typename RealType>
Vector2<RealType> Segment2<RealType>::NearestPoint(const Vector2<RealType>& Point, RealType& ParameterT) const
{
	ParameterT = (Point - Origin).Dot(Direction);
	ParameterT = GS::Clamp(ParameterT, -Extent, Extent);
	return Origin + ParameterT * Direction;
}





}