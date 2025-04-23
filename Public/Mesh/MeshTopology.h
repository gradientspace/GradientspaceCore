// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Math/GSIndex2.h"
#include "Math/GSIndex3.h"
#include "Core/FunctionRef.h"
#include "Core/packed_int_lists.h"

namespace GS
{



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
	packed_int_lists VertexTriangles;
	packed_int_lists VertexVertices;
	
	struct Edge
	{
		Index2i Vertices;		// sorted min, max
		Index2i TriInfo;		// could be <t0,-1>, <t0,t1> (manifold) or <-1, index> (nonmanifold)
		bool IsManifold() const { return TriInfo.A >= 0; }
	};
	unsafe_vector<Edge> Edges;
	packed_int_lists VertexEdges;
	packed_int_lists NonManifoldEdgeTriLists;

	unsafe_vector<Index3i> TriNeighbours;
	packed_int_lists NonManifoldTriTriLists;

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