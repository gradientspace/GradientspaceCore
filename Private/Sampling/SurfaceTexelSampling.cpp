// Copyright Gradientspace Corp. All Rights Reserved.
#include "Sampling/SurfaceTexelSampling.h"

#include "Core/ParallelFor.h"
#include "Spatial/AxisBoxTree2.h"
#include "Mesh/MeshTypes.h"

using namespace GS;


SurfaceTexelSampling::~SurfaceTexelSampling()
{
	TexelSamples.clear(true);
}


void SurfaceTexelSampling::Build(ConstMeshView2d UVMesh, int ImageWidth, int ImageHeight,
	FunctionRef<Vector3d(int, Vector3d)> ComputeTriBaryPoint3DFunc)
{
	//gs_debug_assert(ImageWidth == ImageHeight);		// other case is untested...

	double PixelToU = (double)1.0 / (double)ImageWidth;
	double PixelToV = (double)1.0 / (double)ImageHeight;
	double MaxGutterPixelWidth = 2.0;
	double MaxGutterUVWidth = MaxGutterPixelWidth * PixelToU;

	// build 2D box tree
	AxisBoxTree2d BoxTree;
	BoxTree.Build(UVMesh.GetNumTriangleIDs(), [&](int TriangleID, GS::AxisBox2d& Box) {
		Triangle2d TriV;
		UVMesh.GetTriangle(TriangleID, TriV);
		Box = ((Triangle2d)TriV).Bounds();
		Box = (AxisBox2d)TriV.Bounds();
		return true;
	});
	//BoxTree.Validate();

	SampleBounds = AxisBox3d::Empty();

	// TODO: to support tiling / worldspace textures, we need to figure out which UV-unit-boxes
	// are occupied, and loop over them below...
	// (this could get very expensive if the tiling is very dense!!)

	// For each pixel, figure out which UV-triangle it's inside of, and then map up to 3D.
	// (No handling of multiple UV-triangles for now...)
	// Pixels in gutter-band get snapped to nearest UV-triangle
	int NumPixels = ImageWidth * ImageHeight;
	unsafe_vector<TexelPoint3d> AllPoints;
	AllPoints.initialize(NumPixels, TexelPoint3d());
	GS::ParallelFor(NumPixels, [&](int LinearIndex) {
		int yi = LinearIndex / ImageWidth;
		int xi = LinearIndex - (yi * ImageWidth);

		double u = ((double)xi + 0.5) * PixelToU;
		double v = ((double)yi + 0.5) * PixelToV;
		Vector2d PosUV(u, v);
		Vector3d PosUV3(u, v, 0);

		// TODO: need to do multi-triangle containment query, to handle overlapping texture islands.
		// But then also need to handle multiple writes to the same pixel...

		GS::DistanceQueryOptions<double> Options;
		Options.MaxDistance = 1.1 * MaxGutterUVWidth;

		DistanceResult2d QueryResult = BoxTree.PointDistanceQuery(PosUV, [&](int tid, const Vector2d& QueryPoint) {
			Triangle2d Tri;
			UVMesh.GetTriangle(tid, Tri);
			int edge; double edgeparam;
			double SignedDist = Tri.SignedDistance(QueryPoint, edge, edgeparam);		// points inside will have negative distance-sqr
			if (SignedDist > 0)
				return DistanceResult2d(tid, SignedDist * SignedDist, Tri.EdgePoint(edge, edgeparam), edge, edgeparam);
			else
				return DistanceResult2d(tid, 0, QueryPoint, edge, edgeparam);
		}, Options);
		if (QueryResult.DistanceSqr > MaxGutterUVWidth * MaxGutterUVWidth)
			return;

		int NearestTID = QueryResult.ElementID;
		if (UVMesh.IsValidTriangleID(NearestTID) == false)
			return;

		// use nearest-point on triangle
		Vector2d TriNearestPoint = QueryResult.Point;

		Triangle2d UVTri;
		UVMesh.GetTriangle(QueryResult.ElementID, UVTri);
		Vector3d BaryCoords = UVTri.GetBaryCoords(TriNearestPoint);

		Vector3d Pos3D = ComputeTriBaryPoint3DFunc(NearestTID, BaryCoords);
		if (!std::isfinite(Pos3D.X)) {
			//__debugbreak();
			BaryCoords.X = 1; BaryCoords.Y = BaryCoords.Z = 0;
			Pos3D = ComputeTriBaryPoint3DFunc(NearestTID, BaryCoords);
		}

		SampleBounds.Contain(Pos3D);

		TexelPoint3d& Pt = AllPoints[LinearIndex];
		//Pt.UVIsland = TriUVIslandIndex[NearestTID];
		Pt.PixelPos = Vector2i(xi, yi);
		Pt.TriangleID = NearestTID;
		//Pt.UVPos = Vector2d(PosUV3.X, PosUV3.Y);
		Pt.SurfacePos = Pos3D;
	});
	//}, ParallelForFlags{true,false} );

	// filter out the invalid texels
	int NumValidTexels = 0;
	for (int k = 0; k < NumPixels; ++k) {
		if (AllPoints[k].TriangleID >= 0)
			++NumValidTexels;
	}

	TexelSamples.resize(NumValidTexels); 
	int ti = 0;
	for (int k = 0; k < NumPixels; ++k) {
		if (AllPoints[k].TriangleID >= 0)
			TexelSamples[ti++] = AllPoints[k];
	}
}


SurfaceTexelColorCache::~SurfaceTexelColorCache()
{
	Colors.clear(true);
}

void SurfaceTexelColorCache::Initialize(
	int NumTexelPoints,
	GS::FunctionRef<GS::Vector4f(int TexelPointID)> GetColorFunc)
{
	Colors.resize(NumTexelPoints);
	for (int k = 0; k < NumTexelPoints; ++k) {
		Colors[k].LinearColor = GetColorFunc(k);
	}
}