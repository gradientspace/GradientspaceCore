// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSVector3.h"
#include "Math/GSVector4.h"
#include "Color/GSColor3b.h"
#include "Color/GSIntColor4.h"
#include <algorithm>

namespace GS
{

GRADIENTSPACECORE_API
Vector3f SRGBToLinear(const Color3b& Colorb);

GRADIENTSPACECORE_API
Vector4f SRGBToLinear(const Color4b& Colorb);


GRADIENTSPACECORE_API
Color3b LinearToSRGB(const Vector3f& Colorf);



//! returns "exact" conversion to SRGB, in float value. Default scales to [0,255]
GRADIENTSPACECORE_API
Vector3f LinearToSRGBfExact(const Vector3f& Colorf, float Scale = 255.0f);

//! returns "exact" conversion to SRGB, more expensive than version above (uses math instead of tables)
GRADIENTSPACECORE_API
Color3b LinearToSRGBExact(const Vector3f& Colorf);

//! returns "exact" conversion to SRGB, more expensive than version above (uses math instead of tables)
GRADIENTSPACECORE_API
Vector3f SRGBToLinearExact(const Color3b& Colorb);



GRADIENTSPACECORE_API
Vector3f RGBtoHSV(const Vector3f& RBGColor);

GRADIENTSPACECORE_API
Vector3f HSVtoRGB(const Vector3f& HSVColor);



} // end namespace GS
