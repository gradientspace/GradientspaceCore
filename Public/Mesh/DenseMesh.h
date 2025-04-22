// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"

#include "Core/dynamic_buffer.h"
#include "Core/rle_buffer.h"
#include "Core/gs_serializer.h"

#include "Mesh/MeshTypes.h"
#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSIndex3.h"
#include "Color/GSIntColor4.h"

namespace GS 
{


class GRADIENTSPACECORE_API DenseMesh
{
protected:

	dynamic_buffer<Vector3d> Positions;
	dynamic_buffer<Index3i> Triangles;

	rle_buffer<int> TriGroups;
	rle_buffer<int> TriMaterialIndexes;
	dynamic_buffer<TriVtxNormals> TriVertexNormals;
	dynamic_buffer<TriVtxUVs> TriVertexUVs;
	dynamic_buffer<TriVtxColors> TriVertexColors;

	// tangents
	// extended UVs
	// extended groups

public:
	DenseMesh();

	void Clear();
	void Resize(int VertexCount, int TriangleCount);

	inline int GetVertexCount() const;
	inline const Vector3d& GetPosition(int VertexIndex) const;
	inline void SetPosition(int VertexIndex, const Vector3d& NewPosition);

	inline int GetTriangleCount() const;
	inline const Index3i& GetTriangle(int TriIndex) const;
	inline void SetTriangle(int TriIndex, const Index3i& NewTriangle);

	inline int GetTriGroup(int TriIndex) const;
	inline void SetTriGroup(int TriIndex, int NewGroup);
	inline void SetConstantTriGroup(int NewGroup);

	inline int GetTriMaterialIndex(int TriIndex) const;
	inline void SetTriMaterialIndex(int TriIndex, int NewMaterialIndex);
	inline void SetConstantTriMaterialIndex(int NewMaterialIndex);

	inline const TriVtxNormals& GetTriVtxNormals(int TriIndex) const;
	inline void SetTriVtxNormals(int TriIndex, const TriVtxNormals& NewNormals);

	inline const TriVtxUVs& GetTriVtxUVs(int TriIndex) const;
	inline void SetTriVtxUVs(int TriIndex, const TriVtxUVs& NewUVs);

	inline const TriVtxColors& GetTriVtxColors(int TriIndex) const;
	inline void SetTriVtxColors(int TriIndex, const TriVtxColors& NewUVs);

	bool Store(GS::ISerializer& Serializer) const;
	bool Restore(GS::ISerializer& Serializer);
	constexpr const char* SerializeVersionString() const { return "DenseMesh_Version"; }
};



int DenseMesh::GetVertexCount() const
{
	return (int)Positions.size();
}

int DenseMesh::GetTriangleCount() const
{
	return (int)Triangles.size();
}

const Vector3d& DenseMesh::GetPosition(int Index) const
{
	return Positions[Index];
}

void DenseMesh::SetPosition(int VertexIndex, const Vector3d& NewPosition)
{
	Positions[VertexIndex] = NewPosition;
}

const Index3i& DenseMesh::GetTriangle(int Index) const
{
	return Triangles[Index];
}

void DenseMesh::SetTriangle(int TriIndex, const Index3i& NewTriangle)
{
	Triangles[TriIndex] = NewTriangle;
}

int DenseMesh::GetTriGroup(int TriIndex) const
{
	return TriGroups[TriIndex];
}

void DenseMesh::SetTriGroup(int TriIndex, int NewGroup)
{
	TriGroups.set_value(TriIndex, NewGroup);
}

void DenseMesh::SetConstantTriGroup(int NewGroup)
{
	TriGroups.resize_init(GetTriangleCount(), NewGroup, false);
}

int DenseMesh::GetTriMaterialIndex(int TriIndex) const
{
	return TriMaterialIndexes[TriIndex];
}

void DenseMesh::SetTriMaterialIndex(int TriIndex, int NewMaterialIndex)
{
	TriMaterialIndexes.set_value(TriIndex, NewMaterialIndex);
}

void DenseMesh::SetConstantTriMaterialIndex(int NewMaterialIndex)
{
	TriMaterialIndexes.resize_init(GetTriangleCount(), NewMaterialIndex, false);
}

const TriVtxNormals& DenseMesh::GetTriVtxNormals(int Index) const
{
	return TriVertexNormals[Index];
}

void DenseMesh::SetTriVtxNormals(int TriIndex, const TriVtxNormals& NewNormals)
{
	TriVertexNormals.set_value(TriIndex, NewNormals);
}

const TriVtxUVs& DenseMesh::GetTriVtxUVs(int Index) const
{
	return TriVertexUVs[Index];
}

void DenseMesh::SetTriVtxUVs(int TriIndex, const TriVtxUVs& NewUVs)
{
	TriVertexUVs.set_value(TriIndex, NewUVs);
}

const TriVtxColors& DenseMesh::GetTriVtxColors(int Index) const
{
	return TriVertexColors[Index];
}

void DenseMesh::SetTriVtxColors(int TriIndex, const TriVtxColors& NewColor)
{
	TriVertexColors.set_value(TriIndex, NewColor);
}


} // end namespace GS
