// Copyright Gradientspace Corp. All Rights Reserved.
#include "Intersection/GSRayBoxIntersection.h"
#include "Math/GSMath.h"

using namespace GS;


// this code is based off an article here:
// https://tavianator.com/2022/ray_box_boundary.html
// should do some testing w/ ray directions that contain zeros, as it will result in nan/inf stuff,
// which is supposed to be handled correctly...
static bool fast_ray_box(const Ray3d& Ray, const AxisBox3d& Box, double& RayHitParam)
{
	const double ray_dir_inv[3] = { 1.0 / Ray.Direction.X, 1.0 / Ray.Direction.Y, 1.0 / Ray.Direction.Z };
	const double* box_corners[2] = { Box.Min.XYZ, Box.Max.XYZ };
	//double box_corners[2][3] = { {Box.Min.X,Box.Min.Y,Box.Min.Z}, {Box.Max.X,Box.Max.Y,Box.Max.Z} };

	double tmin = 0.0, tmax = (double)Mathf::SafeMaxValue();

	for (int d = 0; d < 3; ++d) {
		bool sign = std::signbit(ray_dir_inv[d]);
		double bmin = box_corners[sign][d];
		double bmax = box_corners[!sign][d];

		double dmin = (bmin - Ray.Origin[d]) * ray_dir_inv[d];
		double dmax = (bmax - Ray.Origin[d]) * ray_dir_inv[d];

		// could keep track of which face here...

		tmin = GS::Max(dmin, tmin);
		tmax = GS::Min(dmax, tmax);
	}

	// interval is [min,max]
	// min will be negative if point is inside box, then max is first-hit

	if (tmin >= tmax) return false;

	RayHitParam = (tmin >= 0) ? tmin : tmax;
	return true;
}




double GS::TestRayBoxIntersection(const Ray3d& Ray, const GS::AxisBox3d& Box)
{
	double RayHitParam = Mathd::SafeMaxValue();
	bool bHit = fast_ray_box(Ray, Box, RayHitParam);
	if (bHit)
		return RayHitParam;
	return Mathd::SafeMaxValue();
}



bool GS::ComputeRayBoxIntersection(const Ray3d& Ray, const GS::AxisBox3d& Box,
	double& RayParameterOut, Vector3d& HitPositionOut, Vector3d& CellFaceNormalOut)
{
	double RayT = TestRayBoxIntersection(Ray, Box);
	if (RayT == Mathd::SafeMaxValue())
		return false;

	RayParameterOut = RayT;
	HitPositionOut = Ray.PointAt(RayT);
	CellFaceNormalOut = Vector3d::UnitZ();

	// calculate normal. This is dumb and should come from ray/box intersection code.
	Vector3d RayHitPos = Ray.PointAt(RayParameterOut);
	Vector3d BoxCenter = Box.Center();
	Vector3d Extents = Box.Extents();
	double MaxDist = Extents.Dot(Extents);
	for (int j = 0; j < 6; ++j)
	{
		Vector3d FaceCenter(BoxCenter), FaceNormal(0, 0, 0);
		int Axis = j / 2;
		double Dir = (j == 0 || j == 2 || j == 4) ? 1.0 : -1.0;
		FaceCenter[Axis] += Dir * Extents[Axis];
		FaceNormal[Axis] += Dir;
		double HitPosDist = GS::Abs((RayHitPos - FaceCenter).Dot(FaceNormal));
		if (HitPosDist < MaxDist)
		{
			MaxDist = HitPosDist;
			CellFaceNormalOut = FaceNormal;
		}
	}

	return true;
}
