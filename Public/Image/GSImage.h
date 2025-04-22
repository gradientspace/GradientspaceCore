// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Math/GSIntVector2.h"
#include "Math/GSVector4.h"

namespace GS
{

template<typename RealPixelType, typename RealChannelType, int NumChannels>
class TRealImageBuffer
{
public:
	Vector2i Dimensions;
	unsafe_vector<RealPixelType> Pixels;

	// todo: need to be in .cpp file
	~TRealImageBuffer();
	
	void InitializeBuffer(int Width, int Height);

	template<typename GetColorFuncT>  /*Vector4f GetColorFuncT(int64 LinearIndex)*/
	void Initialize(
		int Width, int Height,
		GetColorFuncT GetColorFunc)
	{
		InitializeBuffer(Width, Height);
		int64_t TotalPixels = (int64_t)Width * (int64_t)Height;
		for (int k = 0; k < TotalPixels; ++k)
			Pixels[k] = GetColorFunc(k);
	}

	int Width() const { return Dimensions.X; }
	int Height() const { return Dimensions.Y; }
	int64_t NumPixels() const { return Pixels.size(); }

	int64_t ToLinearIndex(Vector2i PixelCoords) const {
		return (int64_t)(PixelCoords.Y * Dimensions.X) + (int64_t)PixelCoords.X;
	}
	Vector2i ToPixelCoords(int64_t LinearIndex) const {
		int64_t y = (LinearIndex / Dimensions.X);
		int64_t x = LinearIndex - (y * Dimensions.X);
		return Vector2i((int)x, (int)y);
	}

	const RealPixelType& GetPixel(int64_t LinearIndex) const {
		return Pixels[LinearIndex];
	}
	const RealPixelType& GetPixel(const Vector2i& PixelCoords) const {
		int64_t LinearIndex = (int64_t)(PixelCoords.Y * Dimensions.X) + (int64_t)PixelCoords.X;
		return Pixels[LinearIndex];
	}


	void SetPixel(int64_t LinearIndex, const RealPixelType& NewLinearColor) {
		Pixels[LinearIndex] = NewLinearColor;
	}
	void SetPixel(const Vector2i& PixelCoords, const RealPixelType& NewLinearColor) {
		int64_t LinearIndex = (int64_t)(PixelCoords.Y * Dimensions.X) + (int64_t)PixelCoords.X;
		Pixels[LinearIndex] = NewLinearColor;
	}
};

// standard image types
typedef TRealImageBuffer<Vector4f, float, 4> Image4f;
typedef TRealImageBuffer<Vector3f, float, 3> Image3f;
typedef TRealImageBuffer<float, float, 1> Image1f;

// explicit instantiation
extern template class TRealImageBuffer<Vector4f, float, 4>;
extern template class TRealImageBuffer<Vector3f, float, 3>;
extern template class TRealImageBuffer<float, float, 1>;













}