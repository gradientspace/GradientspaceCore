// Copyright Gradientspace Corp. All Rights Reserved.
#include "Image/GSSurfacePainting.h"
#include "Color/GSColorBlending.h"

using namespace GS;






static double ComputeFalloff3D(double UnitDistance, double FalloffParam)
{
	double FalloffStartRadius = GS::Clamp(1.0 - FalloffParam, 0.0, 1.0);
	if (UnitDistance <= FalloffStartRadius)
		return 1.0;

	double FalloffDistance = GS::Clamp((UnitDistance - FalloffStartRadius) / (1.0 - FalloffStartRadius), 0.0, 1.0);
	double W = (1.0 - FalloffDistance * FalloffDistance);
	return W * W * W;
}




SurfacePaintStroke::~SurfacePaintStroke()
{
	Stamps.clear(true);
	OrderedStrokePoints.clear(true);
	StrokePointsMap = std::unordered_map<int, int>();
}


void SurfacePaintStroke::BeginStroke()
{
	Stamps.clear(false);
	OrderedStrokePoints.clear(false);
	StrokePointsMap.clear();
}

void SurfacePaintStroke::AppendStrokeStamp(
	Frame3d StampFrame, double Radius, 
	double Falloff, double StampPower, Vector4f StrokeColor, 
	double StrokeAlpha,
	const GS::SurfaceTexelSampling& SurfaceSampling,
	const_buffer_view<int> StampTexelPoints,
	GS::SurfaceTexelColorCache& UseColorCache,
	FunctionRef<void(int TexelPointID, const Vector2i& PixelPos, const Vector4f& NewColor)> WritePixelFunc )
{
	AppendStrokeStamp(StampFrame, Radius,
		[&](Vector3d SamplePos, const Frame3d& StampFrame, double StampRadius)
	{
		Vector4f ResultColor(StrokeColor.X, StrokeColor.Y, StrokeColor.Z, 0.0f);
		double Dist = Distance((Vector3d)StampFrame.Origin, SamplePos);
		if (Dist < Radius) {
			double BrushUnitDist = Dist / Radius;
			float BrushAlpha = (float)(ComputeFalloff3D(BrushUnitDist, Falloff) * StampPower);
			ResultColor.W = StrokeColor.W * BrushAlpha;

			//Vector3d LocalPoint = StampFrame.ToLocalPoint(SamplePos);
			//Vector2d LocalUV(LocalPoint.X, LocalPoint.Y);
			//bool bInside = GS::Abs(LocalPoint.X) < (Radius / 4) && GS::Abs(LocalPoint.Y) < (Radius / 2);
			//if (bInside)
			//	ResultColor.W = StrokeColor.W * BrushAlpha;
		}
		else
			ResultColor.W = 0;
		return ResultColor;
	},
	StrokeAlpha, SurfaceSampling, StampTexelPoints, UseColorCache, WritePixelFunc);
}




void SurfacePaintStroke::AppendStrokeStamp(
	Frame3d StampFrame, double Radius, 
	FunctionRef<Vector4f(Vector3d SamplePos, const Frame3d& StampFrame, double StampRadius)> StampFunc,
	double StrokeAlpha,
	const GS::SurfaceTexelSampling& SurfaceSampling,
	const_buffer_view<int> StampTexelPoints,
	GS::SurfaceTexelColorCache& UseColorCache,
	FunctionRef<void(int TexelPointID, const Vector2i& PixelPos, const Vector4f& NewColor)> WritePixelFunc )
{
	Stamps.add(StrokeStamp{ StampFrame, Radius });

	for (int PointID : StampTexelPoints)
	{
		const GS::TexelPoint3d& TexelPoint = SurfaceSampling[PointID];

		Vector4f TexelColor = StampFunc(TexelPoint.SurfacePos, StampFrame, Radius);
		if (TexelColor.W <= GS::Mathf::ZeroTolerance())
			continue;

		std::unordered_map<int, int>::const_iterator found_itr = StrokePointsMap.find(PointID);
		StrokePoint* UpdatePoint = nullptr;
		if (found_itr == StrokePointsMap.end())
		{
			// save initial state
			Vector4f InitialColor = UseColorCache.GetColor(PointID);
			StrokePoint NewPoint{ PointID, InitialColor, TexelColor };
			int NewIndex = (int)OrderedStrokePoints.add(NewPoint);
			UpdatePoint = &OrderedStrokePoints[NewIndex];
			StrokePointsMap.insert({ PointID, NewIndex });
		}
		else {
			UpdatePoint = &OrderedStrokePoints[found_itr->second];
			Vector4f NewStrokeAccumColor;
			GS::CombineColors4f_LerpAdd(UpdatePoint->StrokeAccumColor.AsPointer(), TexelColor.AsPointer(), TexelColor.W, NewStrokeAccumColor.AsPointer());
			UpdatePoint->StrokeAccumColor = NewStrokeAccumColor;
		}

		Vector4f NewBlendColor;
		GS::CombineColors4f_OverBlend(
			UpdatePoint->OriginalColor.AsPointer(), UpdatePoint->StrokeAccumColor.AsPointer(), (float)StrokeAlpha, NewBlendColor.AsPointer());

		GS::CombineColors4f_ChannelLerp(
			UpdatePoint->OriginalColor.AsPointer(), NewBlendColor.AsPointer(), ChannelMask.AsPointer(), NewBlendColor.AsPointer());

		UseColorCache.SetColor(PointID, NewBlendColor);
		WritePixelFunc(PointID, TexelPoint.PixelPos, NewBlendColor);
	}
}



void SurfacePaintStroke::EnumerateStrokePoints(FunctionRef<void(const StrokePoint&)> PointFunc) const 
{
	int64_t N = OrderedStrokePoints.size();
	for (int64_t k = 0; k < N; ++k) {
		const StrokePoint& PixelInfo = OrderedStrokePoints[k];
		PointFunc(PixelInfo);
	};
}

void SurfacePaintStroke::EndStroke()
{
	Stamps.clear(false);
	OrderedStrokePoints.clear(false);
	StrokePointsMap.clear();
}
