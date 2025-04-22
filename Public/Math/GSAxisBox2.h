// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSVector2.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "BoxTypes.h"
#include "Math/Box.h"
#endif

namespace GS
{

template<typename RealType>
struct AxisBox2
{
	Vector2<RealType> Min;
	Vector2<RealType> Max;

	AxisBox2();
	AxisBox2(const Vector2<RealType>& MinCorner, const Vector2<RealType>& MaxCorner);
	AxisBox2(const Vector2<RealType>& A, const Vector2<RealType>& B, const Vector2<RealType>& C);

	//! Empty box is defined by min-corner being larger than max-corner
	constexpr static AxisBox2 Empty() { return AxisBox2(Vector2<RealType>(RealMath<RealType>::SafeMaxExtent()), Vector2<RealType>(-RealMath<RealType>::SafeMaxExtent()) ); }
	//! largest possible empty box, may result in overflow if box extents are used in math operations
	constexpr static AxisBox2 MaxEmpty() { return AxisBox2(Vector2<RealType>(RealMath<RealType>::MaxValue()), Vector2<RealType>(-RealMath<RealType>::MaxValue())); }
	//! zero box contains the point (0,0)
	constexpr static AxisBox2 Zero() { return AxisBox2(Vector2<RealType>::Zero(), Vector2<RealType>::Zero()); }
	//! unit box ranges from 0 to 1
	constexpr static AxisBox2 Unit() { return AxisBox2(Vector2<RealType>::Zero(), Vector2<RealType>::One()); }

	//! return true if box contains no points, ie Empty() or similar
	bool IsValid() const;

	RealType WidthX() const { return GS::Max<RealType>(Max.X - Min.X, (RealType)0); }
	RealType HeightY() const { return GS::Max<RealType>(Max.Y - Min.Y, (RealType)0); }

	RealType DimensionX() const { return GS::Max<RealType>(Max.X - Min.X, (RealType)0); }
	RealType DimensionY() const { return GS::Max<RealType>(Max.Y - Min.Y, (RealType)0); }
	RealType Dimension(int Index) const { return GS::Max<RealType>(Max[Index] - Min[Index], (RealType)0); }

	Vector2<RealType> Diagonal() const;
	Vector2<RealType> Extents() const;
	Vector2<RealType> Center() const;
	RealType Area() const;

	//! returns box corner for i=[0,3]. Order is 2D CCW X/Y from bottom-left ie [ (-x,-y), (x,-y), (x,y), (-x,y)]
	Vector2<RealType> BoxCorner(int CornerIndex) const;
	//! returns point on edge/corner. For each coord value neg==min, 0==center, pos==max
	Vector2<RealType> BoxPoint(int xi, int yi) const;

	//! containment test is inclusive
	bool Contains(const Vector2<RealType>& Point) const;
	bool Contains(const AxisBox2<RealType>& OtherBox) const;

	RealType DistanceSquared(const Vector2<RealType>& Point) const;
	RealType Distance(const Vector2<RealType>& Point) const;
	RealType SignedDistance(const Vector2<RealType>& Point) const;

	bool Intersects(const AxisBox2<RealType>& OtherBox) const;
	RealType DistanceSquared(const AxisBox2<RealType>& OtherBox) const;

	void Contain(const Vector2<RealType>& Point);
	void Contain(const AxisBox2<RealType>& OtherBox);
	void Translate(const Vector2<RealType>& Translation);
	AxisBox2 Translated(const Vector2<RealType>& Translation) const;
	void Expand(const Vector2<RealType>& AxisOffsets);


	// float/double conversion cast
	template<typename RealType2>
	explicit operator AxisBox2<RealType2>() const {
		return AxisBox2<RealType2>( (Vector2<RealType2>)Min, (Vector2<RealType2>)Max );
	}


	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	explicit inline operator UE::Geometry::TAxisAlignedBox2<RealType>() const {
		return UE::Geometry::TAxisAlignedBox2<RealType>((UE::Math::TVector2<RealType>)Min, (UE::Math::TVector2<RealType>)Max);
	}
	explicit inline AxisBox2(const UE::Geometry::TAxisAlignedBox2<RealType>& Box)
		: Min(Box.Min), Max(Box.Max) {}
	explicit inline operator UE::Math::TBox2<RealType>() const {
		if (IsValid())
			return UE::Math::TBox2<RealType>( (UE::Math::TVector<RealType>)Min, (UE::Math::TVector<RealType>)Max );
		else
			return UE::Math::TBox2<RealType>();
	}
	explicit inline AxisBox2(const UE::Math::TBox2<RealType>& Box) {
		if (Box.IsValid) {
			Min = Vector2<RealType>(Box.Min); Max = Vector2<RealType>(Box.Max);
		} else 
			*this = Empty();
	}
#endif
};
typedef AxisBox2<float> AxisBox2f;
typedef AxisBox2<double> AxisBox2d;

template<typename RealType>
AxisBox2<RealType>::AxisBox2()
{}

template<typename RealType>
AxisBox2<RealType>::AxisBox2(const Vector2<RealType>& MinCorner, const Vector2<RealType>& MaxCorner)
	: Min(MinCorner), Max(MaxCorner)
{}

template<typename RealType>
AxisBox2<RealType>::AxisBox2(const Vector2<RealType>& A, const Vector2<RealType>& B, const Vector2<RealType>& C)
{
	Min.X = GS::Min3(A.X, B.X, C.X);
	Max.X = GS::Max3(A.X, B.X, C.X);
	Min.Y = GS::Min3(A.Y, B.Y, C.Y);
	Max.Y = GS::Max3(A.Y, B.Y, C.Y);
}



template<typename RealType>
bool AxisBox2<RealType>::IsValid() const
{
	return Max.X >= Min.X && Max.Y >= Min.Y;
}

template<typename RealType>
RealType AxisBox2<RealType>::Area() const 
{ 
	return DimensionX() * DimensionY(); 
}

template<typename RealType>
Vector2<RealType> AxisBox2<RealType>::Diagonal() const
{
	return Vector2<RealType>( 
		GS::Max(Max.X-Min.X,(RealType)0), 
		GS::Max(Max.Y-Min.Y,(RealType)0) );
}

template<typename RealType>
Vector2<RealType> AxisBox2<RealType>::Extents() const
{
	return Vector2<RealType>( 
		GS::Max( (Max.X-Min.X)*(RealType)0.5, (RealType)0), 
		GS::Max( (Max.Y-Min.Y)*(RealType)0.5, (RealType)0));
}


template<typename RealType>
Vector2<RealType> AxisBox2<RealType>::Center() const
{
	return Vector2<RealType>( 
		(Min.X + Max.X)*(RealType)0.5, 
		(Min.Y + Max.Y)*(RealType)0.5 );
}


//! Corner order is (minx,miny),  (maxx,miny),  (maxx,maxy),  (minx, maxy)
template<typename RealType>
Vector2<RealType> AxisBox2<RealType>::BoxCorner(int i) const
{
	RealType X = ((i % 3) == 0) ? (Min.X) : (Max.X);
	RealType Y = ((i & 2) == 0) ? (Min.Y) : (Max.Y);
	return Vector2<RealType>(X, Y);
}



template<typename RealType>
Vector2<RealType> AxisBox2<RealType>::BoxPoint(int xi, int yi) const
{
	RealType x = (xi < 0) ? Min.X : ((xi == 0) ? ((RealType)0.5 * (Min.X + Max.X)) : Max.X);
	RealType y = (yi < 0) ? Min.Y : ((yi == 0) ? ((RealType)0.5 * (Min.Y + Max.Y)) : Max.Y);
	return Vector2<RealType>(x, y);
}

template<typename RealType>
bool AxisBox2<RealType>::Contains(const Vector2<RealType>& Point) const
{
	return (Min.X <= Point.X) && (Min.Y <= Point.Y)
		&& (Max.X >= Point.X) && (Max.Y >= Point.Y);
}

template<typename RealType>
bool AxisBox2<RealType>::Contains(const AxisBox2<RealType>& OtherBox) const
{
	return (Min.X <= OtherBox.Min.X) && (Min.Y <= OtherBox.Min.Y)
		&& (Max.X >= OtherBox.Max.X) && (Max.Y >= OtherBox.Max.Y);
}



template<typename RealType>
RealType AxisBox2<RealType>::DistanceSquared(const Vector2<RealType>& Point) const
{
	RealType dx = (Point.X < Min.X) ? Min.X - Point.X : (Point.X > Max.X ? Point.X - Max.X : 0);
	RealType dy = (Point.Y < Min.Y) ? Min.Y - Point.Y : (Point.Y > Max.Y ? Point.Y - Max.Y : 0);
	return dx * dx + dy * dy;
}

template<typename RealType>
RealType AxisBox2<RealType>::Distance(const Vector2<RealType>& Point) const
{
	return GS::Sqrt(DistanceSquared(Point));
}

template<typename RealType>
RealType AxisBox2<RealType>::SignedDistance(const Vector2<RealType>& Point) const
{
	if (Contains(Point) == false)
	{
		return GS::Sqrt(DistanceSquared(Point));
	}
	else
	{
		RealType dx = GS::Min(GS::Abs(Point.X - Min.X), GS::Abs(Point.X - Max.X));
		RealType dy = GS::Min(GS::Abs(Point.Y - Min.Y), GS::Abs(Point.Y - Max.Y));
		return -GS::Min(dx, dy);
	}
}



template<typename RealType>
bool AxisBox2<RealType>::Intersects(const AxisBox2<RealType>& OtherBox) const
{
	return !((OtherBox.Max.X <= Min.X) || (OtherBox.Min.X >= Max.X)
		|| (OtherBox.Max.Y <= Min.Y) || (OtherBox.Min.Y >= Max.Y));
}



template<typename RealType>
RealType AxisBox2<RealType>::DistanceSquared(const AxisBox2<RealType>& OtherBox) const
{
	// compute lensqr( max(0, abs(center1-center2) - (extent1+extent2)) )
	RealType delta_x = GS::Abs((OtherBox.Min.X + OtherBox.Max.X) - (Min.X + Max.X))
		- ((Max.X - Min.X) + (OtherBox.Max.X - OtherBox.Min.X));
	if (delta_x < 0)
		delta_x = 0;
	RealType delta_y = GS::Abs((OtherBox.Min.Y + OtherBox.Max.Y) - (Min.Y + Max.Y))
		- ((Max.Y - Min.Y) + (OtherBox.Max.Y - OtherBox.Min.Y));
	if (delta_y < 0)
		delta_y = 0;
	return (RealType)0.25 * (delta_x * delta_x + delta_y * delta_y);
}




template<typename RealType>
void AxisBox2<RealType>::Contain(const Vector2<RealType>& Point)
{
	Min.X = GS::Min(Min.X, Point.X);
	Min.Y = GS::Min(Min.Y, Point.Y);
	Max.X = GS::Max(Max.X, Point.X);
	Max.Y = GS::Max(Max.Y, Point.Y);
}

template<typename RealType>
void AxisBox2<RealType>::Contain(const AxisBox2<RealType>& OtherBox)
{
	Min.X = GS::Min(Min.X, OtherBox.Min.X);
	Min.Y = GS::Min(Min.Y, OtherBox.Min.Y);
	Max.X = GS::Max(Max.X, OtherBox.Max.X);
	Max.Y = GS::Max(Max.Y, OtherBox.Max.Y);
}


template<typename RealType>
void AxisBox2<RealType>::Translate(const Vector2<RealType>& Translation)
{
	Min += Translation;
	Max += Translation;
}

template<typename RealType>
AxisBox2<RealType> AxisBox2<RealType>::Translated(const Vector2<RealType>& Translation) const
{
	return AxisBox2<RealType>(Min + Translation, Max + Translation);
}

template<typename RealType>
void AxisBox2<RealType>::Expand(const Vector2<RealType>& AxisOffsets)
{
	Min -= AxisOffsets;
	Max += AxisOffsets;
	if ( Min.X > Max.X ) { Min.X = Max.X = (Min.X+Max.X)*(RealType)0.5; }
	if ( Min.Y > Max.Y ) { Min.Y = Max.Y = (Min.Y+Max.Y)*(RealType)0.5; }
}


} // end namespace GS
