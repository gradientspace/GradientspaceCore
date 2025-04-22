// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"

#include "Core/dynamic_buffer.h"
#include "Core/rle_buffer.h"
#include "Core/unsafe_vector.h"
#include "Core/gs_serializer.h"
#include "Core/gs_debug.h"

#include "Mesh/MeshTypes.h"
#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSVector4.h"
#include "Math/GSIndex2.h"
#include "Math/GSIndex3.h"
#include "Math/GSIndex4.h"
#include "Color/GSIntColor4.h"
#include "Mesh/PolyMeshAttributes.h"

namespace GS
{

typedef IndexedPolyMeshAttribute<Vector2d> PolyMeshUVs;
typedef IndexedPolyMeshAttribute<Vector3f> PolyMeshNormals;
typedef IndexedPolyMeshAttribute<Vector4f> PolyMeshColors;


/**
 * PolyMesh stores a 3D Polygon Mesh that supports Triangles and Quads efficiently
 * by storing them in separate 3/4-element buffers, and Polygons in generic-list buffers.
 *
 * To preserve ordering and make it simpler to iterate over faces, a list of "Face" elements
 * is also stored. A Face is a pair (Type,TypeIndex), where the type represents Tri/Quad/Poly (0/1/2),
 * and the TypeIndex is an index into the given type-specific arrays.
 * 
 * Multiple FaceGroup Sets are supported. The Group Sets are stored in a single buffer
 * as tuples of per-set values, ie for N faces and 2 Sets, the storage ordering is interleaved,
 * ie [(Face0Set0, Face0Set1), (Face1Set0, Face1Set1), ...]. Because of this ordering, it is
 * not currently possible to add a new Set after mesh construction begins. Note that if groups
 * sets were stored sequentially, the number of triangles would need to be known up front!
 * (q: perhaps it would make more sense to store separate arrays?)
 * 
 * Indexed Normals, UVs, and Colors are supported. For each attribute type, up to 4 sets 
 * can be stored. The attribute values are stored sequentially in a flat buffer, 
 * ie [ Set1Values ... Set2Values ... Set3Values ... Set4Values ]. 
 * 
 * For Tris and Quads the attribute index 3/4-tuples are stored inside each IndexedPolyMeshAttribute 
 * interleaved per-set, ie [Quad0Set0, Quad0Set1, Quad1Set0, Quad1Set1, ...]. Inside each
 * Polygon the attribute indices are stored sequentially, ie [Set0UVIndices, Set1UVIndicesPoly, ...]
 *
 * Because of this structure, it is not possible to add a new Set after mesh construction begins.
 * In addition, the number of attribute values must currently be known up front for all
 * but the last set (which can be appended-to)
 * 
 * MaterialIDs are supported, but only one MaterialID per Face
 */
class GRADIENTSPACECORE_API PolyMesh
{
public:

	// todo: maybe we should use some TInlineSmallList-type thing to store these? most could be on stack then...
	struct GRADIENTSPACECORE_API Polygon
	{
		int VertexCount;
		unsafe_vector<int> Vertices;
		//! Normal index polygons packed into sequential per-set arrays ordered [Set0V0 ... Set0VN, Set1V0, ... Set1VN, Set2V0, ...]
		unsafe_vector<int> Normals;
		//! UV index polygons packed into sequential per-set arrays ordered [Set0V0 ... Set0VN, Set1V0, ... Set1VN, Set2V0, ...]
		unsafe_vector<int> UVs;
		//! Color index polygons packed into sequential per-set arrays ordered [Set0V0 ... Set0VN, Set1V0, ... Set1VN, Set2V0, ...]
		unsafe_vector<int> Colors;
	};

	struct GRADIENTSPACECORE_API Face
	{
		//! type of this face (Tri / Quad / Poly)
		uint8_t Type : 4;
		//! index into per-type arrays
		uint32_t Index : 28;

		constexpr bool IsTriangle() const { return Type == 0; }
		constexpr bool IsQuad() const { return Type == 1; }
		constexpr bool IsPolygon() const { return Type == 2; }

		static Face Triangle(int Index) { return Face{ 0, (uint32_t)Index }; }
		static Face Quad(int Index) { return Face{ 1, (uint32_t)Index }; }
		static Face Polygon(int Index) { return Face{ 2, (uint32_t)Index }; }
	};

	struct GRADIENTSPACECORE_API FaceInfo
	{
		//! type of this face (Tri / Quad / Poly)
		uint8_t FaceType;
		//! index into per-type arrays
		uint32_t FaceTypeIndex;
		//! index into per-face arrays
		uint32_t FaceIndex;

		constexpr bool IsTriangle() const { return FaceType == 0; }
		constexpr bool IsQuad() const { return FaceType == 1; }
		constexpr bool IsPolygon() const { return FaceType == 2; }

		FaceInfo(const std::pair<Face, int>& info) {
			FaceType = info.first.Type;
			FaceTypeIndex = info.first.Index;
			FaceIndex = info.second;
		}
		FaceInfo& operator=(const std::pair<Face, int>& info) {
			FaceType = info.first.Type;
			FaceTypeIndex = info.first.Index;
			FaceIndex = info.second;
			return *this;
		}
	};

	enum class EAttributeType
	{
		Normal = 0,
		UV = 1,
		Color = 2
	};

protected:

	unsafe_vector<Vector3d> Positions;

	unsafe_vector<Index3i> Triangles;
	unsafe_vector<Index4i> Quads;
	unsafe_vector<Polygon> Polygons;
	unsafe_vector<Face> Faces;

	unsafe_vector<int> FaceGroups;
	uint32_t NumFaceGroupSets;

	unsafe_vector<int> MaterialIDs;

	PolyMeshNormals NormalSets;
	PolyMeshUVs UVSets;
	PolyMeshColors ColorSets;

public:
	PolyMesh();

	void Clear();

	inline int GetVertexCount() const;
	inline const Vector3d& GetPosition(int VertexIndex) const;
	inline void SetPosition(int VertexIndex, const Vector3d& NewPosition);

	inline int GetFaceCount() const;
	inline const Face& GetFace(int FaceIndex) const;
	inline int GetNumFaceVertices(const Face& Face) const;

	inline int GetTriangleCount() const;
	inline const Index3i& GetTriangle(int TriIndex) const;
	inline const Index3i& GetTriangle(const Face& Face) const;
	inline void SetTriangle(int TriIndex, const Index3i& NewTriangle);
	inline TriVtxPositions GetTriangleVertices(int TriIndex) const;

	inline int GetQuadCount() const;
	inline const Index4i& GetQuad(int QuadIndex) const;
	inline const Index4i& GetQuad(const Face& Face) const;
	inline void SetQuad(int QuadIndex, const Index4i& NewQuad);
	inline QuadVtxPositions GetQuadVertices(int QuadIndex) const;

	inline int GetPolygonCount() const;
	inline const Polygon& GetPolygon(int PolygonIndex) const;
	inline const Polygon& GetPolygon(const Face& Face) const;
	inline void SetPolygon(int PolygonIndex, Polygon&& NewPolygon);

	inline int GetFaceVertexCount(int FaceIndex) const;
	inline int GetFaceVertexCount(const Face& Face) const;
	inline int GetFaceVertex(const Face& Face, int FaceVertexIndex) const;
	
	inline int GetNumFaceGroupSets() const;
	inline int GetFaceGroup(int FaceIndex, int GroupSet = 0) const;
	inline void SetFaceGroup(int FaceIndex, int NewGroup, int GroupSet = 0);

	inline int GetNumNormalSets() const;
	inline int GetNormalCount(int NormalSet) const;
	inline const Vector3f& GetNormal(int NormalIndex, int NormalSet = 0) const;
	inline void SetNormal(int NormalIndex, const Vector3f& NewNormal, int NormalSet = 0);
	inline int GetFaceVertexNormalIndex(const Face& Face, int FaceVertexIndex, int NormalSet = 0) const;
	inline Vector3f GetFaceVertexNormal(const Face& Face, int FaceVertexIndex, int NormalSet = 0) const;

	inline int GetNumUVSets() const;
	inline int GetUVCount(int UVSet) const;
	inline const Vector2d& GetUV(int UVIndex, int UVSet = 0) const;
	inline void SetUV(int UVIndex, const Vector2d& NewUV, int UVSet = 0);
	inline int GetFaceVertexUVIndex(const Face& Face, int FaceVertexIndex, int UVSet = 0) const;
	inline Vector2d GetFaceVertexUV(const Face& Face, int FaceVertexIndex, int UVSet = 0) const;

	inline int GetNumColorSets() const;
	inline int GetColorCount(int ColorSet) const;
	inline const Vector4f& GetColor(int ColorIndex, int ColorSet = 0) const;
	inline void SetColor(int ColorIndex, const Vector4f& NewColor, int ColorSet = 0);
	inline int GetFaceVertexColorIndex(const Face& Face, int FaceVertexIndex, int ColorSet = 0) const;
	inline Vector4f GetFaceVertexColor(const Face& Face, int FaceVertexIndex, int ColorSet = 0) const;

	//! returns false if more than 8 vertices exist in polygon
	bool GetFaceVertexPositions(const Face& Face, InlineVec3dList& Positions) const;
	bool GetFaceVertexIndices(const Face& Face, InlineIndexList& Indices) const;


	//
	// Building
	//
	void ReserveVertices(size_t NumVertices);
	void ReserveFaces(size_t NumFaces);
	void ReserveTriangles(size_t NumTriangles);
	void ReserveQuads(size_t NumQuads);
	void ReservePolygons(size_t NumPolygons);

	int AddVertex(const Vector3d& NewPosition);

	int AddNormalSet(size_t NumNormals);
	int AddUVSet(size_t NumUVs);
	int AddColorSet(size_t NumColors);

	void SetNumGroupSets(int NumGroupSets);

	//! returns <Face, FaceIndex>
	std::pair<Face, int> AddTriangle(
		const Index3i& Triangle,
		int GroupID = 0,
		const Index3i* NormalTriangles = nullptr,
		const Index3i* UVTriangles = nullptr,
		bool bSkipInitialization = false);
	std::pair<Face, int> AddQuad(
		const Index4i& Quad,
		int GroupID = 0,
		const Index4i* NormalQuads = nullptr,
		const Index4i* UVQuads = nullptr,
		bool bSkipInitialization = false);
	std::pair<Face, int> AddPolygon(
		Polygon&& Polygon,
		int GroupID = 0);

	// this is an extremely unsafe alternative to passing Normals/UVs to the functions
	// AddTriangle / AddQuad / AddPolygon, the void* pointers will be assumed to be
	// the correct data type (Index3i* for a Tri, Index4i* for a Quad, or an unsafe_vector<int>* for a Poly).
	// If SingleSetIndex is -1, the pointer will be assumed to contain data for all Attribute Sets
	bool InitializeFaceAttributes(
		int FaceIndex,
		EAttributeType AttributeType,
		const void* Values = nullptr,
		int SingleSetIndex = -1
	);

	// direct access to attributes - be careful!!
	PolyMeshNormals& GetNormalSets() { return NormalSets; }
	const PolyMeshNormals& GetNormalSets() const { return NormalSets; }
	PolyMeshUVs& GetUVSets() { return UVSets; }
	const PolyMeshUVs& GetUVSets() const { return UVSets; }
	PolyMeshColors& GetColorSets() { return ColorSets; }
	const PolyMeshColors& GetColorSets() const { return ColorSets; }

	inline Polygon& GetEditablePolygon(int PolygonIndex) { return Polygons[PolygonIndex]; }

	void Translate(const Vector3d& Translation);
	void Scale(const Vector3d& Scale);

protected:
	void on_append_new_face(int NewGroupID);
};



int PolyMesh::GetVertexCount() const {
	return (int)Positions.size();
}
const Vector3d& PolyMesh::GetPosition(int Index) const {
	return Positions[Index];
}
void PolyMesh::SetPosition(int VertexIndex, const Vector3d& NewPosition) {
	Positions[VertexIndex] = NewPosition;
}

int PolyMesh::GetFaceCount() const {
	return (int)Faces.size();
}
const PolyMesh::Face& PolyMesh::GetFace(int FaceIndex) const {
	return Faces[FaceIndex];
}
int PolyMesh::GetNumFaceVertices(const Face& Face) const
{
	switch (Face.Type)  {
		case 0: return 3; 
		case 1: return 4;
		case 2: return Polygons[Face.Index].VertexCount;
		default: return 0;
	}
}


int PolyMesh::GetTriangleCount() const {
	return (int)Triangles.size();
}
const Index3i& PolyMesh::GetTriangle(int Index) const {
	return Triangles[Index];
}
const Index3i& PolyMesh::GetTriangle(const Face& Face) const {
	gs_debug_assert(Face.Type == 0);
	return Triangles[Face.Index];
}
void PolyMesh::SetTriangle(int TriIndex, const Index3i& NewTriangle) {
	Triangles[TriIndex] = NewTriangle;
}
TriVtxPositions PolyMesh::GetTriangleVertices(int TriIndex) const {
	const Index3i& Tri = Triangles[TriIndex];
	return TriVtxPositions( Positions[Tri.A], Positions[Tri.B], Positions[Tri.C] );
}


int PolyMesh::GetQuadCount() const {
	return (int)Quads.size();
}
const Index4i& PolyMesh::GetQuad(int Index) const {
	return Quads[Index];
}
const Index4i& PolyMesh::GetQuad(const Face& Face) const {
	gs_debug_assert(Face.Type == 1);
	return Quads[Face.Index];
}
void PolyMesh::SetQuad(int QuadIndex, const Index4i& NewQuad) {
	Quads[QuadIndex] = NewQuad;
}
QuadVtxPositions PolyMesh::GetQuadVertices(int QuadIndex) const {
	const Index4i& Quad = Quads[QuadIndex];
	return QuadVtxPositions( Positions[Quad.A], Positions[Quad.B], Positions[Quad.C], Positions[Quad.D] );
}

int PolyMesh::GetPolygonCount() const {
	return (int)Polygons.size();
}
const PolyMesh::Polygon& PolyMesh::GetPolygon(int Index) const {
	return Polygons[Index];
}
const PolyMesh::Polygon& PolyMesh::GetPolygon(const Face& Face) const {
	gs_debug_assert(Face.Type == 2);
	return Polygons[Face.Index];
}
void PolyMesh::SetPolygon(int PolygonIndex, PolyMesh::Polygon&& NewPolygon) {
	Polygons[PolygonIndex] = std::move(NewPolygon);
}


int PolyMesh::GetFaceVertexCount(int FaceIndex) const
{
	gs_debug_assert(FaceIndex >= 0 && FaceIndex < Faces.size());
	return GetFaceVertexCount(Faces[FaceIndex]);
}
int PolyMesh::GetFaceVertexCount(const Face& Face) const
{
	if (Face.Type == 0) return 3;
	else if (Face.Type == 1) return 4;
	else if (Face.Type == 2) return Polygons[Face.Index].VertexCount;
	return 0;
}
int PolyMesh::GetFaceVertex(const Face& Face, int FaceVertexIndex) const
{
	if (Face.Type == 0) return Triangles[Face.Index][FaceVertexIndex];
	else if (Face.Type == 1) return Quads[Face.Index][FaceVertexIndex];
	else if (Face.Type == 2) return Polygons[Face.Index].Vertices[FaceVertexIndex];
	return -1;
}


int PolyMesh::GetNumFaceGroupSets() const {
	return NumFaceGroupSets;
}
int PolyMesh::GetFaceGroup(int FaceIndex, int GroupSet) const {
	return FaceGroups[FaceIndex*NumFaceGroupSets + GroupSet];
}
void PolyMesh::SetFaceGroup(int FaceIndex, int NewGroup, int GroupSet) {
	FaceGroups[FaceIndex*NumFaceGroupSets + GroupSet] = NewGroup;
}

int PolyMesh::GetNumNormalSets() const {
	return NormalSets.NumSets;
}
int PolyMesh::GetNormalCount(int NormalSet) const {
	return NormalSets.GetElementCount(NormalSet);
}
const Vector3f& PolyMesh::GetNormal(int NormalIndex, int NormalSet) const {
	return NormalSets.GetElement(NormalIndex, NormalSet);
}
void PolyMesh::SetNormal(int NormalIndex, const Vector3f& NewNormal, int NormalSet) {
	NormalSets.SetElement(NormalIndex, NewNormal, NormalSet);
}
int PolyMesh::GetFaceVertexNormalIndex(const Face& Face, int FaceVertexIndex, int NormalSet) const
{
	if (Face.Type == 2) {
		const Polygon& Poly = Polygons[Face.Index];
		return Poly.Normals[(Poly.VertexCount*NormalSet) + FaceVertexIndex];
	}
	return NormalSets.GetFaceVertexElementIndex(Face.Type, Face.Index, FaceVertexIndex, NormalSet);
}
Vector3f PolyMesh::GetFaceVertexNormal(const Face& Face, int FaceVertexIndex, int NormalSet) const
{
	int NormalIndex = GetFaceVertexNormalIndex(Face, FaceVertexIndex, NormalSet);
	return (NormalIndex == -1) ? Vector3f::Zero() : NormalSets.GetElement(NormalIndex, NormalSet);
}


int PolyMesh::GetNumUVSets() const {
	return UVSets.NumSets;
}
int PolyMesh::GetUVCount(int UVSet) const {
	return UVSets.GetElementCount(UVSet);
}
const Vector2d& PolyMesh::GetUV(int UVIndex, int UVSet) const {
	return UVSets.GetElement(UVIndex, UVSet);
}
void PolyMesh::SetUV(int UVIndex, const Vector2d& NewUV, int UVSet) {
	UVSets.SetElement(UVIndex, NewUV, UVSet);
}
int PolyMesh::GetFaceVertexUVIndex(const Face& Face, int FaceVertexIndex, int UVSet) const
{
	if (Face.Type == 2)	{
		const Polygon& Poly = Polygons[Face.Index];
		return Poly.UVs[(Poly.VertexCount*UVSet) + FaceVertexIndex];
	}
	return UVSets.GetFaceVertexElementIndex(Face.Type, Face.Index, FaceVertexIndex, UVSet);
}
Vector2d PolyMesh::GetFaceVertexUV(const Face& Face, int FaceVertexIndex, int UVSet) const
{
	int UVIndex = GetFaceVertexUVIndex(Face, FaceVertexIndex, UVSet);
	return (UVIndex == -1) ? Vector2d::Zero() : UVSets.GetElement(UVIndex, UVSet);
}



int PolyMesh::GetNumColorSets() const {
	return ColorSets.NumSets;
}
int PolyMesh::GetColorCount(int ColorSet) const {
	return ColorSets.GetElementCount(ColorSet);
}
const Vector4f& PolyMesh::GetColor(int ColorIndex, int ColorSet) const {
	return ColorSets.GetElement(ColorIndex, ColorSet);
}
void PolyMesh::SetColor(int ColorIndex, const Vector4f& NewColor, int ColorSet) {
	ColorSets.SetElement(ColorIndex, NewColor, ColorSet);
}
int PolyMesh::GetFaceVertexColorIndex(const Face& Face, int FaceVertexIndex, int ColorSet) const
{
	if (Face.Type == 2) {
		const Polygon& Poly = Polygons[Face.Index];
		return Poly.Colors[(Poly.VertexCount*ColorSet) + FaceVertexIndex];
	}
	return ColorSets.GetFaceVertexElementIndex(Face.Type, Face.Index, FaceVertexIndex, ColorSet);
}
Vector4f PolyMesh::GetFaceVertexColor(const Face& Face, int FaceVertexIndex, int ColorSet) const
{
	int ColorIndex = GetFaceVertexColorIndex(Face, FaceVertexIndex, ColorSet);
	return (ColorIndex == -1) ? Vector4f::Zero() : ColorSets.GetElement(ColorIndex, ColorSet);
}



} // end namespace GS
