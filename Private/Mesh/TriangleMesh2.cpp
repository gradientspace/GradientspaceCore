// Copyright Gradientspace Corp. All Rights Reserved.
#include "Mesh/TriangleMesh2.h"

using namespace GS;

BaseMeshAttributeSet::BaseMeshAttributeSet()
{
}

BaseMeshAttributeSet::BaseMeshAttributeSet(const BaseMeshAttributeSet& Copy)
{
	*this = Copy;
}

BaseMeshAttributeSet::BaseMeshAttributeSet(BaseMeshAttributeSet&& Move)
{
	*this = std::move(Move);
}

BaseMeshAttributeSet& BaseMeshAttributeSet::operator=(const BaseMeshAttributeSet& Copy)
{
	gs_debug_assert(false);	// todo
	return *this;
}

BaseMeshAttributeSet& BaseMeshAttributeSet::operator=(BaseMeshAttributeSet&& Move)
{
	VertexAttributes = std::move(Move.VertexAttributes);
	TriangleAttributes = std::move(Move.TriangleAttributes);
	IndexedAttributes = std::move(Move.IndexedAttributes);
	return *this;
}


BaseMeshAttributeSet::~BaseMeshAttributeSet()
{
	for (GenericMeshAttribute* Attrib : VertexAttributes)
		delete Attrib;
	for (GenericMeshAttribute* Attrib : TriangleAttributes)
		delete Attrib;
	for (GenericIndexedMeshAttribute* Attrib : IndexedAttributes)
		delete Attrib;
}


static MeshAttributeName MakeUniqueName(std::vector<GenericMeshAttribute*>& AttribList, const MeshAttributeName& BaseName)
{
	MeshAttributeName UniqueName = BaseName;
	uint8_t UseIndex = UniqueName.Index;
	for (const GenericMeshAttribute* attr : AttribList)
		if (attr->Name().SameBaseName(BaseName))
			UseIndex = GS::Max(attr->Name().Index, UseIndex);
	UniqueName.Index = UseIndex;
	return UniqueName;
}


MeshAttributeName BaseMeshAttributeSet::MakeUniqueVertexAttributeName(const MeshAttributeName& BaseName)
{
	return MakeUniqueName(VertexAttributes, BaseName);
}
MeshAttributeName BaseMeshAttributeSet::MakeUniqueTriangleAttributeName(const MeshAttributeName& BaseName)
{
	return MakeUniqueName(TriangleAttributes, BaseName);
}

MeshAttributeName BaseMeshAttributeSet::AddVertexAttribute(const MeshAttributeName& Name, MeshAttributeHeader Header, int InitialVertexCount)
{
	MeshAttributeName UniqueName = MakeUniqueName(VertexAttributes, Name);
	GenericMeshAttribute* Attrib = new GenericMeshAttribute(UniqueName, Header, InitialVertexCount);
	VertexAttributes.push_back(Attrib);
	return UniqueName;
}

MeshAttributeName BaseMeshAttributeSet::AddTriangleAttribute(const MeshAttributeName& Name, MeshAttributeHeader Header, int InitialTriCount)
{
	MeshAttributeName UniqueName = MakeUniqueName(TriangleAttributes, Name);
	GenericMeshAttribute* Attrib = new GenericMeshAttribute(Name, Header, InitialTriCount);
	TriangleAttributes.push_back(Attrib);
	return UniqueName;
}

MeshAttributeStorage* BaseMeshAttributeSet::FindVertexByName(const MeshAttributeName& Name)
{
	for (GenericMeshAttribute* attr : VertexAttributes)
		if (attr->Name() == Name)
			return &(attr->Data);
	return nullptr;
}
const MeshAttributeStorage* BaseMeshAttributeSet::FindVertexByName(const MeshAttributeName& Name) const 
{
	for (const GenericMeshAttribute* attr : VertexAttributes)
		if (attr->Name() == Name)
			return &(attr->Data);
	return nullptr;
}
MeshAttributeStorage* BaseMeshAttributeSet::FindTriangleByName(const MeshAttributeName& Name)
{
	for (GenericMeshAttribute* attr : TriangleAttributes)
		if (attr->Name() == Name)
			return &(attr->Data);
	return nullptr;
}
const MeshAttributeStorage* BaseMeshAttributeSet::FindTriangleByName(const MeshAttributeName& Name) const
{
	for (const GenericMeshAttribute* attr : TriangleAttributes)
		if (attr->Name() == Name)
			return &(attr->Data);
	return nullptr;
}


