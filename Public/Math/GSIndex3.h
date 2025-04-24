// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSHash.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Math/IntVector.h"
#include "IndexTypes.h"
#endif


namespace GS
{

template<typename IndexType>
struct Index3
{
	union {
		struct {
			IndexType A;
			IndexType B;
			IndexType C;
		};
		IndexType ABC[3] = { {}, {}, {} };
	};

	Index3();
	explicit Index3(IndexType InitialValue);
	explicit Index3(IndexType a, IndexType b, IndexType c);
	explicit Index3(const IndexType* Values);

	static Index3 Zero() { return Index3(0); }
	static Index3 One() { return Index3(1); }
	static Index3 NegOne() { return Index3(-1); }
	static Index3 UnitX() { return Index3(1, 0, 0); }
	static Index3 UnitY() { return Index3(0, 1, 0); }
	static Index3 UnitZ() { return Index3(0, 0, 1); }
	static Index3 MakeUnit(int Axis, IndexType Sign = 1) { Index3 V = Zero(); V[Axis] = Sign * 1; return V; }

	IndexType& operator[](int index) {
		return static_cast<IndexType*>(&A)[index];
	}

	IndexType operator[](int index) const {
		return static_cast<const IndexType*>(&A)[index];
	}

	IndexType* AsPointer() {
		return &ABC[0];
	}
	const IndexType* AsPointer() const {
		return &ABC[0];
	}

	constexpr bool operator==(const Index3& Other) const {
		return A == Other.A && B == Other.B && C == Other.C;
	}
	constexpr bool operator!=(const Index3& Other) const {
		return A != Other.A || B != Other.B || C != Other.C;
	}
	constexpr bool operator<(const Index3& Other) const {
		if (A != Other.A)		return A < Other.A;
		else if (B != Other.B)	return B < Other.B;
		else if (C != Other.C)	return C < Other.C;
		else return false;
	}

	constexpr bool Contains(IndexType Value) const {
		return A == Value || B == Value || C == Value;
	}


	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	inline operator UE::Math::TIntVector3<IndexType>() const {
		return UE::Math::TIntVector3<IndexType>(A, B, C);
	}
	inline Index3(const UE::Math::TIntVector3<IndexType>& Vec)
		: A(Vec.X), B(Vec.Y), C(Vec.Z) {}
	inline operator UE::Geometry::FIndex3i() const {
		return UE::Geometry::FIndex3i((int)A, (int)B, (int)C);
	}
	inline Index3(const UE::Geometry::FIndex3i& Vec)
		: A((IndexType)Vec.A), B((IndexType)Vec.B), C((IndexType)Vec.C) {}
#endif

};
typedef Index3<int> Index3i;


template<typename IndexType>
Index3<IndexType>::Index3()
{
	static_assert(std::is_integral<IndexType>());
	static_assert(std::is_standard_layout<Index3<IndexType>>());
	static_assert(std::is_trivially_copyable<Index3<IndexType>>());
}

template<typename IndexType>
Index3<IndexType>::Index3(IndexType InitialValue)
{
	A = B = C = InitialValue;
}

template<typename IndexType>
Index3<IndexType>::Index3(IndexType a, IndexType b, IndexType c)
{
	A = a; B = b; C = c;
}

template<typename IndexType>
Index3<IndexType>::Index3(const IndexType* Values)
{
	A = Values[0]; B = Values[1]; C = Values[2];
}



template<typename IntType>
uint32_t GetTypeHash(const Index3<IntType>& Index)
{
	return GS::HashVector3(Index.A, Index.B, Index.C);
}


} // end namespace GS


// define std::hash function usable with unordered_map. maybe these should be moved to a separate file to avoid STL header dependency...
namespace std {
	template<typename IntType>
	struct hash<GS::Index3<IntType>> {
		inline size_t operator()(const GS::Index3<IntType>& Index) const {
			// size_t value = your hash computations over x
			return (size_t)GS::HashVector3(Index.A, Index.B, Index.C);
		}
	};
}
