// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSVector3.h"

#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
#include "BoxTypes.h"
#include "Math/Box.h"
#endif

namespace GS
{

template<typename RealType>
struct AxisBox3
{
	Vector3<RealType> Min;
	Vector3<RealType> Max;

	AxisBox3();
	AxisBox3(const Vector3<RealType>& MinCorner, const Vector3<RealType>& MaxCorner);
	AxisBox3(const Vector3<RealType>& Center, const RealType Extent);
	AxisBox3(const Vector3<RealType>& Center, const RealType ExtentX, const RealType ExtentY, const RealType ExtentZ);
	AxisBox3(const RealType MinX, const RealType MinY, const RealType MinZ,
			 const RealType MaxX, const RealType MaxY, const RealType MaxZ);

	//! Empty box is defined by min-corner being larger than max-corner
	constexpr static AxisBox3 Empty() { return AxisBox3(Vector3<RealType>(RealMath<RealType>::SafeMaxExtent()), Vector3<RealType>(-RealMath<RealType>::SafeMaxExtent()) ); }
	//! largest possible empty box, may result in overflow if box extents are used in math operations
	constexpr static AxisBox3 MaxEmpty() { return AxisBox3(Vector3<RealType>(RealMath<RealType>::MaxValue()), Vector3<RealType>(-RealMath<RealType>::MaxValue())); }
	//! zero box contains the point (0,0,0)
	constexpr static AxisBox3 Zero() { return AxisBox3(Vector3<RealType>::Zero(), Vector3<RealType>::Zero()); }

	//! return true if box contains no points, ie Empty() or similar
	bool IsValid() const;

	RealType WidthX() const { return GS::Max<RealType>(Max.X - Min.X, (RealType)0); }
	RealType DepthY() const { return GS::Max<RealType>(Max.Y - Min.Y, (RealType)0); }
	RealType HeightZ() const { return GS::Max<RealType>(Max.Z - Min.Z, (RealType)0); }

	RealType DimensionX() const { return GS::Max<RealType>(Max.X - Min.X, (RealType)0); }
	RealType DimensionY() const { return GS::Max<RealType>(Max.Y - Min.Y, (RealType)0); }
	RealType DimensionZ() const { return GS::Max<RealType>(Max.Z - Min.Z, (RealType)0); }
	RealType Dimension(int Index) const { return GS::Max<RealType>(Max[Index] - Min[Index], (RealType)0); }

	Vector3<RealType> Diagonal() const;
	Vector3<RealType> Extents() const;
	Vector3<RealType> Center() const;
	RealType Volume() const;
	RealType SurfaceArea() const;

	//! returns box corner for i=[0,7]. Order is 2D CCW X/Y from bottom-left, -Z and then +Z , ie [ (-x,-y,-z), (x,-y), (x,y), (-x,y), (-x,-y,z), ... ]
	Vector3<RealType> BoxCorner(int CornerIndex) const;
	//! returns point on face/edge/corner. For each coord value neg==min, 0==center, pos==max. So (eg) center of zmin face is (0,0,-1).
	Vector3<RealType> BoxPoint(int xi, int yi, int zi) const;
	//! returns vertices oriented around box face for FaceIndex=[0,5]
	void FaceCorners(uint32_t FaceIndex, Vector3<RealType>& A, Vector3<RealType>& B, Vector3<RealType>& C, Vector3<RealType>& D) const;
	//! returns center point of box face for FaceIndex=[0,5]. +/- X faces  = indices 0/1,   +/- Y = 2/3,  +/- Z = 4,5
	Vector3<RealType> FaceCenter(uint32_t FaceIndex) const;
	//! returns bottom point of box in Z-up coord space
	Vector3<RealType> BaseZ() const { return Vector3<RealType>( (RealType)0.5*(Min.X+Max.X), (RealType)0.5*(Min.Y+Max.Y), Min.Z); }
	//! returns top point of box in Z-up coord space
	Vector3<RealType> TopZ() const { return Vector3<RealType>( (RealType)0.5*(Min.X+Max.X), (RealType)0.5*(Min.Y+Max.Y), Max.Z); }

	//! containment test is inclusive
	bool Contains(const Vector3<RealType>& Point) const;
	bool Contains(const AxisBox3<RealType>& OtherBox) const;

	RealType DistanceSquared(const Vector3<RealType>& Point) const;
	RealType Distance(const Vector3<RealType>& Point) const;
	RealType SignedDistance(const Vector3<RealType>& Point) const;

	bool Intersects(const AxisBox3<RealType>& OtherBox) const;
	RealType DistanceSquared(const AxisBox3<RealType>& OtherBox) const;

	void Contain(const Vector3<RealType>& Point);
	void Contain(const AxisBox3<RealType>& OtherBox);
	void Translate(const Vector3<RealType>& Translation);
	AxisBox3 Translated(const Vector3<RealType>& Translation) const;
	void Expand(const Vector3<RealType>& AxisOffsets);

	// Unreal Engine casting/conversion
#ifdef GS_ENABLE_UE_TYPE_COMPATIBILITY
	explicit inline operator UE::Geometry::TAxisAlignedBox3<RealType>() const {
		return UE::Geometry::TAxisAlignedBox3<RealType>((UE::Math::TVector<RealType>)Min, (UE::Math::TVector<RealType>)Max);
	}
	explicit inline AxisBox3(const UE::Geometry::TAxisAlignedBox3<RealType>& Box)
		: Min(Box.Min), Max(Box.Max) {}
	explicit inline operator UE::Math::TBox<RealType>() const {
		if (IsValid())
			return UE::Math::TBox<RealType>( (UE::Math::TVector<RealType>)Min, (UE::Math::TVector<RealType>)Max );
		else
			return UE::Math::TBox<RealType>();
	}
	explicit inline AxisBox3(const UE::Math::TBox<RealType>& Box) {
		if (Box.IsValid) {
			Min = Vector3<RealType>(Box.Min); Max = Vector3<RealType>(Box.Max);
		} else 
			*this = Empty();
	}
#endif
};
typedef AxisBox3<float> AxisBox3f;
typedef AxisBox3<double> AxisBox3d;

template<typename RealType>
AxisBox3<RealType>::AxisBox3()
{}

template<typename RealType>
AxisBox3<RealType>::AxisBox3(const Vector3<RealType>& MinCorner, const Vector3<RealType>& MaxCorner)
	: Min(MinCorner), Max(MaxCorner)
{}

template<typename RealType>
AxisBox3<RealType>::AxisBox3(const Vector3<RealType>& Center, const RealType Extent)
	: Min(Center.X-Extent,Center.Y-Extent,Center.Z-Extent), Max(Center.X+Extent,Center.Y+Extent,Center.Z+Extent)
{}

template<typename RealType>
AxisBox3<RealType>::AxisBox3(const Vector3<RealType>& Center, const RealType ExtentX, const RealType ExtentY, const RealType ExtentZ)
	: Min(Center.X-ExtentX,Center.Y-ExtentY,Center.Z-ExtentZ), Max(Center.X+ExtentX,Center.Y+ExtentY,Center.Z+ExtentZ)
{}

template<typename RealType>
AxisBox3<RealType>::AxisBox3(const RealType MinX, const RealType MinY, const RealType MinZ,
							 const RealType MaxX, const RealType MaxY, const RealType MaxZ)
	: Min(MinX, MinY, MinZ), Max(MaxX, MaxY, MaxZ)
{}


template<typename RealType>
bool AxisBox3<RealType>::IsValid() const
{
	return Max.X >= Min.X && Max.Y >= Min.Y && Max.Z >= Min.Z;
}

template<typename RealType>
RealType AxisBox3<RealType>::Volume() const 
{ 
	return DimensionX() * DimensionY() * DimensionZ(); 
}

template<typename RealType>
RealType AxisBox3<RealType>::SurfaceArea() const 
{ 
	return (RealType)2 * (DimensionX() * (DimensionY() + DimensionZ()) + DimensionY() * DimensionZ()); 
}

template<typename RealType>
Vector3<RealType> AxisBox3<RealType>::Diagonal() const
{
	return Vector3<RealType>( 
		GS::Max(Max.X-Min.X,(RealType)0), 
		GS::Max(Max.Y-Min.Y,(RealType)0), 
		GS::Max(Max.Z-Min.Z,(RealType)0) );
}

template<typename RealType>
Vector3<RealType> AxisBox3<RealType>::Extents() const
{
	return Vector3<RealType>( 
		GS::Max( (Max.X-Min.X)*(RealType)0.5, (RealType)0), 
		GS::Max( (Max.Y-Min.Y)*(RealType)0.5, (RealType)0),
		GS::Max( (Max.Z-Min.Z)*(RealType)0.5, (RealType)0));
}


template<typename RealType>
Vector3<RealType> AxisBox3<RealType>::Center() const
{
	return Vector3<RealType>( 
		(Min.X + Max.X)*(RealType)0.5, 
		(Min.Y + Max.Y)*(RealType)0.5, 
		(Min.Z + Max.Z)*(RealType)0.5 );
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
//
// In Unreal Coordinate System it would be like this:
// 
//   4---5      +Z 
//   |\  |\      |
//   0-\-1 \     |
//    \ 7---6    o--- +X
//     \|   |     \
//      3---2      \ +Y
//
template<typename RealType>
Vector3<RealType> AxisBox3<RealType>::BoxCorner(int i) const
{
	// seems like x here might have some issue? "comparison operator has higher precedence than bitwise operator" ?
	RealType x = (((i & 1) != 0) ^ ((i & 2) != 0)) ? (Max.X) : (Min.X);
	RealType y = ((i / 2) % 2 == 0) ? (Min.Y) : (Max.Y);
	RealType z = (i < 4) ? (Min.Z) : (Max.Z);
	return Vector3<RealType>(x, y, z);
}


// Indices are compatible with GS::FaceIndexToNormal / GS::NormalToFaceIndex
template<typename RealType>
void AxisBox3<RealType>::FaceCorners(uint32_t FaceIndex, Vector3<RealType>& A, Vector3<RealType>& B, Vector3<RealType>& C, Vector3<RealType>& D) const
{
	switch (FaceIndex)
	{
	case 0:	A = BoxCorner(1); B = BoxCorner(2); C = BoxCorner(6), D = BoxCorner(5); break;
	case 1:	A = BoxCorner(0); B = BoxCorner(4); C = BoxCorner(7), D = BoxCorner(3); break;
	case 2:	A = BoxCorner(2); B = BoxCorner(3); C = BoxCorner(7), D = BoxCorner(6); break;
	case 3:	A = BoxCorner(0); B = BoxCorner(4); C = BoxCorner(5), D = BoxCorner(1); break;
	case 4:	A = BoxCorner(4); B = BoxCorner(5); C = BoxCorner(6), D = BoxCorner(7); break;
	case 5:	A = BoxCorner(0); B = BoxCorner(3); C = BoxCorner(2), D = BoxCorner(1); break;
	case 6: default: A = Min; B = Max; C = Min; D = Max; break;
	}
}

// Indices are compatible with GS::FaceIndexToNormal / GS::NormalToFaceIndex
template<typename RealType>
Vector3<RealType> AxisBox3<RealType>::FaceCenter(uint32_t FaceIndex) const
{
	switch (FaceIndex)
	{
	case 0:	return Vector3<RealType>( Max.X, (RealType)0.5*(Min.Y+Max.Y), (RealType)0.5*(Min.Z+Max.Z) );
	case 1:	return Vector3<RealType>( Min.X, (RealType)0.5*(Min.Y+Max.Y), (RealType)0.5*(Min.Z+Max.Z) );
	case 2:	return Vector3<RealType>( (RealType)0.5*(Min.X+Max.X), Max.Y, (RealType)0.5*(Min.Z+Max.Z) );
	case 3:	return Vector3<RealType>( (RealType)0.5*(Min.X+Max.X), Min.Y, (RealType)0.5*(Min.Z+Max.Z) );
	case 4:	return Vector3<RealType>( (RealType)0.5*(Min.X+Max.X), (RealType)0.5*(Min.Y+Max.Y), Max.Z);
	case 5:	return Vector3<RealType>( (RealType)0.5*(Min.X+Max.X), (RealType)0.5*(Min.Y+Max.Y), Min.Z);
	case 6:	default: return Center();
	}
}




template<typename RealType>
Vector3<RealType> AxisBox3<RealType>::BoxPoint(int xi, int yi, int zi) const
{
	RealType x = (xi < 0) ? Min.X : ((xi == 0) ? ((RealType)0.5 * (Min.X + Max.X)) : Max.X);
	RealType y = (yi < 0) ? Min.Y : ((yi == 0) ? ((RealType)0.5 * (Min.Y + Max.Y)) : Max.Y);
	RealType z = (zi < 0) ? Min.Z : ((zi == 0) ? ((RealType)0.5 * (Min.Z + Max.Z)) : Max.Z);
	return Vector3<RealType>(x, y, z);
}

template<typename RealType>
bool AxisBox3<RealType>::Contains(const Vector3<RealType>& Point) const
{
	return (Min.X <= Point.X) && (Min.Y <= Point.Y) && (Min.Z <= Point.Z)
		&& (Max.X >= Point.X) && (Max.Y >= Point.Y) && (Max.Z >= Point.Z);
}

template<typename RealType>
bool AxisBox3<RealType>::Contains(const AxisBox3<RealType>& OtherBox) const
{
	return (Min.X <= OtherBox.Min.X) && (Min.Y <= OtherBox.Min.Y) && (Min.Z <= OtherBox.Min.Z)
		&& (Max.X >= OtherBox.Max.X) && (Max.Y >= OtherBox.Max.Y) && (Max.Z >= OtherBox.Max.Z);
}



template<typename RealType>
RealType AxisBox3<RealType>::DistanceSquared(const Vector3<RealType>& Point) const
{
	RealType dx = (Point.X < Min.X) ? Min.X - Point.X : (Point.X > Max.X ? Point.X - Max.X : 0);
	RealType dy = (Point.Y < Min.Y) ? Min.Y - Point.Y : (Point.Y > Max.Y ? Point.Y - Max.Y : 0);
	RealType dz = (Point.Z < Min.Z) ? Min.Z - Point.Z : (Point.Z > Max.Z ? Point.Z - Max.Z : 0);
	return dx * dx + dy * dy + dz * dz;
}

template<typename RealType>
RealType AxisBox3<RealType>::Distance(const Vector3<RealType>& Point) const
{
	return GS::Sqrt(DistanceSquared(Point));
}

template<typename RealType>
RealType AxisBox3<RealType>::SignedDistance(const Vector3<RealType>& Point) const
{
	if (Contains(Point) == false)
	{
		return GS::Sqrt(DistanceSquared(Point));
	}
	else
	{
		RealType dx = GS::Min(GS::Abs(Point.X - Min.X), GS::Abs(Point.X - Max.X));
		RealType dy = GS::Min(GS::Abs(Point.Y - Min.Y), GS::Abs(Point.Y - Max.Y));
		RealType dz = GS::Min(GS::Abs(Point.Z - Min.Z), GS::Abs(Point.Z - Max.Z));
		return -GS::Min(dx, GS::Min(dy, dz));
	}
}



template<typename RealType>
bool AxisBox3<RealType>::Intersects(const AxisBox3<RealType>& OtherBox) const
{
	return !((OtherBox.Max.X <= Min.X) || (OtherBox.Min.X >= Max.X)
		|| (OtherBox.Max.Y <= Min.Y) || (OtherBox.Min.Y >= Max.Y)
		|| (OtherBox.Max.Z <= Min.Z) || (OtherBox.Min.Z >= Max.Z));
}



template<typename RealType>
RealType AxisBox3<RealType>::DistanceSquared(const AxisBox3<RealType>& OtherBox) const
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
	RealType delta_z = GS::Abs((OtherBox.Min.Z + OtherBox.Max.Z) - (Min.Z + Max.Z))
		- ((Max.Z - Min.Z) + (OtherBox.Max.Z - OtherBox.Min.Z));
	if (delta_z < 0)
		delta_z = 0;
	return (RealType)0.25 * (delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
}




template<typename RealType>
void AxisBox3<RealType>::Contain(const Vector3<RealType>& Point)
{
	Min.X = GS::Min(Min.X, Point.X);
	Min.Y = GS::Min(Min.Y, Point.Y);
	Min.Z = GS::Min(Min.Z, Point.Z);
	Max.X = GS::Max(Max.X, Point.X);
	Max.Y = GS::Max(Max.Y, Point.Y);
	Max.Z = GS::Max(Max.Z, Point.Z);
}

template<typename RealType>
void AxisBox3<RealType>::Contain(const AxisBox3<RealType>& OtherBox)
{
	Min.X = GS::Min(Min.X, OtherBox.Min.X);
	Min.Y = GS::Min(Min.Y, OtherBox.Min.Y);
	Min.Z = GS::Min(Min.Z, OtherBox.Min.Z);
	Max.X = GS::Max(Max.X, OtherBox.Max.X);
	Max.Y = GS::Max(Max.Y, OtherBox.Max.Y);
	Max.Z = GS::Max(Max.Z, OtherBox.Max.Z);
}


template<typename RealType>
void AxisBox3<RealType>::Translate(const Vector3<RealType>& Translation)
{
	Min += Translation;
	Max += Translation;
}

template<typename RealType>
AxisBox3<RealType> AxisBox3<RealType>::Translated(const Vector3<RealType>& Translation) const
{
	return AxisBox3<RealType>(Min + Translation, Max + Translation);
}

template<typename RealType>
void AxisBox3<RealType>::Expand(const Vector3<RealType>& AxisOffsets)
{
	Min -= AxisOffsets;
	Max += AxisOffsets;
	if ( Min.X > Max.X ) { Min.X = Max.X = (Min.X+Max.X)*(RealType)0.5; }
	if ( Min.Y > Max.Y ) { Min.Y = Max.Y = (Min.Y+Max.Y)*(RealType)0.5; }
	if ( Min.Z > Max.Z ) { Min.Z = Max.Z = (Min.Z+Max.Z)*(RealType)0.5; }
}


} // end namespace GS
