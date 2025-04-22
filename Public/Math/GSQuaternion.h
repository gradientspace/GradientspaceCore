// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSVector3.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Quaternion.h"
#include "Math/Quat.h"
#endif


namespace GS
{

template<typename RealType>
struct Quaternion
{
	union
	{
		struct
		{
			RealType X;
			RealType Y;
			RealType Z;
			RealType W;
		};
		RealType XYZW[4] = { {}, {}, {}, {} };
	};

	Quaternion();
	Quaternion(RealType x, RealType y, RealType z, RealType w);
	Quaternion(const Vector3<RealType>& Axis, RealType Angle, bool bAngleIsDegrees);
	Quaternion(const Vector3<RealType>& From, const Vector3<RealType>& To);

	static Quaternion Identity() { return Quaternion((RealType)0, (RealType)0, (RealType)0, (RealType)1); }

	constexpr RealType SquaredLength() const {
		return X*X + Y*Y + Z*Z + W*W;
	}
	constexpr RealType Length() const {
		return GS::Sqrt(X*X + Y*Y + Z*Z + W*W);
	}

	RealType Normalize(RealType Epsilon = RealMath<RealType>::NormalizeTolerance());
	Quaternion Normalized(RealType Epsilon = RealMath<RealType>::NormalizeTolerance()) const;

	constexpr bool IsNormalized(const RealType Tolerance = RealMath<RealType>::ZeroTolerance() ) const {
		return GS::Abs( (X*X + Y*Y + Z*Z + W*W) - (RealType)1 ) < Tolerance;
	}

	Vector3<RealType> AxisX() const {
		return Vector3<RealType>((RealType)1 - (RealType)2*(Y*Y + Z*Z), (RealType)2*(Y*X + Z*W), (RealType)2*(Z*X - Y*W));
	}

	Vector3<RealType> AxisY() const	{
		return Vector3<RealType>((RealType)2*(Y*X - Z*W), (RealType)1 - (RealType)2*(X*X + Z*Z), (RealType)2*(Z*Y + X*W));
	}

	Vector3<RealType> AxisZ() const	{
		return Vector3<RealType>((RealType)2*(Z*X + Y*W), (RealType)2*(Z*Y - X*W), (RealType)1 - (RealType)2*(X*X + Y*Y));
	}

	Vector3<RealType> GetFrameAxis(int Index) const {
		switch (Index) {
			case 0: return Vector3<RealType>((RealType)1 - (RealType)2*(Y*Y + Z*Z), (RealType)2*(Y*X + Z*W), (RealType)2*(Z*X - Y*W));
			case 1: return Vector3<RealType>((RealType)2*(Y*X - Z*W), (RealType)1 - (RealType)2*(X*X + Z*Z), (RealType)2*(Z*Y + X*W));
			default: return Vector3<RealType>((RealType)2*(Z*X + Y*W), (RealType)2*(Z*Y - X*W), (RealType)1 - (RealType)2*(X*X + Y*Y));
		}
	}

	constexpr Quaternion operator-() const {
		return Quaternion(-X, -Y, -Z, -W);
	}

	RealType Dot(const Quaternion& Other) const;

	bool IsSameOrientation(const Quaternion& Other, RealType Epsilon = RealMath<RealType>::ZeroTolerance() ) const;

	void SetToAxisAngleDeg(const Vector3<RealType>& Axis, RealType AngleDeg);
	void SetToAxisAngleRad(const Vector3<RealType>& Axis, RealType AngleRad);
	void SetFromTo(const Vector3<RealType>& AxisFrom, const Vector3<RealType>& AxisTo);

	Vector3<RealType> InverseMultiply(const Vector3<RealType>& Vec) const;

	//! convert quaternion back to axis/angle form (todo: unit-quaternion optimization)
	void ExtractAxisAngleRad(Vector3<RealType>& AxisOut, RealType& RadiansOut) const;

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	explicit inline operator UE::Geometry::TQuaternion<RealType>() const {
		return UE::Geometry::TQuaternion<RealType>((RealType)X, (RealType)Y, (RealType)Z, (RealType)W);
	}
	explicit inline Quaternion(const UE::Geometry::TQuaternion<RealType>& Quat)
		: X(Quat.X), Y(Quat.Y), Z(Quat.Z), W(Quat.W) {}

	explicit inline operator FQuat4f() const {
		return FQuat4f((float)X, (float)Y, (float)Z, (float)W);
	}
	explicit inline operator FQuat4d() const {
		return FQuat4d((double)X, (double)Y, (double)Z, (double)W);
	}
	explicit inline Quaternion(const FQuat4f& Quat)
		: X((RealType)Quat.X), Y((RealType)Quat.Y), Z((RealType)Quat.Z), W((RealType)Quat.W) {}
	explicit inline Quaternion(const FQuat4d& Quat)
		: X((RealType)Quat.X), Y((RealType)Quat.Y), Z((RealType)Quat.Z), W((RealType)Quat.W) {}
#endif

};
typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniond;

template<typename RealType>
Quaternion<RealType>::Quaternion()
{
}

template<typename RealType>
Quaternion<RealType>::Quaternion(RealType x, RealType y, RealType z, RealType w)
	: X(x), Y(y), Z(z), W(w)
{}

template<typename RealType>
Quaternion<RealType>::Quaternion(const Vector3<RealType>& Axis, RealType Angle, bool bAngleIsDegrees) {
	RealType AngleRad = (bAngleIsDegrees) ? (RealMath<RealType>::DegToRad() * Angle) : Angle;
	SetToAxisAngleRad(Axis, AngleRad);
}

template<typename RealType>
Quaternion<RealType>::Quaternion(const Vector3<RealType>& From, const Vector3<RealType>& To) {
	SetFromTo(From, To);
}

template<typename RealType>
RealType Quaternion<RealType>::Normalize(RealType Epsilon)
{
	// todo want to set epsilon to avoid creating denorms??
	RealType len = GS::Sqrt(X * X + Y * Y + Z * Z + W * W);
	RealType scale = (len > Epsilon) ? ((RealType)1 / len) : 0;
	X *= scale; Y *= scale; Z *= scale; W *= scale;
	return len;
}

template<typename RealType>
Quaternion<RealType> Quaternion<RealType>::Normalized(RealType Epsilon) const
{
	// todo want to set epsilon to avoid creating denorms??
	RealType len = GS::Sqrt(X * X + Y * Y + Z * Z + W * W);
	RealType scale = (len > Epsilon) ? ((RealType)1 / len) : 0;
	// todo should we be returning (0,0,0,1) in length-0 case?
	return Quaternion(X * scale, Y * scale, Z * scale, W * scale);
}

template<typename RealType>
RealType Quaternion<RealType>::Dot(const Quaternion<RealType>& Other) const
{
	return X*Other.X + Y*Other.Y + Z*Other.Z + W*Other.W;
}

template<typename RealType>
bool Quaternion<RealType>::IsSameOrientation(const Quaternion<RealType>& Other, RealType Epsilon) const
{
	// https://gamedev.stackexchange.com/questions/75072/how-can-i-compare-two-quaternions-for-logical-equality
	// idea is Q and -Q are same orientations, so dot may be +1 or -1
	RealType dot = this->Dot(Other);
	return GS::Abs(dot) > (1.0 - Epsilon);
}


template<typename RealType>
void Quaternion<RealType>::SetToAxisAngleRad(const Vector3<RealType>& Axis, RealType AngleRad)
{
	RealType halfAngle = (RealType)0.5 * AngleRad;
	RealType SinHalfAngle = GS::Sin(halfAngle);
	W = GS::Cos(halfAngle);
	X = (SinHalfAngle * Axis.X);
	Y = (SinHalfAngle * Axis.Y);
	Z = (SinHalfAngle * Axis.Z);
}
template<typename RealType>
void Quaternion<RealType>::SetToAxisAngleDeg(const Vector3<RealType>& Axis, RealType AngleDeg)
{
	return SetToAxisAngleDeg(Axis, RealMath<RealType>::DegToRad() * AngleDeg);
}

template<typename RealType>
void Quaternion<RealType>::SetFromTo(const Vector3<RealType>& AxisFrom, const Vector3<RealType>& AxisTo)
{
	// [TODO] this page seems to have optimized version:
	//    http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors

	// [RMS] not ideal to explicitly normalize here, but if we don't,
	//   output quaternion is not normalized and this causes problems,
	//   eg like drift if we do repeated SetFromTo()
	Vector3<RealType> from = AxisFrom.Normalized(), to = AxisTo.Normalized();
	Vector3<RealType> bisector = (from + to).Normalized();
	W = from.Dot(bisector);
	if (W != 0) {
		Vector3<RealType> cross = Cross(from, bisector);
		X = cross.X;
		Y = cross.Y;
		Z = cross.Z;
	} else {
		RealType invLength;
		if (GS::Abs(from.X) >= GS::Abs(from.Y)) {
			// V1.x or V1.z is the largest magnitude component.
			invLength = (RealType)( (RealType)1 / GS::Sqrt(from.X * from.X + from.Z * from.Z) );
			X = -from.Z * invLength;
			Y = 0;
			Z = +from.X * invLength;
		} else {
			// V1.y or V1.z is the largest magnitude component.
			invLength = (RealType)( (RealType)1 / GS::Sqrt(from.Y * from.Y + from.Z * from.Z) );
			X = 0;
			Y = +from.Z * invLength;
			Z = -from.Y * invLength;
		}
	}
	Normalize();   // just to be safe...
}


template<typename RealType>
Vector3<RealType> Quaternion<RealType>::InverseMultiply(const Vector3<RealType>& Vec) const
{
	// this could be avoided if we knew quat was unit length...
	RealType Length = SquaredLength();
	if (Length > 0)
	{
		RealType InvLength = (RealType)1 / Length;
		RealType qX = -X * InvLength, qY = -Y * InvLength, qZ = -Z * InvLength, qW = W * InvLength;
		RealType twoX = (RealType)2 * qX; RealType twoY = (RealType)2 * qY; RealType twoZ = (RealType)2 * qZ;
		RealType twoWX = twoX * qW; RealType twoWY = twoY * qW; RealType twoWZ = twoZ * qW;
		RealType twoXX = twoX * qX; RealType twoXY = twoY * qX; RealType twoXZ = twoZ * qX;
		RealType twoYY = twoY * qY; RealType twoYZ = twoZ * qY; RealType twoZZ = twoZ * qZ;
		return Vector3<RealType>(
			Vec.X * ((RealType)1 - (twoYY + twoZZ)) + Vec.Y * (twoXY - twoWZ) + Vec.Z * (twoXZ + twoWY),
			Vec.X * (twoXY + twoWZ) + Vec.Y * ((RealType)1 - (twoXX + twoZZ)) + Vec.Z * (twoYZ - twoWX),
			Vec.X * (twoXZ - twoWY) + Vec.Y * (twoYZ + twoWX) + Vec.Z * ((RealType)1 - (twoXX + twoYY)));
	}
	return Vector3<RealType>::Zero();
}


template<typename RealType>
inline Quaternion<RealType> operator*(const Quaternion<RealType>& a, const Quaternion<RealType>& b)
{
	RealType W = a.W * b.W - a.X * b.X - a.Y * b.Y - a.Z * b.Z;
	RealType X = a.W * b.X + a.X * b.W + a.Y * b.Z - a.Z * b.Y;
	RealType Y = a.W * b.Y + a.Y * b.W + a.Z * b.X - a.X * b.Z;
	RealType Z = a.W * b.Z + a.Z * b.W + a.X * b.Y - a.Y * b.X;
	return Quaternion<RealType>(X,Y,Z,W);
}


template<typename RealType>
inline Vector3<RealType> operator*(const Quaternion<RealType>& Quat, const Vector3<RealType>& Vec)
{
	// investigate Eigen implementation? seems to avoid all these temporaries...  (QuaternionBase<Derived>::_transformVector)
	//Vector3 uv = this->vec().cross(v);
	//uv += uv;
	//return v + this->w() * uv + this->vec().cross(uv);

	// TODO unit-multiply variant...

	// inline-expansion of rotation matrix, result is Mat * Vec
	RealType twoX = (RealType)2*Quat.X; RealType twoY = (RealType)2*Quat.Y; RealType twoZ = (RealType)2*Quat.Z;
	RealType twoWX = twoX * Quat.W; RealType twoWY = twoY * Quat.W; RealType twoWZ = twoZ * Quat.W;
	RealType twoXX = twoX * Quat.X; RealType twoXY = twoY * Quat.X; RealType twoXZ = twoZ * Quat.X;
	RealType twoYY = twoY * Quat.Y; RealType twoYZ = twoZ * Quat.Y; RealType twoZZ = twoZ * Quat.Z;
	return Vector3<RealType>(
		Vec.X * ((RealType)1 - (twoYY + twoZZ)) + Vec.Y * (twoXY - twoWZ) + Vec.Z * (twoXZ + twoWY),
		Vec.X * (twoXY + twoWZ) + Vec.Y * ((RealType)1 - (twoXX + twoZZ)) + Vec.Z * (twoYZ - twoWX),
		Vec.X * (twoXZ - twoWY) + Vec.Y * (twoYZ + twoWX) + Vec.Z * ((RealType)1 - (twoXX + twoYY)));
}


template<typename RealType>
void Quaternion<RealType>::ExtractAxisAngleRad(Vector3<RealType>& AxisOut, RealType& RadiansOut) const
{
	RealType Norm = GS::Sqrt(X*X + Y*Y + Z*Z);
	AxisOut.X = X / Norm;
	AxisOut.Y = Y / Norm;
	AxisOut.Z = Z / Norm;

	// wikipedia says this is more numerically stable than 2 * acos(w)
	RadiansOut = (RealType)2 * GS::ATan2(Norm, W);
}


} // end namespace GS
