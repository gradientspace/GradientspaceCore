// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSVector2.h"

namespace GS
{
template<typename ValueType, int NumChannels>
bool BilinearSampleCenteredGrid2(
	const ValueType* Buffer,
	int Width, int Height,
	Vector2d PixelCoords,
	ValueType* ValueOut);
// explicit instantiations of above for known use cases
extern template GRADIENTSPACECORE_API bool BilinearSampleCenteredGrid2<float, 1>(const float* buffer, int Width, int Height, Vector2d PixelCoords, float* ValueOut);
extern template GRADIENTSPACECORE_API bool BilinearSampleCenteredGrid2<float, 3>(const float* buffer, int Width, int Height, Vector2d PixelCoords, float* ValueOut);
extern template GRADIENTSPACECORE_API bool BilinearSampleCenteredGrid2<float, 4>(const float* buffer, int Width, int Height, Vector2d PixelCoords, float* ValueOut);
extern template GRADIENTSPACECORE_API bool BilinearSampleCenteredGrid2<uint8_t, 1>(const uint8_t* buffer, int Width, int Height, Vector2d PixelCoords, uint8_t* ValueOut);
extern template GRADIENTSPACECORE_API bool BilinearSampleCenteredGrid2<uint8_t, 3>(const uint8_t* buffer, int Width, int Height, Vector2d PixelCoords, uint8_t* ValueOut);
extern template GRADIENTSPACECORE_API bool BilinearSampleCenteredGrid2<uint8_t, 4>(const uint8_t* buffer, int Width, int Height, Vector2d PixelCoords, uint8_t* ValueOut);




}