// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSVector3.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "Math/Color.h"
#endif

namespace GS
{


struct PackedColor3b
{
	union
	{
		struct
		{
			uint8_t R;
			uint8_t G;
			uint8_t B;
		};
		uint32_t PackedFields : 24;
	};

	constexpr PackedColor3b() { PackedFields = 0; }
	constexpr PackedColor3b(uint8_t Red, uint8_t Green, uint8_t Blue)
		: R(Red), G(Green), B(Blue) {}
	constexpr PackedColor3b(uint32_t PackedColor)
		: PackedFields(PackedColor) {}

	constexpr explicit operator uint32_t() const { return PackedFields; }
};



struct Color3b
{
	union
	{
		struct
		{
			uint8_t R;
			uint8_t G;
			uint8_t B;
		};
		uint8_t RGB[3] = { {}, {}, {} };
	};

	static Color3b White()		{ return Color3b(255,255,255); }
	static Color3b Black()		{ return Color3b(0, 0, 0); }
	static Color3b Grey()		{ return Color3b(127, 127, 127); }
	static Color3b Red()		{ return Color3b(255,0,0); }
	static Color3b Green()		{ return Color3b(0,255,0); }
	static Color3b Blue()		{ return Color3b(0,0,255); }
	static Color3b HotPink()	{ return Color3b(255,105,180); }

	bool operator==(const Color3b& Other) const
	{
		return R == Other.R && G == Other.G && B == Other.B;
	}

	constexpr Color3b() = default;

	constexpr Color3b(uint8_t Red, uint8_t Green, uint8_t Blue)
		: R(Red), G(Green), B(Blue) {}

	template<typename RealType>
	explicit Color3b(const Vector3<RealType>& Vec)
	{
		R = (uint8_t)GS::Clamp((int)(Vec.X * (RealType)255), 0, 255);
		G = (uint8_t)GS::Clamp((int)(Vec.Y * (RealType)255), 0, 255);
		B = (uint8_t)GS::Clamp((int)(Vec.Z * (RealType)255), 0, 255);
	}

	uint8_t& operator[](int index) {
		return RGB[index];
	}

	uint8_t operator[](int index) const {
		return RGB[index];
	}

	constexpr explicit operator PackedColor3b() const {
		return PackedColor3b(R, G, B);
	}

	template<typename RealType>
	constexpr explicit operator Vector3<RealType>() const {
		return Vector3<RealType>((RealType)R / (RealType)255, (RealType)G / (RealType)255, (RealType)B / (RealType)255);
	}

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY

	explicit inline operator FColor() const	{
		return FColor(R, G, B, 255);
	}
	explicit inline Color3b(const FColor& Color)
		: R(Color.R), G(Color.G), B(Color.B)
	{}

#endif


};


} // end namespace GS
