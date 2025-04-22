// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSVector2.h"
#include "Math/GSVector3.h"

namespace GS
{

GRADIENTSPACECORE_API
double PerlinNoise2D(const Vector2d& Pos);

GRADIENTSPACECORE_API
double PerlinNoise3D(const Vector3d& Pos);



}
