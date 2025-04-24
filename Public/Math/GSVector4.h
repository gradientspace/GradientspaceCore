// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSHash.h"
#include "Math/GSVector3.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Math/Vector4.h"
#include "Math/Color.h"
#endif


namespace GS
{

template<typename RealType>
struct Vector4
{
	union {
		struct {
			RealType X;
			RealType Y;
			RealType Z;
			RealType W;
		};
		RealType XYZW[4] = { {}, {}, {}, {} };
	};

	Vector4();
	explicit Vector4(RealType InitialValue);
	explicit Vector4(RealType x, RealType y, RealType z, RealType w);
	explicit Vector4(const RealType* Values);
	explicit Vector4(const Vector3<RealType>& Vec3, RealType w = (RealType)1);

	static Vector4 One() { return Vector4((RealType)1); }
	static Vector4 Zero() { return Vector4((RealType)0); }
	static Vector4 MakeUnit(int Axis, RealType Sign = (RealType)1.0) { Vector4 V = Zero(); V[Axis] = Sign*1; return V; }


	RealType& operator[](int index)	{
		return XYZW[index];
	}

	RealType operator[](int index) const {
		return XYZW[index];
	}

	RealType* AsPointer() {
		return &XYZW[0];
	}
	const RealType* AsPointer() const { 
		return &XYZW[0]; 
	}

	constexpr RealType SquaredLength() const {
		return X*X + Y*Y + Z*Z + W*W;
	}
	constexpr RealType Length() const {
		return GS::Sqrt(X*X + Y*Y + Z*Z + W*W);
	}
	constexpr RealType DistanceSquared(const Vector4& Vec) const {
		RealType dx = (Vec.X - X), dy = (Vec.Y - Y), dz = (Vec.Z - Z), dw = (Vec.W - W);
		return dx*dx + dy*dy + dz*dz + dw*dw;
	}
	constexpr RealType Distance(const Vector4& Vec) const {
		RealType dx = (Vec.X - X), dy = (Vec.Y - Y), dz = (Vec.Z - Z), dw = (Vec.W - W);
		return GS::Sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
	}
	constexpr RealType Dot(const Vector4& Vec) const {
		return X*Vec.X + Y*Vec.Y + Z*Vec.Z + W*Vec.W;
	}
	constexpr Vector4 Abs() const {
		return Vector4(GS::Abs(X), GS::Abs(Y), GS::Abs(Z), GS::Abs(W));
	}

	RealType Normalize(RealType Epsilon = RealMath<RealType>::NormalizeTolerance())
	{
		// todo want to set epsilon to avoid creating denorms??
		RealType len = GS::Sqrt(X*X + Y*Y + Z*Z + W*W);
		RealType scale = (len > Epsilon) ? ((RealType)1 / len) : 0;
		X *= scale; Y *= scale; Z *= scale; W *= scale;
		return len;
	}

	constexpr bool IsNormalized(const RealType Tolerance = RealMath<RealType>::ZeroTolerance() ) const {
		return GS::Abs( (X*X + Y*Y + Z*Z + W*W) - (RealType)1 ) < Tolerance;
	}

	constexpr Vector4 operator-() const {
		return Vector4(-X, -Y, -Z, -W);
	}

	constexpr Vector4 operator+(const Vector4& Vec) const	{
		return Vector4(X + Vec.X, Y + Vec.Y, Z + Vec.Z, W + Vec.W);
	}
	constexpr Vector4 operator-(const Vector4& Vec) const {
		return Vector4(X - Vec.X, Y - Vec.Y, Z - Vec.Z, W - Vec.W);
	}

	constexpr Vector4 operator*(RealType Scalar) const {
		return Vector4(X * Scalar, Y * Scalar, Z * Scalar, W * Scalar);
	}
	constexpr Vector4 operator/(RealType Scalar) const {
		return Vector4(X / Scalar, Y / Scalar, Z / Scalar, W / Scalar);
	}

	constexpr Vector4 operator*(const Vector4& Vec) const {
		return Vector4(X * Vec.X, Y * Vec.Y, Z * Vec.Z, W * Vec.W);
	}
	constexpr Vector4 operator/(const Vector4& Vec) const {
		return Vector4(X / Vec.X, Y / Vec.Y, Z / Vec.Z, W / Vec.W);
	}

	constexpr Vector4& operator+=(const Vector4& Vec) {
		X += Vec.X; Y += Vec.Y; Z += Vec.Z; W += Vec.W;
		return *this;
	}
	constexpr Vector4& operator-=(const Vector4& Vec) {
		X -= Vec.X; Y -= Vec.Y; Z -= Vec.Z; W -= Vec.W;
		return *this;
	}
	constexpr Vector4& operator*=(RealType Scalar) {
		X *= Scalar; Y *= Scalar; Z *= Scalar; W *= Scalar;
		return *this;
	}
	constexpr Vector4& operator/=(RealType Scalar) {
		X /= Scalar; Y /= Scalar; Z /= Scalar; W /= Scalar;
		return *this;
	}
	constexpr Vector4& operator*=(const Vector4& Vec) {
		X *= Vec.X; Y *= Vec.Y; Z *= Vec.Z; W *= Vec.W;
		return *this;
	}

	constexpr bool operator==(const Vector4& Other) const {
		return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W;
	}
	constexpr bool operator!=(const Vector4& Other) const {
		return X != Other.X || Y != Other.Y || Z != Other.Z || W != Other.W;
	}
	constexpr bool operator<(const Vector4& Other) const {
		if (X != Other.X)		return X < Other.X;
		else if (Y != Other.Y)	return Y < Other.Y;
		else if (Z != Other.Z)	return Z < Other.Z;
		else if (W != Other.W)	return W < Other.W;
		else return false;
	}

	constexpr bool EpsilonEqual(const Vector4& Other, RealType Epsilon = RealConstants<RealType>::Epsilon()) const {
		return (GS::Abs(X-Other.X) < Epsilon) && (GS::Abs(Y-Other.Y) < Epsilon) && (GS::Abs(Z-Other.Z) < Epsilon) && (GS::Abs(W-Other.W) < Epsilon);
	}

	template<typename RealType2>
	explicit operator Vector4<RealType2>() const {
		return Vector4<RealType2>((RealType2)X, (RealType2)Y, (RealType2)Z, (RealType2)W);
	}

	template<typename RealType2>
	explicit operator Vector3<RealType2>() const {
		return Vector3<RealType2>((RealType2)X, (RealType2)Y, (RealType2)Z);
	}

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	inline operator UE::Math::TVector4<RealType>() const {
		return UE::Math::TVector4<RealType>(X, Y, Z, W);
	}
	inline Vector4(const UE::Math::TVector4<RealType>& Vec)
		: X(Vec.X), Y(Vec.Y), Z(Vec.Z), W(Vec.W) {}
	template<typename RealType2>
	explicit inline operator UE::Math::TVector4<RealType2>() const {
		return UE::Math::TVector4<RealType2>((RealType2)X, (RealType2)Y, (RealType2)Z, (RealType2)W);
	}
	template<typename RealType2>
	explicit inline Vector4(const UE::Math::TVector4<RealType2>& Vec)
		: X((RealType)Vec.X), Y((RealType)Vec.Y), Z((RealType)Vec.Z), W((RealType)Vec.W) {}
	explicit inline operator FLinearColor() const {
		return FLinearColor((float)X, (float)Y, (float)Z, (float)W);
	}
	explicit inline Vector4(const FLinearColor& LinearColor)
		: X((RealType)LinearColor.R), Y((RealType)LinearColor.G), Z((RealType)LinearColor.B), W((RealType)LinearColor.A) {}
#endif

};
typedef Vector4<double> Vector4d;
typedef Vector4<float> Vector4f;


template<typename RealType>
Vector4<RealType>::Vector4()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<Vector4<RealType>>());
	static_assert(std::is_trivially_copyable<Vector4<RealType>>());
	static_assert(std::is_floating_point<RealType>());
}

template<typename RealType>
Vector4<RealType>::Vector4(RealType InitialValue)
{
	X = Y = Z = W = InitialValue;
}

template<typename RealType>
Vector4<RealType>::Vector4(RealType x, RealType y, RealType z, RealType w)
{
	X = x; Y = y; Z = z; W = w;
}

template<typename RealType>
Vector4<RealType>::Vector4(const RealType* Values)
{
	X = Values[0]; Y = Values[1]; Z = Values[2]; W = Values[3];
}

template<typename RealType>
Vector4<RealType>::Vector4(const Vector3<RealType>& Vec3, RealType w)
{
	X = Vec3.X; Y = Vec3.Y; Z = Vec3.Z; W = w;
}

template<typename RealType>
inline Vector4<RealType> operator*(RealType Scalar, const Vector4<RealType>& Vec)
{
	return Vector4<RealType>(Scalar * Vec.X, Scalar * Vec.Y, Scalar * Vec.Z, Scalar * Vec.W);
}

template<typename RealType>
inline Vector4<RealType> Normalized(const Vector4<RealType>& Vec, RealType Epsilon = RealMath<RealType>::NormalizeTolerance() )
{
	RealType Length = Vec.Length();
	if (Length > Epsilon) {
		return ( ((RealType)1) / Length ) * Vec;
	}
	return Vector4<RealType>::Zero();
}

template<typename RealType>
inline RealType Distance(const Vector4<RealType>& A, const Vector4<RealType>& B)
{
	RealType dx = (A.X - B.X), dy = (A.Y - B.Y), dz = (A.Z - B.Z), dw = (A.W - B.W);
	return GS::Sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
}

template<typename RealType>
inline RealType DistanceSquared(const Vector4<RealType>& A, const Vector4<RealType>& B)
{
	RealType dx = (A.X - B.X), dy = (A.Y - B.Y), dz = (A.Z - B.Z), dw = (A.W - B.W);
	return dx*dx + dy*dy + dz*dz + dw*dw;
}

template<typename RealType>
inline RealType Dot(const Vector4<RealType>& A, const Vector4<RealType>& B)
{
	return A.X*B.X + A.Y*B.Y + A.Z*B.Z + A.W*B.W;
}

} // end namespace GS



// define std::hash function usable with unordered_map. maybe these should be moved to a separate file to avoid STL header dependency...
namespace std {
	template<typename RealType>
	struct hash<GS::Vector4<RealType>> {
		inline size_t operator()(const GS::Vector4<RealType>& Vector) const {
			return (size_t)GS::HashVector4(Vector.X, Vector.Y, Vector.Z, Vector.W);
		}
	};
}

