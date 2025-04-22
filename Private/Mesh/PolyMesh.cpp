// Copyright Gradientspace Corp. All Rights Reserved.
#include "Mesh/PolyMesh.h"

using namespace GS;



PolyMesh::PolyMesh()
{
	NumFaceGroupSets = 0;
}



void PolyMesh::Clear()
{
	Positions.clear();

	Triangles.clear();
	Quads.clear();
	Polygons.clear();
	Faces.clear();

	NumFaceGroupSets = 0;
	FaceGroups.clear();

	MaterialIDs.clear();

	NormalSets.Clear();
	UVSets.Clear();
	ColorSets.Clear();
}


void PolyMesh::ReserveVertices(size_t NumVertices)
{
	Positions.reserve(Positions.size() + NumVertices);
}
void PolyMesh::ReserveFaces(size_t NumFaces)
{
	Faces.reserve(Faces.size() + NumFaces);
}
void PolyMesh::ReserveTriangles(size_t NumTriangles)
{
	Triangles.reserve(Triangles.size() + NumTriangles);
}
void PolyMesh::ReserveQuads(size_t NumQuads)
{
	Quads.reserve(Quads.size() + NumQuads);
}
void PolyMesh::ReservePolygons(size_t NumPolygons)
{
	Polygons.reserve(Polygons.size() + NumPolygons);
}

int PolyMesh::AddVertex(const Vector3d& NewPosition)
{
	int vid = (int)Positions.add(NewPosition);
	return vid;
}


void PolyMesh::SetNumGroupSets(int NumGroupSetsIn)
{
	if (Faces.size() != 0) // must do this before starting to append faces
	{
		gs_runtime_assert(false);		
		return;
	}
	NumFaceGroupSets = NumGroupSetsIn;
}


int PolyMesh::AddNormalSet(size_t NumNormals)
{
	if (Faces.size() != 0) // must do this before starting to append faces
	{
		gs_runtime_assert(false);
		return -1;
	}
	return NormalSets.AddSet(NumNormals);
}

int PolyMesh::AddUVSet(size_t NumUVs)
{
	if (Faces.size() != 0) // must do this before starting to append faces
	{
		gs_runtime_assert(false);
		return -1;
	}
	return UVSets.AddSet(NumUVs);
}

int PolyMesh::AddColorSet(size_t NumColors)
{
	if (Faces.size() != 0) // must do this before starting to append faces
	{
		gs_runtime_assert(false);
		return -1;
	}
	return ColorSets.AddSet(NumColors);
}


std::pair<PolyMesh::Face, int> PolyMesh::AddTriangle(
	const Index3i& Triangle,
	int GroupID,
	const Index3i* NormalTriangles,
	const Index3i* UVTriangles,
	bool bSkipInitialization)
{
	int NewTriIndex = (int)Triangles.add(Triangle);

	if (NormalSets.NumSets > 0) 
		NormalSets.AddTriangle(NormalTriangles, bSkipInitialization);

	if (UVSets.NumSets > 0) 
		UVSets.AddTriangle(UVTriangles, bSkipInitialization);

	if (ColorSets.NumSets > 0)
		ColorSets.AddTriangle(nullptr, bSkipInitialization);

	Face NewFace = PolyMesh::Face::Triangle(NewTriIndex);
	int NewFaceIndex = (int)Faces.add(NewFace);
	on_append_new_face(GroupID);
	return { NewFace, NewFaceIndex };
}


std::pair<PolyMesh::Face, int> PolyMesh::AddQuad(
	const Index4i& Quad,
	int GroupID,
	const Index4i* NormalQuads,
	const Index4i* UVQuads,
	bool bSkipInitialization)
{
	int NewQuadIndex = (int)Quads.add(Quad);

	if (NormalSets.NumSets > 0)
		NormalSets.AddQuad(NormalQuads, bSkipInitialization);

	if (UVSets.NumSets > 0) 
		UVSets.AddQuad(UVQuads, bSkipInitialization);

	if (ColorSets.NumSets > 0)
		ColorSets.AddQuad(nullptr, bSkipInitialization);

	Face NewFace = PolyMesh::Face::Quad(NewQuadIndex);
	int NewFaceIndex = (int)Faces.add(NewFace);
	on_append_new_face(GroupID);
	return { NewFace, NewFaceIndex };
}

std::pair<PolyMesh::Face, int> PolyMesh::AddPolygon(
	PolyMesh::Polygon&& Polygon,
	int GroupID)
{
	gs_debug_assert(Polygon.Vertices.size() == Polygon.VertexCount);
	gs_debug_assert(Polygon.Normals.size() == 0 || Polygon.Normals.size() == (Polygon.VertexCount * NormalSets.NumSets));
	gs_debug_assert(Polygon.UVs.size() == 0 || Polygon.UVs.size() == (Polygon.VertexCount * UVSets.NumSets));
	gs_debug_assert(Polygon.Colors.size() == 0 || Polygon.Colors.size() == (Polygon.VertexCount * ColorSets.NumSets));

	int NewPolyIndex = (int)Polygons.add_move(std::move(Polygon));

	// todo validate polygon normals and UVs?

	Face NewFace = PolyMesh::Face::Polygon(NewPolyIndex);
	int NewFaceIndex = (int)Faces.add(NewFace);
	on_append_new_face(GroupID);
	return { NewFace, NewFaceIndex };
}


bool PolyMesh::InitializeFaceAttributes(
	int FaceIndex,
	PolyMesh::EAttributeType AttributeType,
	const void* Values,
	int SingleSetIndex )
{
	if (FaceIndex < 0 || FaceIndex >= Faces.size()) return false;
	if (Values == nullptr) return false;

	const Face& Face = Faces[FaceIndex];
	if (Face.Type == 0) {
		switch (AttributeType) {
			case EAttributeType::Normal:
				NormalSets.SetTriangle(Face.Index, (const Index3i*)Values, SingleSetIndex); break;
			case EAttributeType::UV:
				UVSets.SetTriangle(Face.Index, (const Index3i*)Values, SingleSetIndex); break;
			case EAttributeType::Color:
				ColorSets.SetTriangle(Face.Index, (const Index3i*)Values, SingleSetIndex); break;
		}
		return true;
	} else if (Face.Type == 1) {
		switch (AttributeType) {
			case EAttributeType::Normal:
				NormalSets.SetQuad(Face.Index, (const Index4i*)Values, SingleSetIndex); break;
			case EAttributeType::UV:
				UVSets.SetQuad(Face.Index, (const Index4i*)Values, SingleSetIndex); break;
			case EAttributeType::Color:
				ColorSets.SetQuad(Face.Index, (const Index4i*)Values, SingleSetIndex); break;
		}
		return true;
	}
	else if (Face.Type == 2)
	{
		Polygon& Poly = Polygons[Face.Index];
		const unsafe_vector<int>* ElementIDList = (const unsafe_vector<int>*)Values;
		if ( SingleSetIndex == -1 )
		{
			switch (AttributeType) {
				case EAttributeType::Normal: Poly.Normals = *ElementIDList; break;
				case EAttributeType::UV: Poly.UVs = *ElementIDList; break;
				case EAttributeType::Color: Poly.Colors = *ElementIDList; break;
			}
		}
		else
		{
			int StartIndex = SingleSetIndex * Poly.VertexCount;
			switch (AttributeType) {
				case EAttributeType::Normal: 
					if (Poly.Normals.size() != Poly.VertexCount * NormalSets.NumSets)
						Poly.Normals.initialize(Poly.VertexCount * NormalSets.NumSets, -1);
					for (int j = 0; j < Poly.VertexCount; ++j) { Poly.Normals[StartIndex+j] = (*ElementIDList)[j]; } 
					break;
				case EAttributeType::UV:
					if (Poly.UVs.size() != Poly.VertexCount * UVSets.NumSets)
						Poly.UVs.initialize(Poly.VertexCount * UVSets.NumSets, -1);
					for (int j = 0; j < Poly.VertexCount; ++j) { Poly.UVs[StartIndex+j] = (*ElementIDList)[j]; } 
					break;
				case EAttributeType::Color:
					if (Poly.Colors.size() != Poly.VertexCount * ColorSets.NumSets)
						Poly.Colors.initialize(Poly.VertexCount * ColorSets.NumSets, -1);
					for (int j = 0; j < Poly.VertexCount; ++j) { Poly.Colors[StartIndex+j] = (*ElementIDList)[j]; } 
					break;
			}
		}
		return true;
	}
	return false;
}


void PolyMesh::on_append_new_face(int NewGroupID)
{
	if (NumFaceGroupSets > 0)
	{
		int NewGroupStartIndex = (int)FaceGroups.grow(NumFaceGroupSets);
		FaceGroups[NewGroupStartIndex] = NewGroupID;
		for (int j = 1; j < (int)NumFaceGroupSets; ++j)
			FaceGroups[NewGroupStartIndex + j] = 0;

		gs_debug_assert(FaceGroups.size() == NumFaceGroupSets * Faces.size());
	}
}


bool PolyMesh::GetFaceVertexIndices(const Face& Face, InlineIndexList& IndexList) const
{
	if (Face.IsTriangle()) {
		const Index3i& Tri = Triangles[Face.Index];
		IndexList.SetValues3(Tri.A, Tri.B, Tri.C);
		return true;
	}
	else if (Face.IsQuad()) {
		const Index4i& Quad = Quads[Face.Index];
		IndexList.SetValues4(Quad.A, Quad.B, Quad.C, Quad.D);
		return true;
	}
	else if (Face.IsPolygon()) {
		const Polygon& Poly = Polygons[Face.Index];
		IndexList.SetSize(Poly.VertexCount);
		for (int j = 0; j < Poly.VertexCount; ++j)
			IndexList[j] = Poly.Vertices[j];
		return true;
	}
	return false;
}

bool PolyMesh::GetFaceVertexPositions(const Face& Face, InlineVec3dList& PositionsList) const
{
	if (Face.IsTriangle()) {
		const Index3i& Tri = Triangles[Face.Index];
		PositionsList.SetValues3(Positions[Tri.A], Positions[Tri.B], Positions[Tri.C]);
		return true;
	}
	else if (Face.IsQuad()) {
		const Index4i& Quad = Quads[Face.Index];
		PositionsList.SetValues4(Positions[Quad.A], Positions[Quad.B], Positions[Quad.C], Positions[Quad.D]);
		return true;
	}
	else if (Face.IsPolygon()) {
		const Polygon& Poly = Polygons[Face.Index];
		PositionsList.SetSize(Poly.VertexCount);
		for (int j = 0; j < Poly.VertexCount; ++j)
			PositionsList[j] = Positions[Poly.Vertices[j]];
		return true;
	}
	return false;
}


void PolyMesh::Translate(const Vector3d& Translation)
{
	int NV = (int)Positions.size();
	for (int i = 0; i < NV; ++i) {
		const Vector3d& V = Positions[i];
		Positions[i] = Vector3d(V.X + Translation.X, V.Y + Translation.Y, V.Z + Translation.Z);
	}
}

void PolyMesh::Scale(const Vector3d& Scale)
{
	int NV = (int)Positions.size();
	for (int i = 0; i < NV; ++i) {
		const Vector3d& V = Positions[i];
		Positions[i] = Vector3d(V.X * Scale.X, V.Y * Scale.Y, V.Z * Scale.Z);
	}
}
