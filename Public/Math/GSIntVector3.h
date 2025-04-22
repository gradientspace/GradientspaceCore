// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"

#include "Math/GSHash.h"
#include "Math/GSVector3.h"
#include "Math/GSIndex3.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Math/Vector.h"
#include "Math/IntVector.h"
#include "IntVectorTypes.h"
#include "IntTypes.h"
#endif

#include <xhash>  // for std::hash defined at bottom

namespace GS
{

template<typename IntType>
struct IntVector3
{
	union {
		struct {
			IntType X;
			IntType Y;
			IntType Z;
		};
		IntType XYZ[3] = { {}, {}, {} };
	};

	constexpr IntVector3();
	constexpr explicit IntVector3(IntType InitialValue);
	constexpr explicit IntVector3(IntType x, IntType y, IntType z);
	constexpr explicit IntVector3(const IntType* Values);

	constexpr static IntVector3 One() { return IntVector3((IntType)1); }
	constexpr static IntVector3 Zero() { return IntVector3((IntType)0); }
	constexpr static IntVector3 UnitX() { return IntVector3((IntType)1, (IntType)0, (IntType)0); }
	constexpr static IntVector3 UnitY() { return IntVector3((IntType)0, (IntType)1, (IntType)0); }
	constexpr static IntVector3 UnitZ() { return IntVector3((IntType)0, (IntType)0, (IntType)1); }
	constexpr static IntVector3 MakeUnit(int Axis, IntType Sign = (IntType)1.0) { IntVector3 V = Zero(); V[Axis] = Sign*1; return V; }
	constexpr static IntVector3 MaxInt() { return IntVector3(IntConstants<IntType>::MaxValue()); }


	IntType& operator[](int index)	{
		return XYZ[index];
	}

	IntType operator[](int index) const {
		return XYZ[index];
	}

	IntType* AsPointer() {
		return &XYZ[0];
	}
	const IntType* AsPointer() const {
		return &XYZ[0];
	}

	// these use int64_t to avoid overflow on smaller int types
	constexpr int64_t SquaredLength() const {
		return (int64_t)X*(int64_t)X + (int64_t)Y*(int64_t)Y + (int64_t)Z*(int64_t)Z;
	}
	constexpr int64_t DistanceSquared(const IntVector3& Vec) const {
		int64_t dx = ((int64_t)Vec.X - (int64_t)X), dy = ((int64_t)Vec.Y - (int64_t)Y), dz = ((int64_t)Vec.Z - (int64_t)Z);
		return dx*dx + dy*dy + dz*dz;
	}
	constexpr int64_t CityBlockDistance(const IntVector3& Vec) const {
		int64_t dx = ((int64_t)Vec.X - (int64_t)X), dy = ((int64_t)Vec.Y - (int64_t)Y), dz = ((int64_t)Vec.Z - (int64_t)Z);
		return GS::Abs(dx) + GS::Abs(dy) + GS::Abs(dz);
	}
	constexpr int64_t Dot(const IntVector3& Vec) const {
		return (int64_t)X*(int64_t)Vec.X + (int64_t)Y*(int64_t)Vec.Y + (int64_t)Z*(int64_t)Vec.Z;
	}
	constexpr IntVector3 Abs() const {
		return IntVector3(GS::Abs(X), GS::Abs(Y), GS::Abs(Z));
	}

	constexpr IntVector3 operator-() const {
		return IntVector3(-X, -Y, -Z);
	}

	constexpr IntVector3 operator+(const IntVector3& Vec) const	{
		return IntVector3(X + Vec.X, Y + Vec.Y, Z + Vec.Z);
	}
	constexpr IntVector3 operator-(const IntVector3& Vec) const {
		return IntVector3(X - Vec.X, Y - Vec.Y, Z - Vec.Z);
	}

	constexpr IntVector3 operator*(int32_t Scalar) const {
		return IntVector3(X * Scalar, Y * Scalar, Z * Scalar);
	}
	constexpr IntVector3 operator/(int32_t Scalar) const {
		return IntVector3(X / Scalar, Y / Scalar, Z / Scalar);
	}

	constexpr IntVector3 operator*(const IntVector3& Vec) const {
		return IntVector3(X * Vec.X, Y * Vec.Y, Z * Vec.Z);
	}
	constexpr IntVector3 operator/(const IntVector3& Vec) const {
		return IntVector3(X / Vec.X, Y / Vec.Y, Z / Vec.Z);
	}

	constexpr IntVector3& operator+=(const IntVector3& Vec) {
		X += Vec.X; Y += Vec.Y; Z += Vec.Z;
		return *this;
	}
	constexpr IntVector3& operator-=(const IntVector3& Vec) {
		X -= Vec.X; Y -= Vec.Y; Z -= Vec.Z; 
		return *this;
	}
	constexpr IntVector3& operator*=(int32_t Scalar) {
		X *= Scalar; Y *= Scalar; Z *= Scalar;
		return *this;
	}
	constexpr IntVector3& operator/=(int32_t Scalar) {
		X /= Scalar; Y /= Scalar; Z /= Scalar;
		return *this;
	}


	constexpr bool operator==(const IntVector3& Other) const {
		return X == Other.X && Y == Other.Y && Z == Other.Z;
	}
	constexpr bool operator!=(const IntVector3& Other) const {
		return X != Other.X || Y != Other.Y || Z != Other.Z;
	}
	constexpr bool operator<(const IntVector3& Other) const {
		if (X != Other.X)		return X < Other.X;
		else if (Y != Other.Y)	return Y < Other.Y;
		else if (Z != Other.Z)	return Z < Other.Z;
		else return false;
	}

	// real/int conversion
	template<typename RealType>
	explicit operator Vector3<RealType>() const {
		return Vector3<RealType>((RealType)X, (RealType)Y, (RealType)Z);
	}
	template<typename RealType>
	explicit IntVector3(const Vector3<RealType>& Vec)
		: X((IntType)Vec.X), Y((IntType)Vec.Y), Z((IntType)Vec.Z) 
	{}

	template<typename RealType>
	static IntVector3 RoundToInt(const Vector3<RealType>& Vec) {
		return IntVector3<IntType>(
			(IntType)std::lround(Vec.X), (IntType)std::lround(Vec.Y), (IntType)std::lround(Vec.Z) );
	}

	//int/int type conversions
	template<typename IntType2>
	explicit operator IntVector3<IntType2>() const {
		return IntVector3<IntType2>((IntType2)X, (IntType2)Y, (IntType2)Z);
	}
	template<typename IntType2>
	explicit IntVector3(const IntVector3<IntType2>& Vec)
		: X((IntType)Vec.X), Y((IntType)Vec.Y), Z((IntType)Vec.Z)
	{}

	template<typename IndexType>
	explicit operator Index3<IndexType>() const {
		return Index3<IndexType>((IndexType)X, (IndexType)Y, (IndexType)Z);
	}
	template<typename IndexType>
	explicit IntVector3(const Index3<IndexType>& Idx)
		: X((IntType)Idx.A), Y((IntType)Idx.B), Z((IntType)Idx.C)
	{}

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	inline operator UE::Math::TIntVector3<IntType>() const {
		return UE::Math::TIntVector3<IntType>(X, Y, Z);
	}
	inline IntVector3(const UE::Math::TIntVector3<IntType>& Vec)
		: X(Vec.X), Y(Vec.Y), Z(Vec.Z) {}
	inline operator UE::Geometry::FVector3i() const {
		return UE::Geometry::FVector3i((int)X, (int)Y, (int)Z);
	}
	inline IntVector3(const UE::Geometry::FVector3i& Vec)
		: X((IntType)Vec.X), Y((IntType)Vec.Y), Z((IntType)Vec.Z) {}
	inline operator UE::Geometry::FIndex3i() const {
		return UE::Geometry::FIndex3i((int)X, (int)Y, (int)Z);
	}
	inline IntVector3(const UE::Geometry::FIndex3i& Vec)
		: X((IntType)Vec.A), Y((IntType)Vec.B), Z((IntType)Vec.C) {}
	template<typename RealType>
	explicit operator UE::Math::TVector<RealType>() const {
		return UE::Math::TVector<RealType>((RealType)X, (RealType)Y, (RealType)Z);
	}
	template<typename RealType>
	explicit IntVector3(const UE::Math::TVector<RealType>& Vec)
		: X((IntType)Vec.X), Y((IntType)Vec.Y), Z((IntType)Vec.Z)
	{}
#endif

};
typedef IntVector3<int> Vector3i;
typedef IntVector3<int64_t> Vector3i64;
typedef IntVector3<uint64_t> Vector3u64;


template<typename IntType>
constexpr IntVector3<IntType>::IntVector3()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<IntVector3<IntType>>());
	static_assert(std::is_trivially_copyable<IntVector3<IntType>>());
	static_assert(std::is_integral<IntType>());
}

template<typename IntType>
constexpr IntVector3<IntType>::IntVector3(IntType InitialValue)
{
	X = Y = Z = InitialValue;
}

template<typename IntType>
constexpr IntVector3<IntType>::IntVector3(IntType x, IntType y, IntType z)
{
	X = x; Y = y; Z = z;
}

template<typename IntType>
constexpr IntVector3<IntType>::IntVector3(const IntType* Values)
{
	X = Values[0]; Y = Values[1]; Z = Values[2];
}

template<typename IntType>
inline IntVector3<IntType> operator*(IntType Scalar, const IntVector3<IntType>& Vec)
{
	return IntVector3<IntType>(Scalar * Vec.X, Scalar * Vec.Y, Scalar * Vec.Z);
}

template<typename IntType>
inline IntVector3<IntType> Max(const IntVector3<IntType>& A, const IntVector3<IntType>& B)
{
	return IntVector3<IntType>(GS::Max(A.X, B.X), GS::Max(A.Y, B.Y), GS::Max(A.Z, B.Z));
}
template<typename IntType>
inline IntVector3<IntType> Min(const IntVector3<IntType>& A, const IntVector3<IntType>& B)
{
	return IntVector3<IntType>(GS::Min(A.X, B.X), GS::Min(A.Y, B.Y), GS::Min(A.Z, B.Z));
}



template<typename IntType>
uint32_t GetTypeHash(const IntVector3<IntType>& Vector)
{
	return GS::HashVector3(Vector.X, Vector.Y, Vector.Z);
}


} // end namespace GS


// define std::hash function usable with unordered_map. maybe these should be moved to a separate file to avoid STL header dependency...
namespace std {
	template<typename IntType>
	struct hash<GS::IntVector3<IntType>> {
		inline size_t operator()(const GS::IntVector3<IntType>& Vector) const {
			// size_t value = your hash computations over x
			return (size_t)GS::HashVector3(Vector.X, Vector.Y, Vector.Z);
		}
	};
}



