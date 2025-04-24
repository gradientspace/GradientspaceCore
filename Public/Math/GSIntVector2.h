// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"

#include "Math/GSMath.h"
#include "Math/GSHash.h"
#include "Math/GSVector2.h"
#include "Math/GSIndex2.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Math/IntVector.h"
#include "IntVectorTypes.h"
#include "IntTypes.h"
#endif


namespace GS
{

template<typename IntType>
struct IntVector2
{
	union {
		struct {
			IntType X;
			IntType Y;
		};
		IntType XY[2] = { {}, {} };
	};

	IntVector2();
	explicit IntVector2(IntType InitialValue);
	explicit IntVector2(IntType x, IntType y);
	explicit IntVector2(const IntType* Values);

	static IntVector2 One() { return IntVector2((IntType)1); }
	static IntVector2 Zero() { return IntVector2((IntType)0); }
	static IntVector2 UnitX() { return IntVector2((IntType)1, (IntType)0); }
	static IntVector2 UnitY() { return IntVector2((IntType)0, (IntType)1); }
		static IntVector2 MakeUnit(int Axis, IntType Sign = (IntType)1.0) { IntVector2 V = Zero(); V[Axis] = Sign*1; return V; }


	IntType& operator[](int index)	{
		return XY[index];
	}

	IntType operator[](int index) const {
		return XY[index];
	}

	IntType* AsPointer() {
		return &XY[0];
	}
	const IntType* AsPointer() const {
		return &XY[0];
	}

	constexpr int64_t SquaredLength() const {
		return (int64_t)X*(int64_t)X + (int64_t)Y*(int64_t)Y;
	}
	constexpr int64_t DistanceSquared(const IntVector2& Vec) const {
		int64_t dx = ((int64_t)Vec.X - (int64_t)X), dy = ((int64_t)Vec.Y - (int64_t)Y);
		return dx*dx + dy*dy;
	}
	constexpr int64_t Dot(const IntVector2& Vec) const {
		return (int64_t)X*(int64_t)Vec.X + (int64_t)Y*(int64_t)Vec.Y;
	}
	constexpr IntVector2 Abs() const {
		return IntVector2(GS::Abs(X), GS::Abs(Y));
	}

	constexpr IntVector2 operator-() const {
		return IntVector2(-X, -Y);
	}

	constexpr IntVector2 operator+(const IntVector2& Vec) const	{
		return IntVector2(X + Vec.X, Y + Vec.Y);
	}
	constexpr IntVector2 operator-(const IntVector2& Vec) const {
		return IntVector2(X - Vec.X, Y - Vec.Y);
	}

	constexpr IntVector2 operator*(int32_t Scalar) const {
		return IntVector2(X * Scalar, Y * Scalar);
	}
	constexpr IntVector2 operator/(int32_t Scalar) const {
		return IntVector2(X / Scalar, Y / Scalar);
	}

	constexpr IntVector2 operator*(const IntVector2& Vec) const {
		return IntVector2(X * Vec.X, Y * Vec.Y);
	}
	constexpr IntVector2 operator/(const IntVector2& Vec) const {
		return IntVector2(X / Vec.X, Y / Vec.Y);
	}

	constexpr IntVector2& operator+=(const IntVector2& Vec) {
		X += Vec.X; Y += Vec.Y;
		return *this;
	}
	constexpr IntVector2& operator-=(const IntVector2& Vec) {
		X -= Vec.X; Y -= Vec.Y;
		return *this;
	}
	constexpr IntVector2& operator*=(int32_t Scalar) {
		X *= Scalar; Y *= Scalar;
		return *this;
	}
	constexpr IntVector2& operator/=(int32_t Scalar) {
		X /= Scalar; Y /= Scalar;
		return *this;
	}


	constexpr bool operator==(const IntVector2& Other) const {
		return X == Other.X && Y == Other.Y;
	}
	constexpr bool operator!=(const IntVector2& Other) const {
		return X != Other.X || Y != Other.Y;
	}
	constexpr bool operator<(const IntVector2& Other) const {
		if (X != Other.X)		return X < Other.X;
		else if (Y != Other.Y)	return Y < Other.Y;
		else return false;
	}


	// real/int conversion
	template<typename RealType>
	explicit operator Vector2<RealType>() const {
		return Vector2<RealType>((RealType)X, (RealType)Y);
	}
	template<typename RealType>
	explicit IntVector2(const Vector2<RealType>& Vec)
		: X((IntType)Vec.X), Y((IntType)Vec.Y) 
	{}

	template<typename IndexType>
	explicit operator Index2<IndexType>() const {
		return Index2<IndexType>((IndexType)X, (IndexType)Y);
	}
	template<typename IndexType>
	explicit IntVector2(const Index2<IndexType>& Idx)
		: X((IntType)Idx.A), Y((IntType)Idx.B)
	{}

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	inline operator UE::Math::TIntVector2<IntType>() const {
		return UE::Math::TIntVector2<IntType>(X, Y);
	}
	inline IntVector2(const UE::Math::TIntVector2<IntType>& Vec)
		: X(Vec.X), Y(Vec.Y) {}
	inline operator UE::Geometry::FVector2i() const {
		return UE::Geometry::FVector2i((int)X, (int)Y);
	}
	inline IntVector2(const UE::Geometry::FVector2i& Vec)
		: X((IntType)Vec.X), Y((IntType)Vec.Y) {}
	inline operator UE::Geometry::FIndex2i() const {
		return UE::Geometry::FIndex2i((int)X, (int)Y);
	}
	inline IntVector2(const UE::Geometry::FIndex2i& Vec)
		: X((IntType)Vec.A), Y((IntType)Vec.B) {}
#endif

};
typedef IntVector2<int> Vector2i;


template<typename IntType>
IntVector2<IntType>::IntVector2()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<IntVector2<IntType>>());
	static_assert(std::is_trivially_copyable<IntVector2<IntType>>());
	static_assert(std::is_integral<IntType>());
}

template<typename IntType>
IntVector2<IntType>::IntVector2(IntType InitialValue)
{
	X = Y = InitialValue;
}

template<typename IntType>
IntVector2<IntType>::IntVector2(IntType x, IntType y)
{
	X = x; Y = y;
}

template<typename IntType>
IntVector2<IntType>::IntVector2(const IntType* Values)
{
	X = Values[0]; Y = Values[1];
}

template<typename IntType>
inline IntVector2<IntType> operator*(IntType Scalar, const IntVector2<IntType>& Vec)
{
	return IntVector2<IntType>(Scalar * Vec.X, Scalar * Vec.Y);
}


template<typename IntType>
uint32_t GetTypeHash(const IntVector2<IntType>& Vector)
{
	return GS::HashVector2(Vector.X, Vector.Y);
}

} // end namespace GS


// define std::hash function usable with unordered_map. maybe these should be moved to a separate file to avoid STL header dependency...
namespace std {
	template<typename IntType>
	struct hash<GS::IntVector2<IntType>> {
		inline size_t operator()(const GS::IntVector2<IntType>& Vector) const {
			// size_t value = your hash computations over x
			return (size_t)GS::HashVector2(Vector.X, Vector.Y);
		}
	};
}




