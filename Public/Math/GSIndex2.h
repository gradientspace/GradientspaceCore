// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSHash.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Math/IntVector.h"
#include "IndexTypes.h"
#endif

#include <xhash>  // works for std::hash defined at bottom? or is <unordered_set> required?


namespace GS
{

template<typename IndexType>
struct Index2
{
	union {
		struct {
			IndexType A;
			IndexType B;
		};
		IndexType AB[2] = { {}, {} };
	};

	Index2();
	explicit Index2(IndexType InitialValue);
	explicit Index2(IndexType a, IndexType b);
	explicit Index2(const IndexType* Values);

	static Index2 Zero() { return Index2(0); }
	static Index2 One() { return Index2(1); }
	static Index2 NegOne() { return Index2(-1); }
	static Index2 UnitX() { return Index2(1, 0); }
	static Index2 UnitY() { return Index2(0, 1); }
	static Index2 MakeUnit(int Axis, IndexType Sign = 1) { Index2 V = Zero(); V[Axis] = Sign * 1; return V; }

	IndexType& operator[](int index) {
		return AB[index];
	}

	IndexType operator[](int index) const {
		return AB[index];
	}

	IndexType* AsPointer() {
		return &AB[0];
	}
	const IndexType* AsPointer() const {
		return &AB[0];
	}

	constexpr bool operator==(const Index2& Other) const {
		return A == Other.A && B == Other.B;
	}
	constexpr bool operator!=(const Index2& Other) const {
		return A != Other.A || B != Other.B;
	}
	constexpr bool operator<(const Index2& Other) const {
		if (A != Other.A)		return A < Other.A;
		else if (B != Other.B)	return B < Other.B;
		else return false;
	}

	constexpr bool Contains(IndexType Value) const {
		return A == Value || B == Value;
	}

	//! return B if SearchValue==A, otherwise return A. Does not handle case where SearchValue != A or B
	constexpr IndexType GetOtherValue(IndexType SearchValue) const {
		return (A == SearchValue) ? B : A;
	}

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	inline operator UE::Math::TIntVector2<IndexType>() const {
		return UE::Math::TIntVector2<IndexType>(A, B);
	}
	inline Index2(const UE::Math::TIntVector2<IndexType>& Vec)
		: A(Vec.X), B(Vec.Y) {}
	inline operator UE::Geometry::FIndex2i() const {
		return UE::Geometry::FIndex2i((int)A, (int)B);
	}
	inline Index2(const UE::Geometry::FIndex2i& Vec)
		: A((IndexType)Vec.A), B((IndexType)Vec.B) {}
#endif

};
typedef Index2<int> Index2i;


template<typename IndexType>
Index2<IndexType>::Index2()
{
	static_assert(std::is_integral<IndexType>());
	static_assert(std::is_standard_layout<Index2<IndexType>>());
	static_assert(std::is_trivially_copyable<Index2<IndexType>>());
}

template<typename IndexType>
Index2<IndexType>::Index2(IndexType InitialValue)
{
	A = B = InitialValue;
}

template<typename IndexType>
Index2<IndexType>::Index2(IndexType a, IndexType b)
{
	A = a; B = b;
}

template<typename IndexType>
Index2<IndexType>::Index2(const IndexType* Values)
{
	A = Values[0]; B = Values[1];
}


template<typename IntType>
uint32_t GetTypeHash(const Index2<IntType>& Index)
{
	return GS::HashVector2(Index.A, Index.B);
}


} // end namespace GS



// define std::hash function usable with unordered_map. maybe these should be moved to a separate file to avoid STL header dependency...
namespace std {
	template<typename IntType>
	struct hash<GS::Index2<IntType>> {
		inline size_t operator()(const GS::Index2<IntType>& Index) const {
			// size_t value = your hash computations over x
			return (size_t)GS::HashVector2(Index.A, Index.B);
		}
	};
}
