// Copyright Gradientspace Corp. All Rights Reserved.
#include "Sampling/ImageSampling.h"

using namespace GS;

template<typename ValueType, int NumChannels>
bool GS::BilinearSampleCenteredGrid2(
	const ValueType* Buffer,
	int Width, int Height,
	Vector2d PixelCoords,
	ValueType* ValueOut)
{
	if (PixelCoords.X < 0 || PixelCoords.X > Width || PixelCoords.Y < 0 || PixelCoords.Y > Height)
		return false;

	// shift by 0.5 to account for grid values being considered to be "at center" of each grid cell
	double shifted_x = PixelCoords.X - 0.5;
	double shifted_y = PixelCoords.Y - 0.5;
	// convert to integral pixel coords
	double pixel_x = GS::Floor(shifted_x);
	double pixel_y = GS::Floor(shifted_y);
	int x0 = (int)(pixel_x);
	int y0 = (int)(GS::Floor(shifted_y));

	// convert double coords to [0,1] range, in shifted cells
	double Ax = shifted_x - pixel_x;
	double Ay = shifted_y - pixel_y;

	// clamp min/max pixel coords to grid
	int x1 = x0 + 1, y1 = y0 + 1;
	x0 = GS::Max(0, x0);
	y0 = GS::Max(0, y0);
	x1 = GS::Min(x1, Width - 1);
	y1 = GS::Min(y1, Height - 1);

	// grab pixel pointers. At edges/corners some of these pixels will be the same, due to clamping
	const int RowStride = (Width * NumChannels);
	const ValueType* V00 = &Buffer[y0 * RowStride + x0 * NumChannels];
	const ValueType* V10 = &Buffer[y0 * RowStride + x1 * NumChannels];
	const ValueType* V01 = &Buffer[y1 * RowStride + x0 * NumChannels];
	const ValueType* V11 = &Buffer[y1 * RowStride + x1 * NumChannels];

	// bilerp!
	double OneMinusAx = 1.0 - Ax;
	double OneMinusAy = 1.0 - Ay;
	for (int j = 0; j < NumChannels; ++j)
	{
		// this can be expanded out and regrouped more efficiently...
		double c = ((double)V00[j] * OneMinusAx * OneMinusAy)
			+ ((double)V01[j] * OneMinusAx * Ay)
			+ ((double)V10[j] * Ax * OneMinusAy)
			+ ((double)V11[j] * Ax * Ay);

		ValueOut[j] = (ValueType)c;
	}
	return true;
}

template bool GS::BilinearSampleCenteredGrid2<float, 1>(const float* buffer, int Width, int Height, Vector2d PixelCoords, float* ValueOut);
template bool GS::BilinearSampleCenteredGrid2<float, 3>(const float* buffer, int Width, int Height, Vector2d PixelCoords, float* ValueOut);
template bool GS::BilinearSampleCenteredGrid2<float, 4>(const float* buffer, int Width, int Height, Vector2d PixelCoords, float* ValueOut);
template bool GS::BilinearSampleCenteredGrid2<uint8_t, 1>(const uint8_t* buffer, int Width, int Height, Vector2d PixelCoords, uint8_t* ValueOut);
template bool GS::BilinearSampleCenteredGrid2<uint8_t, 3>(const uint8_t* buffer, int Width, int Height, Vector2d PixelCoords, uint8_t* ValueOut);
template bool GS::BilinearSampleCenteredGrid2<uint8_t, 4>(const uint8_t* buffer, int Width, int Height, Vector2d PixelCoords, uint8_t* ValueOut);
