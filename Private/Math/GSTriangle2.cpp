// Copyright Gradientspace Corp. All Rights Reserved.
#include "Math/GSTriangle2.h"

using namespace GS;


Vector3d GS::GetTriBaryCoords2d(const Vector2d& V0, const Vector2d& V1, const Vector2d& V2, const Vector2d& Point)
{
	// this is 3D formula...maybe there are 2D optimizations?

	Vector2d kV02 = V0 - V2;
	Vector2d kV12 = V1 - V2;
	Vector2d kPV2 = Point - V2;
	double fM00 = kV02.Dot(kV02);
	double fM01 = kV02.Dot(kV12);
	double fM11 = kV12.Dot(kV12);
	double fR0 = kV02.Dot(kPV2);
	double fR1 = kV12.Dot(kPV2);
	double fDet = fM00 * fM11 - fM01 * fM01;
	
	// for a degenerate triangle, fDet will be zero and this divide will be inf
	// if we add a tiny epsilon, then fBary1 and fBary2 will be zero and fBary3 will be 1
	//double fInvDet = 1.0 / fDet;
	double fInvDet = 1.0 / (fDet + Mathd::Epsilon());

	double fBary1 = (fM11 * fR0 - fM01 * fR1) * fInvDet;
	double fBary2 = (fM00 * fR1 - fM01 * fR0) * fInvDet;
	double fBary3 = 1.0 - fBary1 - fBary2;
	return Vector3d(fBary1, fBary2, fBary3);
}


bool GS::IsPointInsideTriangle(const Vector2d& A, const Vector2d& B, const Vector2d& C, const Vector2d& Point)
{
	// point needs to be on the same side of all 3 triangle edges
	// note that this test is invalid if A == B
	double SignAB = Fast2DPointSideTest(A, B, Point);
	double SignBC = Fast2DPointSideTest(B, C, Point);
	double SignCA = Fast2DPointSideTest(C, A, Point);
	// if we don't use >= here, then point right on the line between two triangles will not be
	// in either of them. But if we use >=, then degenerate triangles will pass...
	return (SignAB * SignBC > 0) && (SignBC * SignCA > 0) && (SignCA * SignAB > 0);
}

//
//bool GS::IsPointInsideOrOnTriangle(const Vector2d& A, const Vector2d& B, const Vector2d& C, const Vector2d& Point)
//{
//	// this breaks if (LineB-LineA0 == 0
//	// need to catch that case and ignore it...in which case test only passes
//	// if tri becomes a line and Point is on that line, or a point and C is also that point
//	//Fast2DPointSideTest(A,B,Point) = DotPerp((LineB - LineA), (TestPoint - LineA));
//
//	// point needs to be on the same side of all 3 triangle edges
//	// note that this test is invalid if A == B
//	double SignAB = Fast2DPointSideTest(A, B, Point);
//	double SignBC = Fast2DPointSideTest(B, C, Point);
//	double SignCA = Fast2DPointSideTest(C, A, Point);
//	// if we don't use >= here, then point right on the line between two triangles will not be
//	// in either of them. But if we use >=, then degenerate triangles will pass...
//	return (SignAB * SignBC > 0) && (SignBC * SignCA > 0) && (SignCA * SignAB > 0);
//}

