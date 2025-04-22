// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSIntVector2.h"
#include "Math/GSAxisBox2.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "IntBoxTypes.h"
#include "BoxTypes.h"
#endif

namespace GS
{

/**
 * Integer bounding box. Note that an integer bounding box is quite different from a float bounding box because
 * of the uncertaintly about whether the Max values are meant to be included (ie "inside" is Min <= x <= Max) or if they
 * are not ("inside" is Min <= x < Max)m where the latter corresponds to (eg) looping from 0 to vector.size().
 * The question is whether the box is containing a set of integers, or if it represents a range of indices. 
 * IntAxisBox2 functions generally treat Max as Inclusive, although some functions have Exclusive versions.
 * 
 * (perhaps there should be an IndexBox2, if this comes up often...)
 */
template<typename IntType>
struct IntAxisBox2
{
	IntVector2<IntType> Min;
	IntVector2<IntType> Max;

	IntAxisBox2();
	IntAxisBox2(const IntVector2<IntType>& MinCorner, const IntVector2<IntType>& MaxCorner);

	//! Empty box is defined by min-corner being larger than max-corner
	constexpr static IntAxisBox2 Empty() { return IntAxisBox2(IntVector2<IntType>(IntMath<IntType>::MaxValue()), IntVector2<IntType>(-IntMath<IntType>::MaxValue())); }
	//! Empty box is defined by min-corner being larger than max-corner
	constexpr static IntAxisBox2 Infinite() { return IntAxisBox2(IntVector2<IntType>(-IntMath<IntType>::MaxValue()), IntVector2<IntType>(IntMath<IntType>::MaxValue())); }
	//! zero box contains the point (0,0,0)
	constexpr static IntAxisBox2 Zero() { return IntAxisBox2(IntVector2<IntType>::Zero(), IntVector2<IntType>::Zero()); }

	//! return true if box contains no points, ie Empty() or similar
	bool IsValid() const;

	// using 'Count' instead of 'Dimension' here to emphasize that these functions are inclusive

	//! Count is the inclusive number of integers along an axis, this is (Max-Min+1) because (eg) range [-2,1] includes 4 values, not 2  (-2,-1,0,1)
	int64_t CountX() const { return GS::Max<int64_t>((int64_t)Max.X - (int64_t)Min.X + (int64_t)1, (int64_t)0); }
	int64_t CountY() const { return GS::Max<int64_t>((int64_t)Max.Y - (int64_t)Min.Y + (int64_t)1, (int64_t)0); }
	//! Integer dimension is inclusive (Max-Min+1), because (eg) range [-2,1] includes 4 values, not 2  (-2,-1,0,1)
	int64_t AxisCount(int AxisIndex) const { return GS::Max<int64_t>((int64_t)Max[AxisIndex] - (int64_t)Min[AxisIndex] + (int64_t)1, (int64_t)0); }

	IntVector2<int64_t> AxisCounts() const;
	//! inclusive area, which means that the Zero() box still contains one element and Area=1
	int64_t AreaCount() const;

	//! returns box corner for i=[0,3]. Order is 2D CCW X/Y from bottom-left, ie [ (-x,-y), (x,-y), (x,y), (-x,y) ]
	IntVector2<IntType> BoxCorner(int i) const;
	//! returns point on face/edge/corner. For each coord value neg==min, 0==center, pos==max. So (eg) center of zmin face is (0,0,-1).
	IntVector2<IntType> BoxPoint(int xi, int yi) const;

	// note: no Center or Extents because can't divide by 2 if dimension is odd. Caller has to handle this.

	void Contain(const IntVector2<IntType>& Point);
	void Contain(const IntAxisBox2<IntType>& OtherBox);

	//! containment test is inclusive [-Min,Max]
	bool Contains(const IntVector2<IntType>& Point) const;
	//! exclusive containment test does not include Max, ie [Min,Max), so valid value is >= Min, < Max
	bool ContainsExclusive(const IntVector2<IntType>& Point) const;
	//! containment test is inclusive [-Min,Max]
	bool AxisContains(int AxisIndex, IntType Value) const;

	constexpr bool operator==(const IntAxisBox2& Other) const {
		return Min == Other.Min && Max == Other.Max;
	}
	constexpr bool operator!=(const IntAxisBox2& Other) const {
		return Min != Other.Min || Max != Other.Max;
	}


	template<typename RealType>
	explicit inline operator AxisBox2<RealType>() const {
		return AxisBox2<RealType>((Vector2<RealType>)Min, (Vector2<RealType>)Max);
	}
	template<typename RealType>
	explicit inline IntAxisBox2(const AxisBox2<RealType>& Boxf)
		: Min((IntVector2<IntType>)Boxf.Min), Max((IntVector2<IntType>)Boxf.Max) {}



	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	template<typename RealType>
	explicit inline operator UE::Geometry::TAxisAlignedBox2<RealType>() const {
		return UE::Geometry::TAxisAlignedBox2<RealType>((UE::Math::TVector<RealType>)Min, (UE::Math::TVector<RealType>)Max);
	}
	template<typename RealType>
	explicit inline IntAxisBox2(const UE::Geometry::TAxisAlignedBox2<RealType>& Boxf)
		: Min((IntVector2<IntType>)Boxf.Min), Max((IntVector2<IntType>)Boxf.Max) {}
	explicit inline operator UE::Geometry::FAxisAlignedBox2i() const {
		return UE::Geometry::FAxisAlignedBox2i((UE::Geometry::FVector2i)Min, (UE::Geometry::FVector2i)Max);
	}
	explicit inline IntAxisBox2(const UE::Geometry::FAxisAlignedBox2i& Box)
		: Min((IntVector2<IntType>)Box.Min), Max((IntVector2<IntType>)Box.Max) {}
#endif

};
typedef IntAxisBox2<int> AxisBox2i;

template<typename IntType>
IntAxisBox2<IntType>::IntAxisBox2()
{}

template<typename IntType>
IntAxisBox2<IntType>::IntAxisBox2(const IntVector2<IntType>& MinCorner, const IntVector2<IntType>& MaxCorner)
	: Min(MinCorner), Max(MaxCorner)
{}


template<typename IntType>
bool IntAxisBox2<IntType>::IsValid() const
{
	return Max.X >= Min.X && Max.Y >= Min.Y;
}



template<typename IntType>
IntVector2<int64_t> IntAxisBox2<IntType>::AxisCounts() const
{
	return IntVector2<int64_t>(CountX(), CountY());
}


template<typename IntType>
int64_t IntAxisBox2<IntType>::AreaCount() const
{
	return CountX() * CountY();
}

template<typename IntType>
IntVector2<IntType> IntAxisBox2<IntType>::BoxCorner(int i) const
{
	IntType x = ((i % 3) == 0) ? (Min.X) : (Max.X);
	IntType y = ((i & 2) == 0) ? (Min.Y) : (Max.Y);
	return IntVector2<IntType>(x, y);
}

template<typename IntType>
IntVector2<IntType> IntAxisBox2<IntType>::BoxPoint(int xi, int yi) const
{
	IntType x = (xi < 0) ? Min.X : ((xi == 0) ? ((Min.X + Max.X)/2) : Max.X);
	IntType y = (yi < 0) ? Min.Y : ((yi == 0) ? ((Min.Y + Max.Y)/2) : Max.Y);
	return IntVector2<IntType>(x, y);
}



template<typename IntType>
void IntAxisBox2<IntType>::Contain(const IntVector2<IntType>& Point)
{
	Min.X = GS::Min(Min.X, Point.X);
	Min.Y = GS::Min(Min.Y, Point.Y);
	Max.X = GS::Max(Max.X, Point.X);
	Max.Y = GS::Max(Max.Y, Point.Y);
}

template<typename IntType>
void IntAxisBox2<IntType>::Contain(const IntAxisBox2<IntType>& OtherBox)
{
	Min.X = GS::Min(Min.X, OtherBox.Min.X);
	Min.Y = GS::Min(Min.Y, OtherBox.Min.Y);
	Max.X = GS::Max(Max.X, OtherBox.Max.X);
	Max.Y = GS::Max(Max.Y, OtherBox.Max.Y);
}


template<typename IntType>
bool IntAxisBox2<IntType>::Contains(const IntVector2<IntType>& Point) const
{
	return (Min.X <= Point.X) && (Min.Y <= Point.Y)
		&& (Max.X >= Point.X) && (Max.Y >= Point.Y);
}

template<typename IntType>
bool IntAxisBox2<IntType>::ContainsExclusive(const IntVector2<IntType>& Point) const
{
	return (Min.X <= Point.X) && (Min.Y <= Point.Y)
		&& (Max.X > Point.X) && (Max.Y > Point.Y);
}

template<typename IntType>
bool IntAxisBox2<IntType>::AxisContains(int AxisIndex, IntType Value) const
{
	return Min[AxisIndex] <= Value && Value <= Max[AxisIndex];
}


} // end namespace GS

