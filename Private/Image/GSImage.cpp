// Copyright Gradientspace Corp. All Rights Reserved.
#include "Image/GSImage.h"

using namespace GS;

template<typename RealPixelType, typename RealChannelType, int NumChannels>
GS::TRealImageBuffer<RealPixelType, RealChannelType, NumChannels>::~TRealImageBuffer()
{
	Pixels.clear(true);
}

template<typename RealPixelType, typename RealChannelType, int NumChannels>
void GS::TRealImageBuffer<RealPixelType, RealChannelType, NumChannels>::InitializeBuffer(int Width, int Height)
{
	Dimensions = Vector2i(Width, Height);
	Pixels.resize((int64_t)Width * (int64_t)Height);
}

// explicit instantiation
template class GRADIENTSPACECORE_API GS::TRealImageBuffer<Vector4f, float, 4>;
template class GRADIENTSPACECORE_API GS::TRealImageBuffer<Vector3f, float, 3>;
template class GRADIENTSPACECORE_API GS::TRealImageBuffer<float, float, 1>;

