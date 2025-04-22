// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/FunctionRef.h"
#include "Core/unsafe_vector.h"
#include "Math/GSIntVector2.h"
#include "Math/GSVector4.h"
#include "Image/GSImage.h"

#include <unordered_map>

namespace GS
{

class GRADIENTSPACECORE_API PixelPaintStroke
{
public:
	struct StrokeStamp
	{
		Vector2i Location;
	};
	// sequential list of "stamps" in this stroke, in pixel coords
	unsafe_vector<StrokeStamp> Stamps;

	struct ModifiedPixel
	{
		int PixelIndex;
		Vector4f OriginalColor;
		Vector4f StrokeAccumColor;
	};
	// (unordered) list of pixels that we have written to in this stroke
	// (if we did ordered insertion we could skip the map below?)
	unsafe_vector<ModifiedPixel> OrderedModifiedPixels;

	// map linear pixel index to index of that pixel in OrderedModifiedPixels
	std::unordered_map<int, int> PixelIndexToModifiedPixelMap;

	Vector4f ChannelFilter = Vector4f::One();


public:
	virtual ~PixelPaintStroke();

	virtual void BeginStroke();

	virtual void AppendStrokeStamp(
		Vector2i PixelCoords, float StampPower,
		Vector4f StrokeColor, float StrokeAlpha,
		GS::Image4f& ImageBuffer,
		FunctionRef<void(const Vector2i& PixelPos, const Vector4f& NewColor)> WritePixelFunc);

	size_t GetNumModifiedPixels() const { return OrderedModifiedPixels.size(); }

	void EnumerateModifiedPixels(FunctionRef<void(const ModifiedPixel&)> PointFunc) const;

	virtual void EndStroke();
};


}
