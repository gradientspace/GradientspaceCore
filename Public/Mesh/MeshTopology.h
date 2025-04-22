// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Math/GSIndex2.h"
#include "Math/GSIndex3.h"
#include "Core/FunctionRef.h"

namespace GS
{

class GRADIENTSPACECORE_API PackedIntLists
{
public:
	// indices into PackedLists
	unsafe_vector<int> ListPointers;
	// sequential packed lists, each list starts with list size and then elements
	unsafe_vector<int> PackedLists;

	int NumLists() const { return (int)ListPointers.size(); }
	int64_t NumListElements() const { return PackedLists.size(); }

	//! initialize with a fixed number of known ListIDs
	void Initialize(int NumListIDs, int ListSizeEstimate = 0, size_t KnownExactTotalListItems = 0);

	//! append a (non-empty) list at a given ListID. returns false if ListID's list is non-empty, or on invalid input.
	bool AppendList(int ListID, int NumItems, const int* Items);

	//! append a (non-empty) list at a new ListID. returns new ListID, or -1 on invalid input.
	int AppendList(int NumItems, const int* Items);

	bool HasList(int ListID) const { return ListPointers[ListID] >= 0; }

	int GetListSizeUnsafe(int ListID) const { 
		return PackedLists[ListPointers[ListID]]; 
	}
	
	const int* GetListItemsUnsafe(int ListID) const { 
		return PackedLists.raw_pointer(ListPointers[ListID] + 1); 
	}

	const int* GetListItemsUnsafe(int ListID, int& SizeOut) const {
		const int* ptr = PackedLists.raw_pointer(ListPointers[ListID]);
		SizeOut = *ptr;
		return (ptr + 1);
	}

	const_buffer_view<int> GetListView(int ListID) const;
};




enum class EMeshTopologyTypes : uint8_t
{
	TriangleNeighbours = 1 << 1,
	VertexTriangles = 1 << 2,
	VertexVertices = 1 << 3,
	VertexEdges = 1 << 4,
	All = 0xFF
};
inline EMeshTopologyTypes operator|(EMeshTopologyTypes A, EMeshTopologyTypes B) {
	return (EMeshTopologyTypes)((uint8_t)A | (uint8_t)B);
}
inline bool operator&(EMeshTopologyTypes Combined, EMeshTopologyTypes Check) {
	return ((uint8_t)Combined & (uint8_t)Check) != 0;
}



class GRADIENTSPACECORE_API MeshTopology
{


public:
	PackedIntLists VertexTriangles;
	PackedIntLists VertexVertices;
	
	struct Edge
	{
		Index2i Vertices;		// sorted min, max
		Index2i TriInfo;		// could be <t0,-1>, <t0,t1> (manifold) or <-1, index> (nonmanifold)
		bool IsManifold() const { return TriInfo.A >= 0; }
	};
	unsafe_vector<Edge> Edges;
	PackedIntLists VertexEdges;
	PackedIntLists NonManifoldEdgeTriLists;

	unsafe_vector<Index3i> TriNeighbours;
	PackedIntLists NonManifoldTriTriLists;

public:
	void Build(
		int NumVertexIDs,
		FunctionRef<bool(int)> IsVertexValidFunc,
		int NumTriangleIDs,
		FunctionRef<bool(int TriangleID, Index3i& TriVertices)> GetTriangleFunc,
		EMeshTopologyTypes WhichParts = EMeshTopologyTypes::All
	);


public:

	bool IsNonManifold() const { return NonManifoldEdgeTriLists.NumLists() > 0 || NonManifoldTriTriLists.NumLists() > 0; }

	bool HasTriNeighbours() const { return TriNeighbours.size() > 0; }
	bool HasVertexTriangles() const { return VertexTriangles.NumLists() > 0; }
	bool HasVertexVertices() const { return VertexVertices.NumLists() > 0; }
	bool HasVertexEdges() const { return VertexEdges.NumLists() > 0; }


	//! return Index into .Edges[], or -1 
	int FindEdgeID(int VertexA, int VertexB) const;
	
	//! return true if TriangleA and TriangleB are connected. Requires that TriangleNeighbours are available.
	inline bool AreTrisConnected(int TriangleA, int TriangleB) const {
		return TriNeighbours[TriangleA].Contains(TriangleB);
	}


	bool CheckValidity() const;

};




}