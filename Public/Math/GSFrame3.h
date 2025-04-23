// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSQuaternion.h"
#include "Math/GSRay3.h"
#include "Core/GSResult.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "FrameTypes.h"
#endif

namespace GS
{

template<typename RealType>
struct Frame3
{
	Vector3<RealType> Origin;
	Quaternion<RealType> Rotation;

	Frame3();
	explicit Frame3(const Vector3<RealType>& OriginIn);
	explicit Frame3(const Vector3<RealType>& OriginIn, const Quaternion<RealType>& RotationIn);
	explicit Frame3(const Vector3<RealType>& OriginIn, const Vector3<RealType>& AxisZ);

	static Frame3 Identity() { return Frame3(Vector3<RealType>::Zero(), Quaternion<RealType>::Identity()); }

	Vector3<RealType> X() const { return Rotation.AxisX(); }
	Vector3<RealType> Y() const { return Rotation.AxisY(); }
	Vector3<RealType> Z() const { return Rotation.AxisZ(); }
	Vector3<RealType> GetAxis(int Index) const { return Rotation.GetFrameAxis(Index); }

	Vector3<RealType> ToLocalPoint(const Vector3<RealType>& WorldPoint) const;
	Vector3<RealType> ToWorldPoint(const Vector3<RealType>& LocalPoint) const;

	Vector3<RealType> ToLocalVector(const Vector3<RealType>& WorldVector) const;
	Vector3<RealType> ToWorldVector(const Vector3<RealType>& LocalVector) const;

	Quaternion<RealType> ToLocalQuat(const Quaternion<RealType>& WorldQuat) const;
	Quaternion<RealType> ToWorldQuat(const Quaternion<RealType>& LocalQuat) const;

	Ray3<RealType> ToLocalRay(const Ray3<RealType>& WorldRay) const;
	Ray3<RealType> ToWorldRay(const Ray3<RealType>& LocalRay) const;

	Frame3<RealType> ToLocalFrame(const Frame3<RealType>& WorldFrame) const;
	Frame3<RealType> ToWorldFrame(const Frame3<RealType>& LocalFrame) const;

	Vector2<RealType> ToPlaneXY(const Vector3<RealType>& Point) const;

	ResultOrFail<Vector3<RealType>> FindRayPlaneIntersection(const Ray3<RealType>& Ray, int NormalAxisIndex) const;

	void AlignAxis(int AxisIndex, const Vector3<RealType>& ToDirection);

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	explicit inline operator UE::Geometry::TFrame3<RealType>() const {
		return UE::Geometry::TFrame3<RealType>( (UE::Math::TVector<RealType>)Origin, (UE::Geometry::TQuaternion<RealType>)Rotation );
	}
	explicit inline Frame3(const UE::Geometry::TFrame3<RealType>& Frame)
		: Origin(Frame.Origin), Rotation(Frame.Rotation) {}
#endif

};
typedef Frame3<float> Frame3f;
typedef Frame3<double> Frame3d;

template<typename RealType>
Frame3<RealType>::Frame3()
	: Origin(Vector3<RealType>::Zero()), Rotation(Quaternion<RealType>::Identity())
{}

template<typename RealType>
Frame3<RealType>::Frame3(const Vector3<RealType>& OriginIn)
	: Origin(OriginIn), Rotation(Quaternion<RealType>::Identity())
{}

template<typename RealType>
Frame3<RealType>::Frame3(const Vector3<RealType>& OriginIn, const Quaternion<RealType>& RotationIn)
	: Origin(OriginIn), Rotation(RotationIn)
{}

template<typename RealType>
Frame3<RealType>::Frame3(const Vector3<RealType>& OriginIn, const Vector3<RealType>& AxisZ)
	: Origin(OriginIn), Rotation(Vector3<RealType>::UnitZ(), AxisZ)
{}


template<typename RealType>
Vector3<RealType> Frame3<RealType>::ToLocalPoint(const Vector3<RealType>& Point) const
{
	return Rotation.InverseMultiply(Point - Origin);
}
template<typename RealType>
Vector3<RealType> Frame3<RealType>::ToWorldPoint(const Vector3<RealType>& Point) const
{
	return Rotation * Point + Origin;
}

template<typename RealType>
Vector3<RealType> Frame3<RealType>::ToLocalVector(const Vector3<RealType>& Vec) const
{
	return Rotation.InverseMultiply(Vec);
}
template<typename RealType>
Vector3<RealType> Frame3<RealType>::ToWorldVector(const Vector3<RealType>& Vec) const
{
	return Rotation * Vec;
}

template<typename RealType>
Quaternion<RealType> Frame3<RealType>::ToLocalQuat(const Quaternion<RealType>& WorldQuat) const
{
	return Rotation.Inverse() * WorldQuat;
}
template<typename RealType>
Quaternion<RealType> Frame3<RealType>::ToWorldQuat(const Quaternion<RealType>& LocalQuat) const
{
	return Rotation * LocalQuat;
}

template<typename RealType>
Ray3<RealType> Frame3<RealType>::ToLocalRay(const Ray3<RealType>& WorldRay) const
{
	return Ray3<RealType>( ToLocalPoint(WorldRay.Origin), ToLocalVector(WorldRay.Direction) );
}
template<typename RealType>
Ray3<RealType> Frame3<RealType>::ToWorldRay(const Ray3<RealType>& LocalRay) const
{
	return Ray3<RealType>( ToWorldRay(LocalRay.Origin), ToWorldVector(LocalRay.Direction) );
}

template<typename RealType>
Frame3<RealType> Frame3<RealType>::ToLocalFrame(const Frame3<RealType>& WorldFrame) const
{
	return Frame3<RealType>(ToLocalPoint(WorldFrame.Origin), ToLocalQuat(WorldFrame.Rotation));
}
template<typename RealType>
Frame3<RealType> Frame3<RealType>::ToWorldFrame(const Frame3<RealType>& LocalFrame) const
{
	return Frame3<RealType>(ToWorldPoint(LocalFrame.Origin), ToWorldQuat(LocalFrame.Rotation));
}

template<typename RealType>
Vector2<RealType> Frame3<RealType>::ToPlaneXY(const Vector3<RealType>& Point) const 
{
	Vector3<RealType> Vec(Point - Origin);
	RealType U = Vec.Dot(Rotation.AxisX());
	RealType V = Vec.Dot(Rotation.AxisY());
	return Vector2<RealType>(U, V);
}

template<typename RealType>
ResultOrFail<Vector3<RealType>> Frame3<RealType>::FindRayPlaneIntersection(const Ray3<RealType>& Ray, int NormalAxisIndex) const
{
	Vector3<RealType> N = Rotation.GetFrameAxis(NormalAxisIndex);
	RealType d = -Origin.Dot(N);
	RealType DirDotN = Ray.Direction.Dot(N);
	if (GS::ToleranceEqual(DirDotN, (RealType)0))
		return ResultOrFail<Vector3<RealType>>();
	RealType t = -(Ray.Origin.Dot(N) + d) / DirDotN;
	if (t < 0)
		return ResultOrFail<Vector3<RealType>>();
	return Ray.PointAt(t);
}


template<typename RealType>
void Frame3<RealType>::AlignAxis(int AxisIndex, const Vector3<RealType>& ToDirection)
{
	Quaterniond AlignRotation(GetAxis(AxisIndex), ToDirection.Normalized() );
	this->Rotation = AlignRotation * Rotation;
}




} // end namespace GS
