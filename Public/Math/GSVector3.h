// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSHash.h"

#ifdef GEOMETRYCORE_API
#include "Math/Vector.h"
#include "IntVectorTypes.h"
#endif

#include <xhash>  // works for std::hash defined at bottom? or is <unordered_set> required?

namespace GS
{

template<typename RealType>
struct Vector3
{
	union {
		struct {
			RealType X;
			RealType Y;
			RealType Z;
		};
		RealType XYZ[3] = { {}, {}, {} };
	};

	Vector3();
	explicit Vector3(RealType InitialValue);
	explicit Vector3(RealType x, RealType y, RealType z);
	explicit Vector3(const RealType* Values);

	static Vector3 One() { return Vector3((RealType)1); }
	static Vector3 Zero() { return Vector3((RealType)0); }
	static Vector3 UnitX() { return Vector3((RealType)1, (RealType)0, (RealType)0); }
	static Vector3 UnitY() { return Vector3((RealType)0, (RealType)1, (RealType)0); }
	static Vector3 UnitZ() { return Vector3((RealType)0, (RealType)0, (RealType)1); }
	static Vector3 MakeUnit(int Axis, RealType Sign = (RealType)1.0) { Vector3 V = Zero(); V[Axis] = Sign*1; return V; }


	RealType& operator[](int index)	{
		return XYZ[index];
	}

	RealType operator[](int index) const {
		return XYZ[index];
	}

	RealType* AsPointer() {
		return &XYZ[0];
	}
	const RealType* AsPointer() const {
		return &XYZ[0];
	}

	constexpr RealType SquaredLength() const {
		return X * X + Y * Y + Z * Z;
	}
	constexpr RealType Length() const {
		return GS::Sqrt(X*X + Y*Y + Z*Z);
	}
	constexpr RealType DistanceSquared(const Vector3& Vec) const {
		RealType dx = (Vec.X - X), dy = (Vec.Y - Y), dz = (Vec.Z - Z);
		return dx*dx + dy*dy + dz*dz;
	}
	constexpr RealType Distance(const Vector3& Vec) const {
		RealType dx = (Vec.X - X), dy = (Vec.Y - Y), dz = (Vec.Z - Z);
		return GS::Sqrt(dx*dx + dy*dy + dz*dz);
	}
	constexpr RealType Dot(const Vector3& Vec) const {
		return X*Vec.X + Y*Vec.Y + Z*Vec.Z;
	}
	constexpr Vector3 Abs() const {
		return Vector3(GS::Abs(X), GS::Abs(Y), GS::Abs(Z));
	}
	constexpr RealType AbsMax() const {
		return GS::Max3(GS::Abs(X), GS::Abs(Y), GS::Abs(Z));
	}

	RealType Normalize(RealType Epsilon = RealMath<RealType>::NormalizeTolerance())
	{
		// todo want to set epsilon to avoid creating denorms??
		RealType len = GS::Sqrt(X*X + Y*Y + Z*Z);
		RealType scale = (len > Epsilon) ? ((RealType)1 / len) : 0;
		X *= scale; Y *= scale; Z *= scale;
		return len;
	}
	Vector3 Normalized(RealType Epsilon = RealMath<RealType>::NormalizeTolerance()) const
	{
		RealType len = GS::Sqrt(X * X + Y * Y + Z * Z);
		RealType scale = (len > Epsilon) ? ((RealType)1 / len) : 0;
		return Vector3(X * scale, Y * scale, Z * scale);
	}

	constexpr bool IsNormalized(const RealType Tolerance = RealMath<RealType>::ZeroTolerance() ) const {
		return GS::Abs( (X*X + Y*Y + Z*Z) - (RealType)1 ) < Tolerance;
	}

	constexpr Vector3 operator-() const {
		return Vector3(-X, -Y, -Z);
	}

	constexpr Vector3 operator+(const Vector3& Vec) const	{
		return Vector3(X + Vec.X, Y + Vec.Y, Z + Vec.Z);
	}
	constexpr Vector3 operator-(const Vector3& Vec) const {
		return Vector3(X - Vec.X, Y - Vec.Y, Z - Vec.Z);
	}

	constexpr Vector3 operator*(RealType Scalar) const {
		return Vector3(X * Scalar, Y * Scalar, Z * Scalar);
	}
	template<typename RealType2>
	constexpr Vector3 operator*(RealType2 Scalar) const {
		return Vector3(X * (RealType)Scalar, Y * (RealType)Scalar, Z * (RealType)Scalar);
	}
	constexpr Vector3 operator/(RealType Scalar) const {
		return Vector3(X / Scalar, Y / Scalar, Z / Scalar);
	}
	template<typename RealType2>
	constexpr Vector3 operator/(RealType2 Scalar) const {
		return Vector3(X / (RealType)Scalar, Y / (RealType)Scalar, Z / (RealType)Scalar);
	}

	constexpr Vector3 operator*(const Vector3& Vec) const {
		return Vector3(X * Vec.X, Y * Vec.Y, Z * Vec.Z);
	}
	constexpr Vector3 operator/(const Vector3& Vec) const {
		return Vector3(X / Vec.X, Y / Vec.Y, Z / Vec.Z);
	}

	constexpr Vector3& operator+=(const Vector3& Vec) {
		X += Vec.X; Y += Vec.Y; Z += Vec.Z;
		return *this;
	}
	constexpr Vector3& operator-=(const Vector3& Vec) {
		X -= Vec.X; Y -= Vec.Y; Z -= Vec.Z; 
		return *this;
	}
	constexpr Vector3& operator*=(RealType Scalar) {
		X *= Scalar; Y *= Scalar; Z *= Scalar;
		return *this;
	}
	template<typename RealType2>
	constexpr Vector3& operator*=(RealType2 Scalar) {
		X *= (RealType)Scalar; Y *= (RealType)Scalar; Z *= (RealType)Scalar;
		return *this;
	}
	constexpr Vector3& operator/=(RealType Scalar) {
		X /= Scalar; Y /= Scalar; Z /= Scalar;
		return *this;
	}
	template<typename RealType2>
	constexpr Vector3& operator/=(RealType2 Scalar) {
		X /= (RealType)Scalar; Y /= (RealType)Scalar; Z /= (RealType)Scalar;
		return *this;
	}
	constexpr Vector3& operator*=(const Vector3& Vec) {
		X *= Vec.X; Y *= Vec.Y; Z *= Vec.Z;
		return *this;
	}

	constexpr bool operator==(const Vector3& Other) const {
		return X == Other.X && Y == Other.Y && Z == Other.Z;
	}
	constexpr bool operator!=(const Vector3& Other) const {
		return X != Other.X || Y != Other.Y || Z != Other.Z;
	}
	constexpr bool operator<(const Vector3& Other) const {
		if (X != Other.X)		return X < Other.X;
		else if (Y != Other.Y)	return Y < Other.Y;
		else if (Z != Other.Z)	return Z < Other.Z;
		else return false;
	}

	constexpr bool EpsilonEqual(const Vector3& Other, RealType Epsilon = RealConstants<RealType>::Epsilon()) const {
		return (GS::Abs(X-Other.X) < Epsilon) && (GS::Abs(Y-Other.Y) < Epsilon) && (GS::Abs(Z-Other.Z) < Epsilon);
	}

	template<typename RealType2>
	explicit operator Vector3<RealType2>() const {
		return Vector3<RealType2>((RealType2)X, (RealType2)Y, (RealType2)Z);
	}


	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	inline operator UE::Math::TVector<RealType>() const {
		return UE::Math::TVector<RealType>(X, Y, Z);
	}
	inline Vector3(const UE::Math::TVector<RealType>& Vec)
		: X(Vec.X), Y(Vec.Y), Z(Vec.Z) {}
	template<typename RealType2>
	explicit inline operator UE::Math::TVector<RealType2>() const {
		return UE::Math::TVector<RealType2>((RealType2)X, (RealType2)Y, (RealType2)Z);
	}
	template<typename RealType2>
	explicit inline Vector3(const UE::Math::TVector<RealType2>& Vec)
		: X((RealType)Vec.X), Y((RealType)Vec.Y), Z((RealType)Vec.Z) {}
	
	explicit Vector3(const UE::Geometry::FVector3i& Vec) 
		: Vector3<RealType>((RealType)Vec.X, (RealType)Vec.Y, (RealType)Vec.Z) { }
#endif

};
typedef Vector3<double> Vector3d;
typedef Vector3<float> Vector3f;



template<typename RealType>
Vector3<RealType>::Vector3()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<Vector3<RealType>>());
	static_assert(std::is_trivially_copyable<Vector3<RealType>>());
	static_assert(std::is_floating_point<RealType>());
}

template<typename RealType>
Vector3<RealType>::Vector3(RealType InitialValue)
{
	X = Y = Z = InitialValue;
}

template<typename RealType>
Vector3<RealType>::Vector3(RealType x, RealType y, RealType z)
{
	X = x; Y = y; Z = z;
}

template<typename RealType>
Vector3<RealType>::Vector3(const RealType* Values)
{
	X = Values[0]; Y = Values[1]; Z = Values[2];
}

template<typename RealType>
inline Vector3<RealType> operator*(RealType Scalar, const Vector3<RealType>& Vec)
{
	return Vector3<RealType>(Scalar * Vec.X, Scalar * Vec.Y, Scalar * Vec.Z);
}

template<typename RealType>
inline Vector3<RealType> Normalized(const Vector3<RealType>& Vec, RealType Epsilon = RealMath<RealType>::NormalizeTolerance() )
{
	RealType Length = Vec.Length();
	if (Length > Epsilon) {
		return ( ((RealType)1) / Length ) * Vec;
	}
	return Vector3<RealType>::Zero();
}

template<typename RealType>
inline RealType Distance(const Vector3<RealType>& A, const Vector3<RealType>& B)
{
	RealType dx = (A.X - B.X), dy = (A.Y - B.Y), dz = (A.Z - B.Z);
	return GS::Sqrt(dx * dx + dy * dy + dz * dz);
}

template<typename RealType>
inline RealType DistanceSquared(const Vector3<RealType>& A, const Vector3<RealType>& B)
{
	RealType dx = (A.X - B.X), dy = (A.Y - B.Y), dz = (A.Z - B.Z);
	return dx * dx + dy * dy + dz * dz;
}

template<typename RealType>
inline RealType VectorAngleRad(const Vector3<RealType>& A, const Vector3<RealType>& B)
{
	RealType ClampedDot = GS::Clamp( A.Dot(B), (RealType)-1, (RealType)1 );
	return GS::ACos(ClampedDot);
}

template<typename RealType>
inline RealType VectorAngleDeg(const Vector3<RealType>& A, const Vector3<RealType>& B)
{
	return GS::RadToDeg(VectorAngleRad(A, B));
}

template<typename RealType>
inline RealType Dot(const Vector3<RealType>& A, const Vector3<RealType>& B)
{
	return A.X*B.X + A.Y*B.Y + A.Z*B.Z;
}

template<typename RealType>
inline Vector3<RealType> Cross(const Vector3<RealType>& A, const Vector3<RealType>& B)
{
	return Vector3<RealType>(A.Y*B.Z - A.Z*B.Y,	A.Z*B.X - A.X*B.Z, A.X*B.Y - A.Y*B.X);
}

template<typename RealType>
inline Vector3<RealType> UnitCross(const Vector3<RealType>& A, const Vector3<RealType>& B, RealType Epsilon = RealMath<RealType>::NormalizeTolerance())
{
	return Normalized(Cross(A, B));
}

template<typename RealType>
inline Vector3<RealType> Normal(const Vector3<RealType>& A, const Vector3<RealType>& B, const Vector3<RealType>& C)
{
	// todo investigate if it is better to use C-B here... or maybe a 'robust' version that checks lengths of all 3?
	// we are computing the lengths anyway...
	Vector3<RealType> e0(B - A);
	Vector3<RealType> e1(C - A);
	e0.Normalize();		
	e1.Normalize();
	// do we really need to normalize again?
#ifdef GS_USE_LEFT_HANDED_COORDINATE_SYSTEM
	return Normalized(Cross(e1, e0));
#else
	return Normalized(Cross(e0, e1));
#endif
}


} // end namespace GS



// define std::hash function usable with unordered_map. maybe these should be moved to a separate file to avoid STL header dependency...
namespace std {
	template<typename RealType>
	struct hash<GS::Vector3<RealType>> {
		inline size_t operator()(const GS::Vector3<RealType>& Vector) const {
			return (size_t)GS::HashVector3(Vector.X, Vector.Y, Vector.Z);
		}
	};
}

