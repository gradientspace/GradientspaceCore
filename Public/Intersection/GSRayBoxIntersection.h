// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "Math/GSRay3.h"
#include "Math/GSAxisBox3.h"

namespace GS
{

//! returns ray parameter on intersection, or Mathd::SafeMaxValue() on no-hit
GRADIENTSPACECORE_API
double TestRayBoxIntersection(const Ray3d& Ray, const GS::AxisBox3d& Box);


GRADIENTSPACECORE_API
bool ComputeRayBoxIntersection(const Ray3d& Ray, const GS::AxisBox3d& Box, double& RayParameterOut, Vector3d& HitPositionOut, Vector3d& CellFaceNormalOut);


}
