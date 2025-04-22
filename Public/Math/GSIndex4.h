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
struct Index4
{
	union {
		struct {
			IndexType A;
			IndexType B;
			IndexType C;
			IndexType D;
		};
		IndexType ABCD[4] = { {}, {}, {}, {} };
	};

	Index4();
	explicit Index4(IndexType InitialValue);
	explicit Index4(IndexType a, IndexType b, IndexType c, IndexType d);
	explicit Index4(const IndexType* Values);

	static Index4 Zero() { return Index4(0); }
	static Index4 One() { return Index4(1); }
	static Index4 NegOne() { return Index4(-1); }
	static Index4 MakeUnit(int Axis, IndexType Sign = 1) { Index4 V = Zero(); V[Axis] = Sign * 1; return V; }

	IndexType& operator[](int index) {
		return ABCD[index];
	}

	IndexType operator[](int index) const {
		return ABCD[index];
	}

	IndexType* AsPointer() {
		return &ABCD[0];
	}
	const IndexType* AsPointer() const {
		return &ABCD[0];
	}

	constexpr bool operator==(const Index4& Other) const {
		return A == Other.A && B == Other.B && C == Other.C && D == Other.D;
	}
	constexpr bool operator!=(const Index4& Other) const {
		return A != Other.A || B != Other.B || C != Other.C || D != Other.D;
	}
	constexpr bool operator<(const Index4& Other) const {
		if (A != Other.A)		return A < Other.A;
		else if (B != Other.B)	return B < Other.B;
		else if (C != Other.C)	return C < Other.C;
		else if (D != Other.D)	return D < Other.D;
		else return false;
	}

	constexpr bool Contains(IndexType Value) const {
		return A == Value || B == Value || C == Value || D == Value;
	}

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	inline operator UE::Math::TIntVector4<IndexType>() const {
		return UE::Math::TIntVector4<IndexType>(A, B, C, D);
	}
	inline Index4(const UE::Math::TIntVector4<IndexType>& Vec)
		: A(Vec.X), B(Vec.Y), C(Vec.Z), D(Vec.W) {}
	inline operator UE::Geometry::FIndex4i() const {
		return UE::Geometry::FIndex4i((int)A, (int)B, (int)C, (int)D);
	}
	inline Index4(const UE::Geometry::FIndex4i& Vec)
		: A((IndexType)Vec.A), B((IndexType)Vec.B), C((IndexType)Vec.C), D((IndexType)Vec.D) {}
#endif

};
typedef Index4<int> Index4i;


template<typename IndexType>
Index4<IndexType>::Index4()
{
	static_assert(std::is_integral<IndexType>());
	static_assert(std::is_standard_layout<Index4<IndexType>>());
	static_assert(std::is_trivially_copyable<Index4<IndexType>>());
}

template<typename IndexType>
Index4<IndexType>::Index4(IndexType InitialValue)
{
	A = B = C = D = InitialValue;
}

template<typename IndexType>
Index4<IndexType>::Index4(IndexType a, IndexType b, IndexType c, IndexType d)
{
	A = a; B = b; C = c; D = d;
}

template<typename IndexType>
Index4<IndexType>::Index4(const IndexType* Values)
{
	A = Values[0]; B = Values[1]; C = Values[2]; D = Values[3];
}


} // end namespace GS
