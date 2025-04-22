// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSVector3.h"
#include "Math/GSVector4.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Math/Color.h"
#endif

namespace GS
{

struct PackedColor4b
{
	union
	{
		struct
		{
			uint8_t R;
			uint8_t G;
			uint8_t B;
			uint8_t A;
		};
		uint32_t PackedFields;
	};

	PackedColor4b() { PackedFields = 0; A = 255; }
	constexpr PackedColor4b(uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Alpha)
		: R(Red), G(Green), B(Blue), A(Alpha) {}
	constexpr PackedColor4b(uint32_t PackedColor)
		: PackedFields(PackedColor) {}

	constexpr explicit operator uint32_t() const { return PackedFields; }
};


template<typename IntType>
struct IntColor4
{
	union {
		struct {
			IntType R;
			IntType G;
			IntType B;
			IntType A;
		};
		IntType RGBA[4] = { {}, {}, {}, {} };
	};

	constexpr IntColor4();
	constexpr IntColor4(IntType r, IntType g, IntType b);
	constexpr IntColor4(IntType r, IntType g, IntType b, IntType a);
	constexpr IntColor4(const IntType* Values, uint8_t NumValues);

	IntType& operator[](int Index) {
		return RGBA[Index];
	}

	IntType operator[](int Index) const {
		return RGBA[Index];
	}

	IntType* AsPointer() {
		return &RGBA[0];
	}
	const IntType* AsPointer() const {
		return &RGBA[0];
	}

	bool operator==(const IntColor4<IntType>& Other) const	{
		return R == Other.R && G == Other.G && B == Other.B && A == Other.A;
	}

	static IntColor4<IntType> White() { return IntColor4<IntType>(255, 255, 255); }
	static IntColor4<IntType> Black() { return IntColor4<IntType>(0, 0, 0); }
	static IntColor4<IntType> Grey() { return IntColor4<IntType>(127, 127, 127); }
	static IntColor4<IntType> Red() { return IntColor4<IntType>(255, 0, 0); }
	static IntColor4<IntType> Green() { return IntColor4<IntType>(0, 255, 0); }
	static IntColor4<IntType> Blue() { return IntColor4<IntType>(0, 0, 255); }


	template<typename RealType>
	constexpr explicit operator Vector3<RealType>() const {
		return Vector3<RealType>((RealType)R / (RealType)255, (RealType)G / (RealType)255, (RealType)B / (RealType)255);
	}
	template<typename RealType>
	constexpr explicit operator Vector4<RealType>() const {
		return Vector4<RealType>((RealType)R / (RealType)255, (RealType)G / (RealType)255, (RealType)B / (RealType)255, (RealType)A / (RealType)255);
	}

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	explicit inline operator FColor() const {
		return FColor((uint8_t)R, (uint8_t)G, (uint8_t)B, (uint8_t)A);
	}
	explicit inline IntColor4(const FColor& Col)
		: R((IntType)Col.R), G((IntType)Col.G), B((IntType)Col.B), A((IntType)Col.A) {}
#endif

};
typedef IntColor4<uint8_t> Color4b;
typedef IntColor4<int> Color4i;



template<typename IntType>
constexpr IntColor4<IntType>::IntColor4()
{
	static_assert(std::is_standard_layout<IntColor4<IntType>>());
	static_assert(std::is_trivially_copyable<IntColor4<IntType>>());
}

template<typename IntType>
constexpr IntColor4<IntType>::IntColor4(IntType r, IntType g, IntType b)
{
	R = r; G = g; B = b; A = 255;
}

template<typename IntType>
constexpr IntColor4<IntType>::IntColor4(IntType r, IntType g, IntType b, IntType a)
{
	R = r; G = g; B = b; A = a;
}

template<typename IntType>
constexpr IntColor4<IntType>::IntColor4(const IntType* Values, uint8_t NumValues)
{
	uint8_t MaxVals = GS::Max(NumValues, (uint8_t)4);
	for (uint8_t j = 0; j < MaxVals; ++j)
		RGBA[j] = Values[j];
}



} // end namespace GS
