// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Core/FunctionRef.h"
#include "Math/GSIntVector2.h"
#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSVector4.h"
#include "Math/GSIntAxisBox2.h"
#include "Math/GSAxisBox3.h"
#include "Mesh/MeshView2.h"

namespace GS
{

struct GRADIENTSPACECORE_API TexelPoint3d
{
	Vector2i PixelPos;
	Vector3d SurfacePos;
	int TriangleID = -1;
	int UVIsland = -1;
};

class GRADIENTSPACECORE_API SurfaceTexelSampling
{
public:
	unsafe_vector<TexelPoint3d> TexelSamples;
	AxisBox3d SampleBounds;

public:
	~SurfaceTexelSampling();

	void Build(ConstMeshView2d UVMesh, int ImageWidth, int ImageHeight,
		FunctionRef<Vector3d(int,Vector3d)> ComputeTriBaryPoint3DFUnc
	);

	int NumSamples() const { return (int)TexelSamples.size(); }

	TexelPoint3d& operator[](int Index) { return TexelSamples[Index]; }
	const TexelPoint3d& operator[](int Index) const { return TexelSamples[Index]; }
};






class GRADIENTSPACECORE_API SurfaceTexelColorCache
{
public:
	struct TexelColor
	{
		Vector4f LinearColor;
	};
	unsafe_vector<TexelColor> Colors;

	~SurfaceTexelColorCache();

	void Initialize(
		int NumTexelPoints,
		FunctionRef<Vector4f(int TexelPointID)> GetColorFunc);


	const GS::Vector4f& GetColor(int PointID) const {
		return Colors[PointID].LinearColor;
	}


	void SetColor(int PointID, const GS::Vector4f& NewLinearColor) {
		Colors[PointID].LinearColor = NewLinearColor;
	}
};



}