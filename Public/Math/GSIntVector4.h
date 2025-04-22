// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"

#include "Math/GSHash.h"
//#include "Math/GSVector3.h"
#include "Math/GSIndex3.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Math/Vector.h"
#include "Math/IntVector.h"
#include "IntVectorTypes.h"
#include "IntTypes.h"
#endif

#include <xhash>  // works for std::hash defined at bottom? or is <unordered_set> required?

namespace GS
{

template<typename IntType>
struct IntVector4
{
	union {
		struct {
			IntType X;
			IntType Y;
			IntType Z;
			IntType W;
		};
		IntType XYZW[4] = { {}, {}, {}, {} };
	};

	constexpr IntVector4();
	constexpr explicit IntVector4(IntType InitialValue);
	constexpr explicit IntVector4(IntType x, IntType y, IntType z, IntType w);
	constexpr explicit IntVector4(const IntType* Values);

	constexpr static IntVector4 One() { return IntVector4((IntType)1); }
	constexpr static IntVector4 Zero() { return IntVector4((IntType)0); }
	constexpr static IntVector4 UnitX() { return IntVector4((IntType)1, (IntType)0, (IntType)0, (IntType)0); }
	constexpr static IntVector4 UnitY() { return IntVector4((IntType)0, (IntType)1, (IntType)0, (IntType)0); }
	constexpr static IntVector4 UnitZ() { return IntVector4((IntType)0, (IntType)0, (IntType)1, (IntType)0); }
	constexpr static IntVector4 UnitW() { return IntVector4((IntType)0, (IntType)0, (IntType)0, (IntType)1); }
	constexpr static IntVector4 MakeUnit(int Axis, IntType Sign = (IntType)1.0) { IntVector4 V = Zero(); V[Axis] = Sign*1; return V; }


	IntType& operator[](int index)	{
		return XYZW[index];
	}

	IntType operator[](int index) const {
		return XYZW[index];
	}

	IntType* AsPointer() {
		return &XYZW[0];
	}
	const IntType* AsPointer() const {
		return &XYZW[0];
	}

	// these use int64_t to avoid overflow on smaller int types
	constexpr int64_t SquaredLength() const {
		return (int64_t)X*(int64_t)X + (int64_t)Y*(int64_t)Y + (int64_t)Z*(int64_t)Z + (int64_t)W*(int64_t)W;
	}
	constexpr int64_t DistanceSquared(const IntVector4& Vec) const {
		int64_t dx = ((int64_t)Vec.X - (int64_t)X), dy = ((int64_t)Vec.Y - (int64_t)Y), dz = ((int64_t)Vec.Z - (int64_t)Z), dw = ((int64_t)Vec.W - (int64_t)W);
		return dx*dx + dy*dy + dz*dz + dw*dw;
	}
	constexpr int64_t CityBlockDistance(const IntVector4& Vec) const {
		int64_t dx = ((int64_t)Vec.X - (int64_t)X), dy = ((int64_t)Vec.Y - (int64_t)Y), dz = ((int64_t)Vec.Z - (int64_t)Z), dw = ((int64_t)Vec.W - (int64_t)W);
		return GS::Abs(dx) + GS::Abs(dy) + GS::Abs(dz) + GS::Abs(dw);
	}
	constexpr int64_t Dot(const IntVector4& Vec) const {
		return (int64_t)X*(int64_t)Vec.X + (int64_t)Y*(int64_t)Vec.Y + (int64_t)Z*(int64_t)Vec.Z + (int64_t)W*(int64_t)Vec.W;
	}
	constexpr IntVector4 Abs() const {
		return IntVector4(GS::Abs(X), GS::Abs(Y), GS::Abs(Z), GS::Abs(W));
	}

	constexpr IntVector4 operator-() const {
		return IntVector4(-X, -Y, -Z, -W);
	}

	constexpr IntVector4 operator+(const IntVector4& Vec) const	{
		return IntVector4(X + Vec.X, Y + Vec.Y, Z + Vec.Z, W + Vec.W);
	}
	constexpr IntVector4 operator-(const IntVector4& Vec) const {
		return IntVector4(X - Vec.X, Y - Vec.Y, Z - Vec.Z, W - Vec.W);
	}

	constexpr IntVector4 operator*(int32 Scalar) const {
		return IntVector4(X * Scalar, Y * Scalar, Z * Scalar);
	}
	constexpr IntVector4 operator/(int32 Scalar) const {
		return IntVector4(X / Scalar, Y / Scalar, Z / Scalar, W / Scalar);
	}

	constexpr IntVector4 operator*(const IntVector4& Vec) const {
		return IntVector4(X * Vec.X, Y * Vec.Y, Z * Vec.Z, W * Vec.W);
	}
	constexpr IntVector4 operator/(const IntVector4& Vec) const {
		return IntVector4(X / Vec.X, Y / Vec.Y, Z / Vec.Z, W / Vec.W);
	}

	constexpr IntVector4& operator+=(const IntVector4& Vec) {
		X += Vec.X; Y += Vec.Y; Z += Vec.Z; W += Vec.W;
		return *this;
	}
	constexpr IntVector4& operator-=(const IntVector4& Vec) {
		X -= Vec.X; Y -= Vec.Y; Z -= Vec.Z; W -= Vec.W;
		return *this;
	}
	constexpr IntVector4& operator*=(int32 Scalar) {
		X *= Scalar; Y *= Scalar; Z *= Scalar; W *= Scalar;
		return *this;
	}
	constexpr IntVector4& operator/=(int32 Scalar) {
		X /= Scalar; Y /= Scalar; Z /= Scalar; W /= Scalar;
		return *this;
	}


	constexpr bool operator==(const IntVector4& Other) const {
		return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W;
	}
	constexpr bool operator!=(const IntVector4& Other) const {
		return X != Other.X || Y != Other.Y || Z != Other.Z || W != Other.W;
	}
	constexpr bool operator<(const IntVector4& Other) const {
		if (X != Other.X)		return X < Other.X;
		else if (Y != Other.Y)	return Y < Other.Y;
		else if (Z != Other.Z)	return Z < Other.Z;
		else if (W != Other.W)	return W < Other.W;
		else return false;
	}

	// real/int conversion
	//template<typename RealType>
	//explicit operator Vector4<RealType>() const {
	//	return Vector4<RealType>((RealType)X, (RealType)Y, (RealType)Z, (RealType)W);
	//}
	//template<typename RealType>
	//explicit IntVector4(const Vector4<RealType>& Vec)
	//	: X((IntType)Vec.X), Y((IntType)Vec.Y), Z((IntType)Vec.Z), W((IntType)Vec.W)
	//{}

	//int/int type conversions
	template<typename IntType2>
	explicit operator IntVector4<IntType2>() const {
		return IntVector4<IntType2>((IntType2)X, (IntType2)Y, (IntType2)Z, (IntType2)W);
	}
	template<typename IntType2>
	explicit IntVector4(const IntVector4<IntType2>& Vec)
		: X((IntType)Vec.X), Y((IntType)Vec.Y), Z((IntType)Vec.Z), W((IntType)Vec.W)
	{}

	template<typename IndexType>
	explicit operator Index4<IndexType>() const {
		return Index4<IndexType>((IndexType)X, (IndexType)Y, (IndexType)Z, (IndexType)W);
	}
	template<typename IndexType>
	explicit IntVector4(const Index4<IndexType>& Idx)
		: X((IntType)Idx.A), Y((IntType)Idx.B), Z((IntType)Idx.C), W((IntType)Idx.D)
	{}

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	inline operator UE::Math::TIntVector4<IntType>() const {
		return UE::Math::TIntVector4<IntType>(X, Y, Z, W);
	}
	inline IntVector4(const UE::Math::TIntVector4<IntType>& Vec)
		: X(Vec.X), Y(Vec.Y), Z(Vec.Z), W(Vec.W) {}
	inline operator UE::Geometry::FVector4i() const {
		return UE::Geometry::FVector4i((int)X, (int)Y, (int)Z, (int)W);
	}
	inline IntVector4(const UE::Geometry::FVector4i& Vec)
		: X((IntType)Vec.X), Y((IntType)Vec.Y), Z((IntType)Vec.Z), W((IntType)Vec.W) {}
	inline operator UE::Geometry::FIndex4i() const {
		return UE::Geometry::FIndex4i((int)X, (int)Y, (int)Z, (int)W);
	}
	inline IntVector4(const UE::Geometry::FIndex4i& Vec)
		: X((IntType)Vec.A), Y((IntType)Vec.B), Z((IntType)Vec.C), W((IntType)Vec.D) {}
	template<typename RealType>
	explicit operator UE::Math::TVector4<RealType>() const {
		return UE::Math::TVector<RealType>((RealType)X, (RealType)Y, (RealType)Z, (RealType)W);
	}
	template<typename RealType>
	explicit IntVector4(const UE::Math::TVector4<RealType>& Vec)
		: X((IntType)Vec.X), Y((IntType)Vec.Y), Z((IntType)Vec.Z), W((IntType)Vec.W)
	{}
#endif

};
typedef IntVector4<int> Vector4i;
typedef IntVector4<int64_t> Vector4i64;
typedef IntVector4<uint64_t> Vector4u64;


template<typename IntType>
constexpr IntVector4<IntType>::IntVector4()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<IntVector4<IntType>>());
	static_assert(std::is_trivially_copyable<IntVector4<IntType>>());
	static_assert(std::is_integral<IntType>());
}

template<typename IntType>
constexpr IntVector4<IntType>::IntVector4(IntType InitialValue)
{
	X = Y = Z = InitialValue;
}

template<typename IntType>
constexpr IntVector4<IntType>::IntVector4(IntType x, IntType y, IntType z, IntType w)
{
	X = x; Y = y; Z = z; W = w;
}

template<typename IntType>
constexpr IntVector4<IntType>::IntVector4(const IntType* Values)
{
	X = Values[0]; Y = Values[1]; Z = Values[2]; W = Values[3];
}

template<typename IntType>
inline IntVector4<IntType> operator*(IntType Scalar, const IntVector4<IntType>& Vec)
{
	return IntVector4<IntType>(Scalar * Vec.X, Scalar * Vec.Y, Scalar * Vec.Z, Scalar *  Vec.W);
}


template<typename IntType>
uint32 GetTypeHash(const IntVector4<IntType>& Vector)
{
	return GS::HashVector4(Vector.X, Vector.Y, Vector.Z, Vector.W);
}


} // end namespace GS


// define std::hash function usable with unordered_map. maybe these should be moved to a separate file to avoid STL header dependency...
namespace std {
	template<typename IntType>
	struct hash<GS::IntVector4<IntType>> {
		inline size_t operator()(const GS::IntVector4<IntType>& Vector) const {
			// size_t value = your hash computations over x
			return (size_t)GS::HashVector4(Vector.X, Vector.Y, Vector.Z, Vector.W);
		}
	};
}



