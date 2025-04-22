// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSHash.h"
#include "Math/GSMath.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Math/Vector2D.h"
#endif

#include <xhash>  // works for std::hash defined at bottom? or is <unordered_set> required?

namespace GS
{

template<typename RealType>
struct Vector2
{
	union {
		struct {
			RealType X;
			RealType Y;
		};
		RealType XY[2] = { {}, {} };
	};

	Vector2();
	explicit Vector2(RealType InitialValue);
	explicit Vector2(RealType x, RealType y);
	explicit Vector2(const RealType* Values);

	static Vector2 One() { return Vector2((RealType)1); }
	static Vector2 Zero() { return Vector2((RealType)0); }
	static Vector2 UnitX() { return Vector2((RealType)1, (RealType)0); }
	static Vector2 UnitY() { return Vector2((RealType)0, (RealType)1); }
	static Vector2 MakeUnit(int Axis, RealType Sign = (RealType)1.0) { Vector2 V = Zero(); V[Axis] = Sign * 1; return V; }

	RealType& operator[](int index)	{
		return XY[index];
	}

	RealType operator[](int index) const {
		return XY[index];
	}

	RealType* AsPointer() {
		return &XY[0];
	}
	const RealType* AsPointer() const {
		return &XY[0];
	}


	constexpr RealType SquaredLength() const {
		return X * X + Y * Y;
	}
	constexpr RealType Length() const {
		return GS::Sqrt(X * X + Y * Y);
	}
	constexpr RealType DistanceSquared(const Vector2& Vec) const {
		RealType dx = (Vec.X - X), dy = (Vec.Y - Y);
		return dx * dx + dy * dy;
	}
	constexpr RealType Distance(const Vector2& Vec) const {
		RealType dx = (Vec.X - X), dy = (Vec.Y - Y);
		return GS::Sqrt(dx * dx + dy * dy);
	}
	constexpr RealType Dot(const Vector2& Vec) const {
		return X * Vec.X + Y * Vec.Y;
	}
	//! dot product of this vector with PerpCW(Vec) (ie Vec rotated 90 degrees clockwise)
	constexpr RealType DotPerp(const Vector2& Vec) const {
		return X * Vec.Y - Y * Vec.X;
	}
	//! rotate this vector 90-degrees clockwise
	constexpr Vector2 PerpCW() const {
		return Vector2(Y, -X);
	}
	constexpr Vector2 Abs() const {
		return Vector2(GS::Abs(X), GS::Abs(Y));
	}
	constexpr RealType AbsMax() const {
		return GS::Max(GS::Abs(X), GS::Abs(Y));
	}


	RealType Normalize(RealType Epsilon = RealMath<RealType>::NormalizeTolerance())
	{
		// todo want to set epsilon to avoid creating denorms??
		RealType len = GS::Sqrt(X*X + Y*Y);
		RealType scale = (len > Epsilon) ? ((RealType)1 / len) : 0;
		X *= scale; Y *= scale;
		return len;
	}
	Vector2 Normalized(RealType Epsilon = RealMath<RealType>::NormalizeTolerance()) const
	{
		RealType len = GS::Sqrt(X*X + Y*Y);
		RealType scale = (len > Epsilon) ? ((RealType)1 / len) : 0;
		return Vector2(X * scale, Y * scale);
	}

	constexpr bool IsNormalized(const RealType Tolerance = RealMath<RealType>::ZeroTolerance() ) const {
		return GS::Abs( (X*X + Y*Y) - (RealType)1 ) < Tolerance;
	}


	constexpr Vector2 operator-() const {
		return Vector2(-X, -Y);
	}

	constexpr Vector2 operator+(const Vector2& Vec) const {
		return Vector2(X + Vec.X, Y + Vec.Y);
	}
	constexpr Vector2 operator-(const Vector2& Vec) const {
		return Vector2(X - Vec.X, Y - Vec.Y);
	}

	constexpr Vector2 operator*(RealType Scalar) const {
		return Vector2(X * Scalar, Y * Scalar);
	}
	template<typename RealType2>
	constexpr Vector2 operator*(RealType2 Scalar) const {
		return Vector2(X * (RealType)Scalar, Y * (RealType)Scalar);
	}
	constexpr Vector2 operator/(RealType Scalar) const {
		return Vector2(X / Scalar, Y / Scalar);
	}
	template<typename RealType2>
	constexpr Vector2 operator/(RealType2 Scalar) const {
		return Vector2(X / (RealType)Scalar, Y / (RealType)Scalar);
	}

	constexpr Vector2 operator*(const Vector2& Vec) const {
		return Vector2(X * Vec.X, Y * Vec.Y);
	}
	constexpr Vector2 operator/(const Vector2& Vec) const {
		return Vector2(X / Vec.X, Y / Vec.Y);
	}

	constexpr Vector2& operator+=(const Vector2& Vec) {
		X += Vec.X; Y += Vec.Y;
		return *this;
	}
	constexpr Vector2& operator-=(const Vector2& Vec) {
		X -= Vec.X; Y -= Vec.Y;
		return *this;
	}
	constexpr Vector2& operator*=(RealType Scalar) {
		X *= Scalar; Y *= Scalar;
		return *this;
	}
	template<typename RealType2>
	constexpr Vector2& operator*=(RealType2 Scalar) {
		X *= (RealType)Scalar; Y *= (RealType)Scalar;
		return *this;
	}
	constexpr Vector2& operator/=(RealType Scalar) {
		X /= Scalar; Y /= Scalar;
		return *this;
	}
	template<typename RealType2>
	constexpr Vector2& operator/=(RealType2 Scalar) {
		X /= (RealType)Scalar; Y /= (RealType)Scalar;
		return *this;
	}
	constexpr Vector2& operator*=(const Vector2& Vec) {
		X *= Vec.X; Y *= Vec.Y;
		return *this;
	}

	constexpr bool operator==(const Vector2& Other) const {
		return X == Other.X && Y == Other.Y;
	}
	constexpr bool operator!=(const Vector2& Other) const {
		return X != Other.X || Y != Other.Y;
	}
	constexpr bool operator<(const Vector2& Other) const {
		if (X != Other.X)		return X < Other.X;
		else if (Y != Other.Y)	return Y < Other.Y;
		else return false;
	}

	// float/double conversion cast
	template<typename RealType2>
	explicit operator Vector2<RealType2>() const {
		return Vector2<RealType2>((RealType2)X, (RealType2)Y);
	}

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	inline operator UE::Math::TVector2<RealType>() const {
		return UE::Math::TVector2<RealType>(X, Y);
	}
	inline Vector2(const UE::Math::TVector2<RealType>& Vec)
		: X(Vec.X), Y(Vec.Y) {}
	template<typename RealType2>
	explicit inline operator UE::Math::TVector2<RealType2>() const {
		return UE::Math::TVector2<RealType2>((RealType2)X, (RealType2)Y);
	}
	template<typename RealType2>
	explicit inline Vector2(const UE::Math::TVector2<RealType2>& Vec)
		: X((RealType)Vec.X), Y((RealType)Vec.Y) {}
#endif

};
typedef Vector2<double> Vector2d;
typedef Vector2<float> Vector2f;


template<typename RealType>
Vector2<RealType>::Vector2()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<Vector2<RealType>>());
	static_assert(std::is_trivially_copyable<Vector2<RealType>>());
	static_assert(std::is_floating_point<RealType>());
}

template<typename RealType>
Vector2<RealType>::Vector2(RealType InitialValue)
{
	X = Y = InitialValue;
}

template<typename RealType>
Vector2<RealType>::Vector2(RealType x, RealType y)
{
	X = x; Y = y;
}

template<typename RealType>
Vector2<RealType>::Vector2(const RealType* Values)
{
	X = Values[0]; Y = Values[1];
}

template<typename RealType>
inline Vector2<RealType> operator*(RealType Scalar, const Vector2<RealType>& Vec)
{
	return Vector2<RealType>(Scalar * Vec.X, Scalar * Vec.Y);
}


template<typename RealType>
inline Vector2<RealType> Normalized(const Vector2<RealType>& Vec, RealType Epsilon = RealMath<RealType>::NormalizeTolerance() )
{
	RealType Length = Vec.Length();
	if (Length > Epsilon) {
		return ( ((RealType)1) / Length ) * Vec;
	}
	return Vector2<RealType>::Zero();
}

template<typename RealType>
inline RealType Distance(const Vector2<RealType>& A, const Vector2<RealType>& B)
{
	RealType dx = (A.X - B.X), dy = (A.Y - B.Y);
	return GS::Sqrt(dx*dx + dy*dy);
}

template<typename RealType>
inline RealType DistanceSquared(const Vector2<RealType>& A, const Vector2<RealType>& B)
{
	RealType dx = (A.X - B.X), dy = (A.Y - B.Y);
	return dx*dx + dy*dy;
}

template<typename RealType>
inline RealType VectorAngleRad(const Vector2<RealType>& A, const Vector2<RealType>& B)
{
	RealType ClampedDot = GS::Clamp(A.Dot(B), (RealType)-1, (RealType)1);
	return GS::ACos(ClampedDot);
}

template<typename RealType>
inline RealType VectorAngleDeg(const Vector2<RealType>& A, const Vector2<RealType>& B)
{
	return GS::RadToDeg(VectorAngleRad(A, B));
}

template<typename RealType>
inline RealType Dot(const Vector2<RealType>& A, const Vector2<RealType>& B)
{
	return A.X*B.X + A.Y*B.Y;
}

//! dot product of this vector with PerpCW(Vec) (ie Vec rotated 90 degrees clockwise)
template<typename RealType>
inline RealType DotPerp(const Vector2<RealType>& A, const Vector2<RealType>& B)
{
	return A.X * B.Y - A.Y * B.X;
}

//! returns >0 if C is to the "left" of LineAB (ie CCW rotation), <0 if to the "right" (CW rotation), and 0 if on the line
//! (not numerically robust! Also produces nonsensical result if LineA == LineB)
inline double Fast2DPointSideTest(const Vector2d& LineA, const Vector2d& LineB, const Vector2d& TestPoint)
{
	return DotPerp((LineB - LineA), (TestPoint - LineA));
}

//! Apply Wrap-type clamping to 2D Vector
template<typename RealType>
inline Vector2<RealType> WrapToUnitRect(const Vector2<RealType>& Vec, RealType UnitWidthX = (RealType)1, RealType UnitHeightY = (RealType)1)
{
	RealType X = Vec.X, Y = Vec.Y;
	if (X < (RealType)0 || X > UnitWidthX) {
		X = GS::FMod(X, UnitWidthX);
		if (X < (RealType)0)
			X += UnitWidthX;
	}
	if (Y < (RealType)0 || Y > UnitHeightY) {
		Y = GS::FMod(Y, UnitHeightY);
		if (Y < (RealType)0)
			Y += UnitHeightY;
	}
	return Vector2<RealType>(X, Y);
}



} // end namespace GS



// define std::hash function usable with unordered_map. maybe these should be moved to a separate file to avoid STL header dependency...
namespace std {
	template<typename RealType>
	struct hash<GS::Vector2<RealType>> {
		inline size_t operator()(const GS::Vector2<RealType>& Vector) const {
			return (size_t)GS::HashVector2(Vector.X, Vector.Y);
		}
	};
}

