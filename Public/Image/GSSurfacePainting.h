// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/FunctionRef.h"
#include "Core/unsafe_vector.h"
#include "Math/GSIntVector2.h"
#include "Math/GSVector4.h"
#include "Math/GSFrame3.h"
#include "Image/GSImage.h"
#include "Sampling/SurfaceTexelSampling.h"

#include <unordered_map>

namespace GS
{

class GRADIENTSPACECORE_API SurfacePaintStroke
{
public:
	struct StrokeStamp
	{
		Frame3d Orientation;
		double Radius;
	};
	unsafe_vector<StrokeStamp> Stamps;

	std::unordered_map<int, int> StrokePointsMap;
	struct StrokePoint
	{
		int Index;
		Vector4f OriginalColor;
		Vector4f StrokeAccumColor;
	};
	unsafe_vector<StrokePoint> OrderedStrokePoints;

	Vector4f ChannelMask = Vector4f::One();

public:
	virtual ~SurfacePaintStroke();

	virtual void BeginStroke();

	virtual void AppendStrokeStamp(
		Frame3d StampFrame, double Radius, 
		double Falloff, double StampPower, Vector4f StrokeColor, 
		double StrokeAlpha,
		const GS::SurfaceTexelSampling& SurfaceSampling,
		const_buffer_view<int> StampTexelPoints,
		GS::SurfaceTexelColorCache& UseColorCache,
		FunctionRef<void(int TexelPointID, const Vector2i& PixelPos, const Vector4f& NewColor)> WritePixelFunc);

	virtual void AppendStrokeStamp(
		Frame3d StampFrame, double Radius, 
		FunctionRef<Vector4f(Vector3d SamplePos, const Frame3d& StampFrame, double StampRadius)> StampFunc, 
		double StrokeAlpha,
		const GS::SurfaceTexelSampling& SurfaceSampling,
		const_buffer_view<int> StampTexelPoints,
		GS::SurfaceTexelColorCache& UseColorCache,
		FunctionRef<void(int TexelPointID, const Vector2i& PixelPos, const Vector4f& NewColor)> WritePixelFunc);


	size_t GetNumStrokePoints() const { return OrderedStrokePoints.size(); }

	void EnumerateStrokePoints(FunctionRef<void(const StrokePoint&)> PointFunc) const;

	virtual void EndStroke();
};


}