// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSIntVector3.h"
#include "Math/GSAxisBox3.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "IntBoxTypes.h"
#include "BoxTypes.h"
#include "Math/Box.h"
#endif

namespace GS
{

/**
 * Integer bounding box. Note that an integer bounding box is quite different from a float bounding box because
 * of the uncertaintly about whether the Max values are meant to be included (ie "inside" is Min <= x <= Max) or if they
 * are not ("inside" is Min <= x < Max)m where the latter corresponds to (eg) looping from 0 to vector.size().
 * The question is whether the box is containing a set of integers, or if it represents a range of indices. 
 * IntAxisBox3 functions generally treat Max as Inclusive, although some functions have Exclusive versions.
 * 
 * (perhaps there should be an IndexBox3, if this comes up often...)
 */
template<typename IntType>
struct IntAxisBox3
{
	IntVector3<IntType> Min;
	IntVector3<IntType> Max;

	IntAxisBox3();
	IntAxisBox3(const IntVector3<IntType>& MinCorner, const IntVector3<IntType>& MaxCorner);

	//! Empty box is defined by min-corner being larger than max-corner
	constexpr static IntAxisBox3 Empty() { return IntAxisBox3(IntVector3<IntType>(IntMath<IntType>::MaxValue()), IntVector3<IntType>(-IntMath<IntType>::MaxValue())); }
	//! Empty box is defined by min-corner being larger than max-corner
	constexpr static IntAxisBox3 Infinite() { return IntAxisBox3(IntVector3<IntType>(-IntMath<IntType>::MaxValue()), IntVector3<IntType>(IntMath<IntType>::MaxValue())); }
	//! zero box contains the point (0,0,0)
	constexpr static IntAxisBox3 Zero() { return IntAxisBox3(IntVector3<IntType>::Zero(), IntVector3<IntType>::Zero()); }

	//! return true if box contains no points, ie Empty() or similar
	bool IsValid() const;

	// using 'Count' instead of 'Dimension' here to emphasize that these functions are inclusive

	//! Count is the inclusive number of integers along an axis, this is (Max-Min+1) because (eg) range [-2,1] includes 4 values, not 3  (-2,-1,0,1)
	int64_t CountX() const { return GS::Max<int64_t>((int64_t)Max.X - (int64_t)Min.X + (int64_t)1, (int64_t)0); }
	int64_t CountY() const { return GS::Max<int64_t>((int64_t)Max.Y - (int64_t)Min.Y + (int64_t)1, (int64_t)0); }
	int64_t CountZ() const { return GS::Max<int64_t>((int64_t)Max.Z - (int64_t)Min.Z + (int64_t)1, (int64_t)0); }
	//! Integer dimension is inclusive (Max-Min+1), because (eg) range [-2,1] includes 4 values, not 3  (-2,-1,0,1)
	int64_t AxisCount(int AxisIndex) const { return GS::Max<int64_t>((int64_t)Max[AxisIndex] - (int64_t)Min[AxisIndex] + (int64_t)1, (int64_t)0); }

	IntVector3<int64_t> AxisCounts() const;
	//! inclusive volume, which means that the Zero() box still contains one element and Volume=1
	int64_t VolumeCount() const;
	//! inclusive area, which means that the Zero() box still contains one element and SurfaceArea=6
	int64_t SurfaceAreaCount() const;

	//! returns box corner for i=[0,7]. Order is 2D CCW X/Y from bottom-left, -Z and then +Z , ie [ (-x,-y,-z), (x,-y), (x,y), (-x,y), (-x,-y,z), ... ]
	IntVector3<IntType> BoxCorner(int i) const;
	//! returns point on face/edge/corner. For each coord value neg==min, 0==center, pos==max. So (eg) center of zmin face is (0,0,-1).
	IntVector3<IntType> BoxPoint(int xi, int yi, int zi) const;

	// note: no Center or Extents because can't divide by 2 if dimension is odd. Caller has to handle this.

	void Contain(const IntVector3<IntType>& Point);
	void Contain(const IntAxisBox3<IntType>& OtherBox);

	//! containment test is inclusive [-Min,Max]
	bool Contains(const IntVector3<IntType>& Point) const;
	//! exclusive containment test does not include Max, ie [Min,Max), so valid value is >= Min, < Max
	bool ContainsExclusive(const IntVector3<IntType>& Point) const;
	//! containment test is inclusive [-Min,Max]
	bool AxisContains(int AxisIndex, IntType Value) const;

	constexpr bool operator==(const IntAxisBox3& Other) const {
		return Min == Other.Min && Max == Other.Max;
	}
	constexpr bool operator!=(const IntAxisBox3& Other) const {
		return Min != Other.Min || Max != Other.Max;
	}


	template<typename RealType>
	explicit inline operator AxisBox3<RealType>() const {
		return AxisBox3<RealType>((Vector3<RealType>)Min, (Vector3<RealType>)Max);
	}
	template<typename RealType>
	explicit inline IntAxisBox3(const AxisBox3<RealType>& Boxf)
		: Min((IntVector3<IntType>)Boxf.Min), Max((IntVector3<IntType>)Boxf.Max) {}



	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	template<typename RealType>
	explicit inline operator UE::Geometry::TAxisAlignedBox3<RealType>() const {
		return UE::Geometry::TAxisAlignedBox3<RealType>((UE::Math::TVector<RealType>)Min, (UE::Math::TVector<RealType>)Max);
	}
	template<typename RealType>
	explicit inline IntAxisBox3(const UE::Geometry::TAxisAlignedBox3<RealType>& Boxf)
		: Min((IntVector3<IntType>)Boxf.Min), Max((IntVector3<IntType>)Boxf.Max) {}
	explicit inline operator UE::Geometry::FAxisAlignedBox3i() const {
		return UE::Geometry::FAxisAlignedBox3i((UE::Geometry::FVector3i)Min, (UE::Geometry::FVector3i)Max);
	}
	explicit inline IntAxisBox3(const UE::Geometry::FAxisAlignedBox3i& Box)
		: Min((IntVector3<IntType>)Box.Min), Max((IntVector3<IntType>)Box.Max) {}
#endif

};
typedef IntAxisBox3<int> AxisBox3i;

template<typename IntType>
IntAxisBox3<IntType>::IntAxisBox3()
{}

template<typename IntType>
IntAxisBox3<IntType>::IntAxisBox3(const IntVector3<IntType>& MinCorner, const IntVector3<IntType>& MaxCorner)
	: Min(MinCorner), Max(MaxCorner)
{}


template<typename IntType>
bool IntAxisBox3<IntType>::IsValid() const
{
	return Max.X >= Min.X && Max.Y >= Min.Y && Max.Z >= Min.Z;
}



template<typename IntType>
IntVector3<int64_t> IntAxisBox3<IntType>::AxisCounts() const
{
	return IntVector3<int64_t>(CountX(), CountY(), CountZ());
}


template<typename IntType>
int64_t IntAxisBox3<IntType>::VolumeCount() const
{
	return CountX() * CountY() * CountZ();
}

template<typename IntType>
int64_t IntAxisBox3<IntType>::SurfaceAreaCount() const
{
	return (IntType)2 * (CountX() * (CountY() + CountZ()) + CountY() * CountZ());
}



// currently works like this, copied from geometry3Sharp Box3 class...
// corners [ (-x,-y), (x,-y), (x,y), (-x,y) ], -z, then +z
//
//   7---6     +z       or        3---2     -z
//   |\  |\                       |\  |\
//   4-\-5 \                      0-\-1 \
//    \ 3---2                      \ 7---6   
//     \|   |                       \|   |
//      0---1  -z                    4---5  +z
template<typename IntType>
IntVector3<IntType> IntAxisBox3<IntType>::BoxCorner(int i) const
{
	// seems like x here might have some issue? "comparison operator has higher precedence than bitwise operator" ?
	IntType x = (((i & 1) != 0) ^ ((i & 2) != 0)) ? (Max.X) : (Min.X);
	IntType y = ((i / 2) % 2 == 0) ? (Min.Y) : (Max.Y);
	IntType z = (i < 4) ? (Min.Z) : (Max.Z);
	return IntVector3<IntType>(x, y, z);
}

template<typename IntType>
IntVector3<IntType> IntAxisBox3<IntType>::BoxPoint(int xi, int yi, int zi) const
{
	IntType x = (xi < 0) ? Min.X : ((xi == 0) ? ((Min.X + Max.X)/2) : Max.X);
	IntType y = (yi < 0) ? Min.Y : ((yi == 0) ? ((Min.Y + Max.Y)/2) : Max.Y);
	IntType z = (zi < 0) ? Min.Z : ((zi == 0) ? ((Min.Z + Max.Z)/2) : Max.Z);
	return IntVector3<IntType>(x, y, z);
}



template<typename IntType>
void IntAxisBox3<IntType>::Contain(const IntVector3<IntType>& Point)
{
	Min.X = GS::Min(Min.X, Point.X);
	Min.Y = GS::Min(Min.Y, Point.Y);
	Min.Z = GS::Min(Min.Z, Point.Z);
	Max.X = GS::Max(Max.X, Point.X);
	Max.Y = GS::Max(Max.Y, Point.Y);
	Max.Z = GS::Max(Max.Z, Point.Z);
}

template<typename IntType>
void IntAxisBox3<IntType>::Contain(const IntAxisBox3<IntType>& OtherBox)
{
	Min.X = GS::Min(Min.X, OtherBox.Min.X);
	Min.Y = GS::Min(Min.Y, OtherBox.Min.Y);
	Min.Z = GS::Min(Min.Z, OtherBox.Min.Z);
	Max.X = GS::Max(Max.X, OtherBox.Max.X);
	Max.Y = GS::Max(Max.Y, OtherBox.Max.Y);
	Max.Z = GS::Max(Max.Z, OtherBox.Max.Z);
}


template<typename IntType>
bool IntAxisBox3<IntType>::Contains(const IntVector3<IntType>& Point) const
{
	return (Min.X <= Point.X) && (Min.Y <= Point.Y) && (Min.Z <= Point.Z)
		&& (Max.X >= Point.X) && (Max.Y >= Point.Y) && (Max.Z >= Point.Z);
}

template<typename IntType>
bool IntAxisBox3<IntType>::ContainsExclusive(const IntVector3<IntType>& Point) const
{
	return (Min.X <= Point.X) && (Min.Y <= Point.Y) && (Min.Z <= Point.Z)
		&& (Max.X > Point.X) && (Max.Y > Point.Y) && (Max.Z > Point.Z);
}

template<typename IntType>
bool IntAxisBox3<IntType>::AxisContains(int AxisIndex, IntType Value) const
{
	return Min[AxisIndex] <= Value && Value <= Max[AxisIndex];
}


} // end namespace GS

