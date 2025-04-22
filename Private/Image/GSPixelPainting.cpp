// Copyright Gradientspace Corp. All Rights Reserved.
#include "Image/GSPixelPainting.h"
#include "Color/GSColorBlending.h"

using namespace GS;


PixelPaintStroke::~PixelPaintStroke()
{
	Stamps.clear(true);
	OrderedModifiedPixels.clear(true);
	PixelIndexToModifiedPixelMap = std::unordered_map<int, int>();
}

void PixelPaintStroke::BeginStroke()
{
	Stamps.clear(false);
	OrderedModifiedPixels.clear(false);
	PixelIndexToModifiedPixelMap.clear();
}


void PixelPaintStroke::AppendStrokeStamp(
	Vector2i PixelCoords, float StampPower,
	Vector4f StrokeColor, float StrokeAlpha,
	GS::Image4f& ImageBuffer,
	FunctionRef<void(const Vector2i& PixelPos, const Vector4f& NewColor)> WritePixelFunc)
{
	Stamps.add( StrokeStamp{ PixelCoords } );
	int PixelIndex = (int)ImageBuffer.ToLinearIndex(PixelCoords);

	// compute stamp falloff and brush power
	float BrushAlpha = StampPower;

	std::unordered_map<int,int>::const_iterator found_itr = PixelIndexToModifiedPixelMap.find(PixelIndex);
	ModifiedPixel* UpdatePoint = nullptr;
	if (found_itr == PixelIndexToModifiedPixelMap.end())
	{
		// save initial state
		Vector4f InitialColor = ImageBuffer.GetPixel(PixelIndex);
		ModifiedPixel NewPoint{ PixelIndex, InitialColor, Vector4f(StrokeColor.X,StrokeColor.Y,StrokeColor.Z,StrokeColor.W*BrushAlpha) };
		int NewIndex = (int)OrderedModifiedPixels.push_back(NewPoint);
		UpdatePoint = &OrderedModifiedPixels[NewIndex];
		PixelIndexToModifiedPixelMap.insert({ (int)PixelIndex, NewIndex });
	}
	else {
		UpdatePoint = &OrderedModifiedPixels[found_itr->second];
		Vector4f NewStrokeAccumColor;
		GS::CombineColors4f_LerpAdd(UpdatePoint->StrokeAccumColor.AsPointer(), StrokeColor.AsPointer(), StrokeColor.W * BrushAlpha, NewStrokeAccumColor.AsPointer());
		UpdatePoint->StrokeAccumColor = NewStrokeAccumColor;
	}
	Vector4f NewBlendColor;
	GS::CombineColors4f_OverBlend(
		UpdatePoint->OriginalColor.AsPointer(), UpdatePoint->StrokeAccumColor.AsPointer(), StrokeAlpha, NewBlendColor.AsPointer());

	GS::CombineColors4f_ChannelLerp(
		UpdatePoint->OriginalColor.AsPointer(), NewBlendColor.AsPointer(), ChannelFilter.AsPointer(), NewBlendColor.AsPointer());

	ImageBuffer.SetPixel(PixelIndex, (GS::Vector4f)NewBlendColor);
	WritePixelFunc(PixelCoords, NewBlendColor);
}

void PixelPaintStroke::EnumerateModifiedPixels(FunctionRef<void(const PixelPaintStroke::ModifiedPixel&)> PointFunc) const
{
	int64_t N = OrderedModifiedPixels.size();
	for (int64_t k = 0; k < N; ++k) {
		const PixelPaintStroke::ModifiedPixel& PixelInfo = OrderedModifiedPixels[k];
		PointFunc(PixelInfo);
	};
}


void PixelPaintStroke::EndStroke()
{
	Stamps.clear(false);
	OrderedModifiedPixels.clear(false);
	PixelIndexToModifiedPixelMap.clear();
}