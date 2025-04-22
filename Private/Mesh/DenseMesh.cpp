// Copyright Gradientspace Corp. All Rights Reserved.

#include "Mesh/DenseMesh.h"

using namespace GS;



DenseMesh::DenseMesh()
	: TriGroups(0), 
	  TriMaterialIndexes(0)
{
}



void DenseMesh::Clear()
{
	Positions.clear();

	Triangles.clear();
	TriGroups.clear();
	TriMaterialIndexes.clear();
	TriVertexNormals.clear();
	TriVertexUVs.clear();
	TriVertexColors.clear();
}


void DenseMesh::Resize(int VertexCount, int TriangleCount)
{
	Positions.resize(VertexCount);

	Triangles.resize(TriangleCount);
	TriGroups.resize(TriangleCount);
	TriMaterialIndexes.resize(TriangleCount);
	TriVertexNormals.resize(TriangleCount);
	TriVertexUVs.resize(TriangleCount);
	TriVertexColors.resize(TriangleCount);
}





struct DenseMeshVersions
{
	static constexpr uint32_t CurrentVersionNumber = 1;
};

struct DenseMeshHeaderV1
{
	uint32_t VertexCount = 0;
	uint32_t TriangleCount = 0;
};

bool DenseMesh::Store(GS::ISerializer& Serializer) const
{
	GS::SerializationVersion CurrentVersion(DenseMeshVersions::CurrentVersionNumber);
	bool bOK = Serializer.WriteVersion(SerializeVersionString(), CurrentVersion);

	DenseMeshHeaderV1 Header;
	Header.VertexCount = (uint32_t)Positions.size();
	Header.TriangleCount = (uint32_t)Triangles.size();
	bOK = bOK && Serializer.WriteValue<DenseMeshHeaderV1>("DenseMesh", Header);

	bOK = bOK && Positions.Store(Serializer, "Positions");
	bOK = bOK && Triangles.Store(Serializer, "Triangles");
	bOK = bOK && TriGroups.Store(Serializer, "TriGroups");
	bOK = bOK && TriMaterialIndexes.Store(Serializer, "TriMaterialIndexes");
	bOK = bOK && TriVertexNormals.Store(Serializer, "TriVertexNormals");
	bOK = bOK && TriVertexUVs.Store(Serializer, "TriVertexUVs");
	bOK = bOK && TriVertexColors.Store(Serializer, "TriVertexColors");

	return bOK;
}

bool DenseMesh::Restore(GS::ISerializer& Serializer)
{
	GS::SerializationVersion Version;
	bool bOK = Serializer.ReadVersion(SerializeVersionString(), Version);
	gs_debug_assert(Version.Version == DenseMeshVersions::CurrentVersionNumber);

	DenseMeshHeaderV1 Header;
	bOK = bOK && Serializer.ReadValue<DenseMeshHeaderV1>("DenseMesh", Header);

	bOK = bOK && Positions.Restore(Serializer, "Positions");
	bOK = bOK && Triangles.Restore(Serializer, "Triangles");
	bOK = bOK && TriGroups.Restore(Serializer, "TriGroups");
	bOK = bOK && TriMaterialIndexes.Restore(Serializer, "TriMaterialIndexes");
	bOK = bOK && TriVertexNormals.Restore(Serializer, "TriVertexNormals");
	bOK = bOK && TriVertexUVs.Restore(Serializer, "TriVertexUVs");
	bOK = bOK && TriVertexColors.Restore(Serializer, "TriVertexColors");

	gs_debug_assert(Positions.size() == Header.VertexCount);
	gs_debug_assert(Triangles.size() == Header.TriangleCount);
	gs_debug_assert(TriGroups.size() == Header.TriangleCount);
	gs_debug_assert(TriVertexNormals.size() == Header.TriangleCount);
	gs_debug_assert(TriVertexUVs.size() == Header.TriangleCount);
	gs_debug_assert(TriVertexColors.size() == Header.TriangleCount);

	return bOK;
}
