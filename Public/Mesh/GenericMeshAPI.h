// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSVector4.h"
#include "Math/GSIndex3.h"
#include "Math/GSAxisBox3.h"

namespace GS
{

/**
 * IMeshBuilder is an API that can be used for triangle mesh construction.
 * This can be used to (eg) allow GS code to construct a FDynamicMesh3, without
 * having direct visibility of that type.
 */
class /*GRADIENTSPACECORE_API*/ IMeshBuilder
{
public:
	virtual ~IMeshBuilder() {}

	virtual void ResetMesh() = 0;

	virtual int AppendVertex(const Vector3d& Position) = 0;
	virtual int GetVertexCount() const = 0;

	virtual int AllocateGroupID() = 0;
	virtual int AppendTriangle(const Index3i& Triangle, int GroupID) = 0;
	virtual int GetTriangleCount() const = 0;

	virtual void SetMaterialID(int TriangleID, int MaterialID) = 0;

	virtual int AppendColor(const Vector4f& Color, bool bIsLinearColor) = 0;
	virtual void SetTriangleColors(int TriangleID, const Index3i& TriColorIndices) = 0;

	virtual int AppendNormal(const Vector3f& Normal) = 0;
	virtual void SetTriangleNormals(int TriangleID, const Index3i& TriNormalIndices) = 0;

	virtual int AppendUV(const Vector2f& UV) = 0;
	virtual void SetTriangleUVs(int TriangleID, const Index3i& TriUVIndices) = 0;
};

/**
 * IMeshBuilderFactory constructs instances of IMeshBuilder. Code that allocates
 * an IMeshBuilder is responsible for destroying it. In this kind of usage context,
 * the IMeshBuilder would generally "own" the underlying mesh and be responsible
 * for freeing that memory as well.
 * 
 * (todo: use unique or shared ptr?)
 */
class /*GRADIENTSPACECORE_API*/ IMeshBuilderFactory
{
public:
	virtual ~IMeshBuilderFactory() {}

	virtual IMeshBuilder* Allocate() = 0;
};

/**
 * IMeshCollector is an API that can be used to accumulate meshes.
 * Generally the premise is that something inside GS libraries wants to
 * append smaller meshes it has generated in IMeshBuilder instances (eg like
 * when parallel-meshing parts of a ModelGrid and then doing a final accumulation).
 * 
 * The caller will need to handle getting the underlying mesh out of the IMeshBuilder.
 * Generally the usage context is that compatible implementations of IMeshBuilder 
 * and IMeshCollector are used together (eg both backed by FDynamicMesh3).
 * This is not a fully-generic mesh API (yet)
 */
class /*GRADIENTSPACECORE_API*/ IMeshCollector
{
public:
	virtual ~IMeshCollector() {}

	virtual void AppendMesh(const IMeshBuilder* Builder) = 0;
	virtual AxisBox3d GetBounds() const = 0;
};



}
