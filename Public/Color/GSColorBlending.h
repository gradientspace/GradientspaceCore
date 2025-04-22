// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"

namespace GS
{

	//! Simple additive blend
	//! resulting color is [ Lerp(TargetColor,AddColor,AddAlpha), TargetColor.A+AddAlpha ]
	GRADIENTSPACECORE_API
	void CombineColors4f_LerpAdd(
		const float* TargetColor4,
		const float* AddColor3,
		float AddAlpha,
		float* OutputColor4,
		float ClampMin = 0.0f, float ClampMax = 1.0f
	);

	/**
	 * "A Over B" alpha compositing operation - see https://en.wikipedia.org/wiki/Alpha_compositing
	 * AddColor is A, ExistingColor is B
	 * AddAlpha is an additional alpha multiplier on AddColor.a, provided separately as it's convenient for things like brush stamps w/ falloff, etc
	 * This is the not-premultiplied-alpha version of the operator
	 * (should make sure this is correct...)
	 */
	GRADIENTSPACECORE_API
	void CombineColors4f_OverBlend(
		const float* ExistingColor4,
		const float* AddColor4,
		float AddAlpha,
		float* OutputColor4,
		float ClampMin = 0.0f, float ClampMax = 1.0f
	);

	/**
	 * Lerp each channel separately using ChannelAlphas. This can be used to (eg) do masking, where only some
	 * chanels of NewColor replace ExistingColor
	 */
	GRADIENTSPACECORE_API
	void CombineColors4f_ChannelLerp(
		const float* ExistingColor4,
		const float* NewColor4,
		const float* ChannelAlphas4,
		float* OutputColor4,
		float ClampMin = 0.0f, float ClampMax = 1.0f
	);


}