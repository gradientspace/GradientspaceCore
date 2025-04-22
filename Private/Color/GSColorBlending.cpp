// Copyright Gradientspace Corp. All Rights Reserved.
#include "Color/GSColorBlending.h"

#include "Math/GSMath.h"

using namespace GS;


void GS::CombineColors4f_LerpAdd(
	const float* TargetColor4,
	const float* AddColor3,
	float AddAlpha,
	float* OutputColor4,
	float ClampMin, float ClampMax)
{
	float OneMinusAlpha = 1.0f - AddAlpha;
	OutputColor4[0] = GS::Clamp(OneMinusAlpha * TargetColor4[0] + AddAlpha * AddColor3[0], ClampMin, ClampMax);
	OutputColor4[1] = GS::Clamp(OneMinusAlpha * TargetColor4[1] + AddAlpha * AddColor3[1], ClampMin, ClampMax);
	OutputColor4[2] = GS::Clamp(OneMinusAlpha * TargetColor4[2] + AddAlpha * AddColor3[2], ClampMin, ClampMax);
	OutputColor4[3] = GS::Clamp(OneMinusAlpha * TargetColor4[3] + AddAlpha, ClampMin, ClampMax);
}


void GS::CombineColors4f_OverBlend(
	const float* ExistingColor4,
	const float* AddColor4,
	float AddAlpha,
	float* OutputColor4,
	float ClampMin, float ClampMax
)
{
	float CombinedAddAlpha = AddColor4[3] * AddAlpha;
	float OneMinusCombinedAlpha = 1.0f-CombinedAddAlpha;
	for (int k = 0; k < 3; ++k)
	{
		OutputColor4[k] = (AddColor4[k] * CombinedAddAlpha) + (ExistingColor4[k] * ExistingColor4[3] * OneMinusCombinedAlpha);
	}

	OutputColor4[3] = CombinedAddAlpha + ExistingColor4[3] * OneMinusCombinedAlpha;
	if (OutputColor4[3] > GS::Mathf::ZeroTolerance())
	{
		OutputColor4[0] /= OutputColor4[3];
		OutputColor4[1] /= OutputColor4[3];
		OutputColor4[2] /= OutputColor4[3];
	}

	OutputColor4[0] = GS::Clamp(OutputColor4[0], ClampMin, ClampMax);
	OutputColor4[1] = GS::Clamp(OutputColor4[1], ClampMin, ClampMax);
	OutputColor4[2] = GS::Clamp(OutputColor4[2], ClampMin, ClampMax);
	OutputColor4[3] = GS::Clamp(OutputColor4[3], ClampMin, ClampMax);
}



void GS::CombineColors4f_ChannelLerp(
	const float* ExistingColor4,
	const float* NewColor4,
	const float* ChannelAlphas4,
	float* OutputColor4,
	float ClampMin, float ClampMax)
{
	for (int j = 0; j < 4; ++j) {
		OutputColor4[j] = (1.0f - ChannelAlphas4[j]) * ExistingColor4[j] + (ChannelAlphas4[j]) * NewColor4[j];
		OutputColor4[j] = GS::Clamp(OutputColor4[j], ClampMin, ClampMax);
	}
}