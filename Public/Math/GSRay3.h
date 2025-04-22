// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSVector3.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "BoxTypes.h"
#include "Math/Box.h"
#endif

namespace GS
{

/**
 * 3D ray in (origin, unit direction) representation
 */
template<typename RealType>
struct Ray3
{
	Vector3<RealType> Origin;
	//! normalized direction vector
	Vector3<RealType> Direction;

	Ray3();
	Ray3(const Vector3<RealType>& Origin, const Vector3<RealType>& NormalizedDirection);

	constexpr static Ray3 UnitX() { return Ray3(Vector3<RealType>::Zero(), Vector3<RealType>::UnitX()); }
	constexpr static Ray3 UnitY() { return Ray3(Vector3<RealType>::Zero(), Vector3<RealType>::UnitY()); }
	constexpr static Ray3 UnitZ() { return Ray3(Vector3<RealType>::Zero(), Vector3<RealType>::UnitZ()); }
	constexpr static RealType SafeMaxDist() { return RealConstants<RealType>::SafeMaxValue(); }

	Vector3<RealType> PointAt(RealType Distance) const {
		return Origin + Distance*Direction;
	}

	//! return the ray parameter (ie distance to ray origin) at the closest position to Point along the ray
	RealType GetParameter(const Vector3<RealType>& Point) const {
		return (Point-Origin).Dot(Direction);
	}

	Vector3<RealType> ClosestPoint(const Vector3<RealType>& Point) const {
		RealType RayParam = (Point - Origin).Dot(Direction);
		if (RayParam <= 0)
			return Origin;
		return Origin + RayParam*Direction;
	}

	RealType DistanceSquared(const Vector3<RealType>& Point) const {
		RealType RayParam = (Point - Origin).Dot(Direction);
		if (RayParam <= 0)
			return Point.DistanceSquared(Origin);
		return Point.DistanceSquared(Origin + RayParam*Direction);
	}

	RealType Distance(const Vector3<RealType>& Point) const {
		return GS::Sqrt(DistanceSquared);
	}

	//! float/double conversion operator
	template<typename RealType2>
	explicit operator Ray3<RealType2>() const {
		return Ray3<RealType2>( (Vector3<RealType2>)Origin, (Vector3<RealType2>)Direction );
	}


	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	explicit inline operator UE::Math::TRay<RealType>() const {
		return UE::Math::TRay<RealType>((UE::Math::TVector<RealType>)Origin, (UE::Math::TVector<RealType>)Direction);
	}
	explicit inline Ray3(const UE::Math::TRay<RealType>& Ray) 
		: Origin((Vector3<RealType>)Ray.Origin), Direction((Vector3<RealType>)Ray.Direction)
	{}
#endif


};
typedef Ray3<float> Ray3f;
typedef Ray3<double> Ray3d;


template<typename RealType>
Ray3<RealType>::Ray3()
{}

template<typename RealType>
Ray3<RealType>::Ray3(const Vector3<RealType>& origin, const Vector3<RealType>& normalizedDirection)
	: Origin(origin), Direction(normalizedDirection)
{}




} // end namespace GS
