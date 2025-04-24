// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Math/GSVector2.h"
#include "Math/GSTriangle2.h"
#include "Math/GSIndex3.h"
#include "Math/GSIndex4.h"
#include "Mesh/TriangleMeshAttributes.h"
#include "Mesh/MeshView2.h"

#include <vector>

namespace GS
{

template<typename RealType>
class TTriangleMesh2
{
public:
	unsafe_vector<Vector2<RealType>> VertexPositions;
	unsafe_vector<Index3i> Triangles;

	void SetNumVertexIDs(int VertexCount) {
		VertexPositions.resize(VertexCount);
	}
	void ReserveNewVertexIDs(int AdditionalVertexCount) {
		VertexPositions.reserve(VertexPositions.size() + AdditionalVertexCount);
	}
	int GetNumVertexIDs() const {
		return (int)VertexPositions.size();
	}
	bool IsValidVertexID(int VertexID) const {
		return VertexID >= 0 && VertexID < (int)VertexPositions.size();
	}

	int32_t AddPosition(const Vector2<RealType>& NewPosition) {
		return (int32_t)VertexPositions.add(NewPosition);
	}

	void SetPosition(int VertexID, const Vector2<RealType>& NewPosition) {
		VertexPositions[VertexID] = NewPosition;
	}
	const Vector2<RealType>& GetPosition(int VertexID) const {
		return VertexPositions[VertexID];
	}

	void SetNumTriangleIDs(int TriangleCount) {
		Triangles.resize(TriangleCount);
	}
	void ReserveNewTriangleIDs(int AdditionalTriangleCount)
	{
		Triangles.reserve(Triangles.size() + AdditionalTriangleCount);
	}
	int GetNumTriangleIDs() const {
		return (int)Triangles.size();
	}
	bool IsValidTriangleID(int TriangleID) const {
		return TriangleID >= 0 && TriangleID < (int)Triangles.size();
	}

	int32_t AddTriangle(const Index3i& NewTriangle) {
		return (int32_t)Triangles.add(NewTriangle);
	}

	void SetTriangle(int TriangleID, const Index3i& NewTriangle) {
		Triangles[TriangleID] = NewTriangle;
	}
	const Index3i& GetTriangle(int TriangleID) const {
		return Triangles[TriangleID];
	}

	void GetTriangle(int TriangleID, Triangle2<RealType>& TriangleOut) const {
		Index3i TriV = Triangles[TriangleID];
		TriangleOut.A = VertexPositions[TriV.A];
		TriangleOut.B = VertexPositions[TriV.B];
		TriangleOut.C = VertexPositions[TriV.C];
	}

	template<typename RealType2>
	void GetTriangle(int TriangleID, Triangle2<RealType2>& TriangleOut) const {
		Index3i TriV = Triangles[TriangleID];
		TriangleOut.A = (Vector2<RealType2>)VertexPositions[TriV.A];
		TriangleOut.B = (Vector2<RealType2>)VertexPositions[TriV.B];
		TriangleOut.C = (Vector2<RealType2>)VertexPositions[TriV.C];
	}
};
typedef TTriangleMesh2<float> TriangleMesh2f;
typedef TTriangleMesh2<double> TriangleMesh2d;








template<typename RealType>
struct VertexContainer2
{
public:
	unsafe_vector<Vector2<RealType>> Positions;

	int GetStride() const { return sizeof(Vector2<RealType>); }

	void SetNum(int VertexCount) {
		Positions.resize(VertexCount);
	}
	void ReserveAdd(int AdditionalVertexCount)
	{
		Positions.reserve(Positions.size() + AdditionalVertexCount);
	}
	int GetNum() const {
		return (int)Positions.size();
	}
	bool IsValid(int VertexID) const {
		return VertexID >= 0 && VertexID < (int)Positions.size();
	}

	int32_t Add(const Vector2<RealType>& NewPosition) {
		return (int32_t)Positions.add(NewPosition);
	}

	void Set(int VertexID, const Vector2<RealType>& NewPosition) {
		Positions[VertexID] = NewPosition;
	}
	const Vector2<RealType>& Get(int VertexID) const {
		return Positions[VertexID];
	}

	const Vector2<RealType>& operator[](int VertexID) const {
		return Positions[VertexID];
	}
};



struct GRADIENTSPACECORE_API TriangleContainer
{
public:
	unsafe_vector<Index3i> Triangles;

	inline int GetStride() const { return sizeof(Index3i); }

	inline void SetNum(int TriangleCount) {
		Triangles.resize(TriangleCount);
	}
	inline void ReserveAdd(int AdditionalTriangleCount) {
		Triangles.reserve(Triangles.size() + AdditionalTriangleCount);
	}
	inline int GetNum() const {
		return (int)Triangles.size();
	}
	inline bool IsValid(int TriangleID) const {
		return TriangleID >= 0 && TriangleID < (int)Triangles.size();
	}

	inline int32_t Add(const Index3i& NewTriangle) {
		return (int32_t)Triangles.add(NewTriangle);
	}

	inline void SetTriangle(int TriangleID, const Index3i& NewTriangle) {
		Triangles[TriangleID] = NewTriangle;
	}
	inline const Index3i& GetTriangle(int TriangleID) const {
		return Triangles[TriangleID];
	}

	inline const Index3i& operator[](int TriangleID) const {
		return Triangles[TriangleID];
	}
};



struct GRADIENTSPACECORE_API TriangleGroupContainer
{
public:
	unsafe_vector<Index4i> Triangles;

	inline int GetStride() const { return sizeof(Index4i); }

	inline void SetNum(int TriangleCount) {
		Triangles.resize(TriangleCount);
	}
	inline void ReserveAdd(int AdditionalTriangleCount) {
		Triangles.reserve(Triangles.size() + AdditionalTriangleCount);
	}
	inline int GetNum() const {
		return (int)Triangles.size();
	}
	inline bool IsValid(int TriangleID) const {
		return TriangleID >= 0 && TriangleID < (int)Triangles.size();
	}

	inline int32_t Add(const Index3i& NewTriangle, int GroupID) {
		return (int32_t)Triangles.add(Index4i(NewTriangle.A,NewTriangle.B,NewTriangle.C,GroupID));
	}

	inline void SetTriangle(int TriangleID, const Index3i& NewTriangle, int GroupID) {
		Triangles[TriangleID] = Index4i(NewTriangle.A, NewTriangle.B, NewTriangle.C, GroupID);
	}
	inline Index3i GetTriangle(int TriangleID) const {
		return Index3i(Triangles[TriangleID].AsPointer());
	}
	inline Index3i GetTriangle(int TriangleID, int& GroupID) const {
		const Index4i& T = Triangles[TriangleID];
		GroupID = T.D;
		return Index3i(T.AsPointer());
	}
	inline int GetTriangleGroup(int TriangleID) const {
		return Triangles[TriangleID].D;
	}

	inline Index3i operator[](int TriangleID) const {
		return Index3i(Triangles[TriangleID].AsPointer());
	}
};






template<typename RealType>
class TMinimalTriMesh2
{
public:
	VertexContainer2<RealType> Vertices;
	TriangleContainer Triangles;

	int GetNumVertexIDs() const {
		return Vertices.GetNum();
	}
	bool IsValidVertexID(int VertexID) const {
		return Vertices.IsValid(VertexID);
	}
	const Vector2<RealType>& GetVertex(int VertexID) const {
		return Vertices[VertexID];
	}

	//SetNum
	//Reserve
	//Add
	//Set



	int GetNumTriangleIDs() const {
		return Triangles.GetNum();
	}
	bool IsValidTriangleID(int TriangleID) const {
		return Triangles.IsValid(TriangleID);
	}
	const Index3i& GetTriangle(int TriangleID) const {
		return Triangles[TriangleID];
	}


	void GetTriangle(int TriangleID, Triangle2<RealType>& TriangleOut) const {
		Index3i TriV = Triangles[TriangleID];
		TriangleOut.A = Vertices[TriV.A];
		TriangleOut.B = Vertices[TriV.B];
		TriangleOut.C = Vertices[TriV.C];
	}

	template<typename RealType2>
	void GetTriangle(int TriangleID, Triangle2<RealType2>& TriangleOut) const {
		Index3i TriV = Triangles[TriangleID];
		TriangleOut.A = (Vector2<RealType2>)Vertices[TriV.A];
		TriangleOut.B = (Vector2<RealType2>)Vertices[TriV.B];
		TriangleOut.C = (Vector2<RealType2>)Vertices[TriV.C];
	}


	TConstMeshView2<RealType> GetMeshView() const {
		return TConstMeshView2<RealType>(
			(const uint8_t*)Vertices.Positions.raw_pointer(), Vertices.GetNum(),
			(const uint8_t*)Triangles.Triangles.raw_pointer(), Triangles.GetNum(),
			Vertices.GetStride(), Triangles.GetStride() );
	}
};
typedef TMinimalTriMesh2<float> MinimalTriMesh2f;
typedef TMinimalTriMesh2<double> MinimalTriMesh2d;









class GRADIENTSPACECORE_API GenericMeshAttribute
{
protected:
	MeshAttributeName m_Name;

public:
	MeshAttributeStorage Data;

	// allow subclasses of attribute that can have alternate behaviors / additional data / etc
	virtual ~GenericMeshAttribute() {}

	GenericMeshAttribute() = delete;

	GenericMeshAttribute(const MeshAttributeName& UseName, MeshAttributeHeader Header, int InitialElementCount = 0)
		: m_Name(UseName), Data(Header)
	{
		if (InitialElementCount > 0)
			Data.SetNumElements(InitialElementCount);
	}

	const MeshAttributeName& Name() const { return m_Name; }

	// 
	void Rename(const MeshAttributeName& NewName)
	{
		m_Name = NewName;
	}
};



class GRADIENTSPACECORE_API BaseMeshAttributeSet
{
public:
	std::vector<GenericMeshAttribute*> VertexAttributes;
	std::vector<GenericMeshAttribute*> TriangleAttributes;
	std::vector<GenericIndexedMeshAttribute*> IndexedAttributes;

	BaseMeshAttributeSet();
	BaseMeshAttributeSet(const BaseMeshAttributeSet& Copy);
	BaseMeshAttributeSet(BaseMeshAttributeSet&& Move);
	BaseMeshAttributeSet& operator=(const BaseMeshAttributeSet& Copy);
	BaseMeshAttributeSet& operator=(BaseMeshAttributeSet&& Move);
	~BaseMeshAttributeSet();

	/**
	 * Create a new name w/ a unique Index, for the given BaseName (ie if UV0 and UV1 exist, then passing UV0 will return UV2)
	 * This is not necessary if using AddVertexAttribute() as it will be called automatically
	 */
	MeshAttributeName MakeUniqueVertexAttributeName(const MeshAttributeName& BaseName);
	MeshAttributeName MakeUniqueTriangleAttributeName(const MeshAttributeName& BaseName);

	/**
	 * Add a GenericMeshAttribute w/ the provided Header. A Name w/ a unique Index is derived from the BaseName.
	 * @return the derived unique Name
	 */
	MeshAttributeName AddVertexAttribute(const MeshAttributeName& BaseName, MeshAttributeHeader Header, int InitialVertexCount);
	MeshAttributeName AddTriangleAttribute(const MeshAttributeName& BaseName, MeshAttributeHeader Header, int InitialTriCount);

	// todo remove functions...

	/**
	 * Find a vertex attribute by name
	 */
	MeshAttributeStorage* FindVertexByName(const MeshAttributeName& Name);
	const MeshAttributeStorage* FindVertexByName(const MeshAttributeName& Name) const;
	MeshAttributeStorage* FindTriangleByName(const MeshAttributeName& Name);
	const MeshAttributeStorage* FindTriangleByName(const MeshAttributeName& Name) const;

	// get header...
	// get count of type
	// enumerate of type


	MeshNormal3fView ReadVertexNormal3f(uint8_t Index = 0) const {
		return MeshNormal3fView(FindVertexByName(MeshAttributeName::Normal(Index)));
	}
	MeshNormal3fEditor EditVertexNormal3f(uint8_t Index = 0) {
		return MeshNormal3fEditor(FindVertexByName(MeshAttributeName::Normal(Index)));
	}

	MeshUV2fView ReadVertexUV2f(uint8_t Index = 0) const {
		return MeshUV2fView(FindVertexByName(MeshAttributeName::UV(Index)));
	}
	MeshUV2fEditor EditVertexUV2f(uint8_t Index = 0) {
		return MeshUV2fEditor(FindVertexByName(MeshAttributeName::UV(Index)));
	}

	MeshColor4fView ReadVertexColor4f(uint8_t Index = 0) const {
		return MeshColor4fView(FindVertexByName(MeshAttributeName::Color4f(Index)));
	}
	MeshColor4fEditor EditVertexColor4f(uint8_t Index = 0) {
		return MeshColor4fEditor(FindVertexByName(MeshAttributeName::Color4f(Index)));
	}

	MeshColor4bView ReadVertexColor4b(uint8_t Index = 0) const {
		return MeshColor4bView(FindVertexByName(MeshAttributeName::Color4b(Index)));
	}
	MeshColor4bEditor ReadVertexColor4b(uint8_t Index = 0) {
		return MeshColor4bEditor(FindVertexByName(MeshAttributeName::Color4b(Index)));
	}

	MeshTriGroupView ReadTriGroup(uint8_t Index = 0) const {
		return MeshTriGroupView(FindTriangleByName(MeshAttributeName::TriGroup(Index)));
	}
	MeshTriGroupEditor EditTriGroup(uint8_t Index = 0) {
		return MeshTriGroupEditor(FindTriangleByName(MeshAttributeName::TriGroup(Index)));
	}

	MeshTriMaterialView ReadTriMaterial(uint8_t Index = 0) const {
		return MeshTriMaterialView(FindTriangleByName(MeshAttributeName::Material(Index)));
	}
	MeshTriMaterialEditor EditTriMaterial(uint8_t Index = 0) {
		return MeshTriMaterialEditor(FindTriangleByName(MeshAttributeName::Material(Index)));
	}



	void SetNumVertices(int VertexCount) {
		for (GenericMeshAttribute* attr : VertexAttributes)
			attr->Data.SetNumElements(VertexCount);
	}
	void ReserveAddVertices(int NumAdditional) {
		for (GenericMeshAttribute* attr : VertexAttributes)
			attr->Data.ReserveAdd(NumAdditional);
	}
	void AddVertex() {
		for (GenericMeshAttribute* attr : VertexAttributes)
			attr->Data.AddElement();
	}

	void SetNumTriangles(int TriangleCount) {
		for (GenericMeshAttribute* attr : TriangleAttributes)
			attr->Data.SetNumElements(TriangleCount);
	}
	void ReserveAddTriangles(int NumAdditional) {
		for (GenericMeshAttribute* attr : TriangleAttributes)
			attr->Data.ReserveAdd(NumAdditional);
	}
	void AddTriangle() {
		for (GenericMeshAttribute* attr : TriangleAttributes)
			attr->Data.AddElement();
	}



	template<typename GenericMeshAttributeType>
	MeshAttributeName AddVertexAttributeOfType(const MeshAttributeName& BaseName, MeshAttributeHeader Header, int InitialVertexCount)
	{
		MeshAttributeName UniqueName = MakeUniqueVertexAttributeName(BaseName);
		GenericMeshAttributeType* Attrib = new GenericMeshAttributeType(UniqueName, Header, InitialVertexCount);
		VertexAttributes.push_back(Attrib);
		return UniqueName;
	}

	template<typename GenericMeshAttributeType>
	MeshAttributeName AddTriangleAttribute(const MeshAttributeName& BaseName, MeshAttributeHeader Header, int InitialTriCount)
	{
		MeshAttributeName UniqueName = MakeUniqueTriangleAttributeName(BaseName);
		GenericMeshAttributeType* Attrib = new GenericMeshAttributeType(UniqueName, Header, InitialTriCount);
		TriangleAttributes.push_back(Attrib);
		return UniqueName;
	}


	template<typename MeshType>
	MeshAttributeName AddVertexColor4f(const MeshType& BaseMesh) {
		return AddVertexAttribute(MeshAttributeName::Color4f(), MeshAttributeHeader::Color4f(), BaseMesh.GetNumVertexIDs());
	}
	template<typename MeshType>
	MeshAttributeName AddVertexColor4b(const MeshType& BaseMesh) {
		return AddVertexAttribute(MeshAttributeName::Color4b(), MeshAttributeHeader::Color4b(), BaseMesh.GetNumVertexIDs());
	}

	template<typename MeshType>
	MeshAttributeName AddTriangleMaterial(const MeshType& BaseMesh) {
		return AddTriangleAttribute(MeshAttributeName::Material(), MeshAttributeHeader::Material(), BaseMesh.GetNumTriangleIDs());
	}

	template<typename MeshType>
	MeshAttributeName AddTriangleGroup(const MeshType& BaseMesh) {
		return AddTriangleAttribute(MeshAttributeName::TriGroup(), MeshAttributeHeader::TriGroup(), BaseMesh.GetNumTriangleIDs());
	}

	//MeshAttributeName AddGroupSet();
};






class GRADIENTSPACECORE_API GeneralTriMesh2d
{
public:
	VertexContainer2<double> Vertices;
	TriangleGroupContainer Triangles;
	BaseMeshAttributeSet Attributes;

	inline void SetNumVertexIDs(int VertexCount) {
		Vertices.SetNum(VertexCount);
		Attributes.SetNumVertices(VertexCount);
	}
	inline void ReserveNewVertexIDs(int AdditionalVertexCount) {
		Vertices.ReserveAdd(AdditionalVertexCount);
		Attributes.ReserveAddVertices(AdditionalVertexCount);
	}
	inline int GetNumVertexIDs() const {
		return Vertices.GetNum();
	}
	inline bool IsValidVertexID(int VertexID) const {
		return Vertices.IsValid(VertexID);
	}

	inline int32_t AddPosition(const Vector2d& NewPosition) {
		int32_t NewID = (int32_t)Vertices.Add(NewPosition);
		Attributes.AddVertex();
		return NewID;
	}

	inline void SetPosition(int VertexID, const Vector2d& NewPosition) {
		Vertices.Set(VertexID, NewPosition);
	}
	inline const Vector2d& GetPosition(int VertexID) const {
		return Vertices[VertexID];
	}



	inline void SetNumTriangleIDs(int TriangleCount) {
		Triangles.SetNum(TriangleCount);
		Attributes.SetNumTriangles(TriangleCount);
	}
	inline void ReserveNewTriangleIDs(int AdditionalTriangleCount) {
		Triangles.ReserveAdd(AdditionalTriangleCount);
		Attributes.ReserveAddTriangles(AdditionalTriangleCount);
	}
	inline int GetNumTriangleIDs() const {
		return Triangles.GetNum();
	}
	inline bool IsValidTriangleID(int TriangleID) const {
		return Triangles.IsValid(TriangleID) && Triangles[TriangleID].A >= 0;
	}

	inline int32_t AddTriangle(const Index3i& NewTriangle) {
		int32_t NewID = (int32_t)Triangles.Add(NewTriangle, 0);
		Attributes.AddTriangle();
		return NewID;
	}
	inline int32_t AddTriangle(const Index3i& NewTriangle, int GroupID) {
		int32_t NewID = (int)Triangles.Add(NewTriangle, GroupID);
		Attributes.AddTriangle();
		return NewID;
	}

	inline void SetTriangle(int TriangleID, const Index3i& NewTriangle) {
		Triangles.SetTriangle(TriangleID, NewTriangle, 0);
	}
	inline void SetTriangle(int TriangleID, const Index3i& NewTriangle, int GroupID) {
		Triangles.SetTriangle(TriangleID, NewTriangle, GroupID);
	}

	inline Index3i GetTriangle(int TriangleID) const {
		return Triangles[TriangleID];
	}
	inline Index3i GetTriangle(int TriangleID, int& GroupID) const {
		return Triangles.GetTriangle(TriangleID, GroupID);
	}
	inline int GetTriangleGroup(int TriangleID) const {
		return Triangles.GetTriangleGroup(TriangleID);
	}

	void GetTriangle(int TriangleID, Triangle2d& TriangleOut) const {
		Index3i TriV = Triangles[TriangleID];
		TriangleOut.A = Vertices[TriV.A];
		TriangleOut.B = Vertices[TriV.B];
		TriangleOut.C = Vertices[TriV.C];
	}

	template<typename RealType2>
	void GetTriangle(int TriangleID, Triangle2<RealType2>& TriangleOut) const {
		Index3i TriV = Triangles[TriangleID];
		TriangleOut.A = (Vector2<RealType2>)Vertices[TriV.A];
		TriangleOut.B = (Vector2<RealType2>)Vertices[TriV.B];
		TriangleOut.C = (Vector2<RealType2>)Vertices[TriV.C];
	}

	ConstMeshView2d GetMeshView() const {
		return ConstMeshView2d(
			(const uint8_t*)Vertices.Positions.raw_pointer(), Vertices.GetNum(),
			(const uint8_t*)Triangles.Triangles.raw_pointer(), Triangles.GetNum(),
			Vertices.GetStride(), Triangles.GetStride() );
	}




	MeshAttributeName AddVertexColor4f() {
		return Attributes.AddVertexColor4f(*this);
	}
	MeshAttributeName AddTriangleMaterial() {
		return Attributes.AddTriangleMaterial(*this);
	}



};






}