// Copyright Gradientspace Corp. All Rights Reserved.
#include "Mesh/MeshTopology.h"

#include "Core/gs_debug.h"
#include "Mesh/MeshTypes.h"
#include "Core/dynamic_buffer.h"

#include <vector>

using namespace GS;




namespace GS
{
	// build per-vertex lists of unique triangles
	// TODO: InlineIndexList is kinda memory-expensive in this context...
	void BuildPerVertexTriangleSets(
		int NumVertexIDs,
		int NumTriangleIDs,
		FunctionRef<bool(int TriangleID, Index3i& TriVertices)> GetTriangleFunc,
		dynamic_buffer<InlineIndexList>& VertTriSets, 
		size_t& TotalVertTriCountOut)
	{
		VertTriSets.resize(NumVertexIDs);
		TotalVertTriCountOut = 0;
		for (int TriangleID = 0; TriangleID < NumTriangleIDs; ++TriangleID)
		{
			Index3i TriV;
			bool bIsTri = GetTriangleFunc(TriangleID, TriV);
			if (!bIsTri)
				continue;

			if (VertTriSets[TriV.A].AddValueUnique(TriangleID))
				TotalVertTriCountOut++;
			if (VertTriSets[TriV.B].AddValueUnique(TriangleID))
				TotalVertTriCountOut++;
			if (VertTriSets[TriV.C].AddValueUnique(TriangleID))
				TotalVertTriCountOut++;
		}
	}


	// build per-vertex triangle-one-rings from already-computed lists of exactly that
	void BuildVertexTriangles(
		int NumVertexIDs, 
		dynamic_buffer<InlineIndexList>& VertTriSets,
		size_t TotalVertTriCount,
		packed_int_lists& VertexTriangles)
	{
		VertexTriangles.Initialize(NumVertexIDs, 0, TotalVertTriCount);
		for (int VertexID = 0; VertexID < NumVertexIDs; ++VertexID) {
			if (VertTriSets[VertexID].Size() > 0)
				VertexTriangles.AppendList(VertexID, VertTriSets[VertexID].Size(), VertTriSets[VertexID].GetBuffer());
		}
	}


	// build per-vertex triangle-one-rings from edges information
	void BuildVertexTriangles(
		int NumVertexIDs,
		const packed_int_lists& VertexEdges,
		const unsafe_vector<MeshTopology::Edge>& Edges,
		packed_int_lists& VertexTriangles)
	{
		VertexTriangles.Initialize(NumVertexIDs, 0, VertexEdges.NumListElements());
		unsafe_vector<int> OneRingTrisTmp;
		for (int VertexID = 0; VertexID < NumVertexIDs; ++VertexID)
		{
			if (VertexEdges.HasList(VertexID) == false)
				continue;
			int NumEdges;
			const int* OneRingEdges = VertexEdges.GetListItemsUnsafe(VertexID, NumEdges);
			OneRingTrisTmp.reserve(NumEdges);
			int j = 0;
			for (int k = 0; k < NumEdges; ++k) {
				int eid = OneRingEdges[k];
				const MeshTopology::Edge& Edge = Edges[eid];

				if (Edge.TriInfo.A == -1)
				{
					// nonmanifold, todo
					gs_debug_assert(false);
				}
				else
				{
					OneRingTrisTmp.add_unique(Edge.TriInfo.A);
					if (Edge.TriInfo.B >= 0)
						OneRingTrisTmp.add_unique(Edge.TriInfo.B);
				}
			}

			VertexTriangles.AppendList(VertexID, (int)OneRingTrisTmp.size(), OneRingTrisTmp.raw_pointer());
			OneRingTrisTmp.clear();
		}
	}


	// build per-vertex vertex-one-rings from computed sets of per-vertex triangles
	void BuildVertexVertices(
		int NumVertexIDs, 
		FunctionRef<bool(int TriangleID, Index3i& TriVertices)> GetTriangleFunc,
		const dynamic_buffer<InlineIndexList>& VertTriSets, 
		packed_int_lists& VertexVertices,
		size_t VertTriCountHint)
	{
		VertexVertices.Initialize(NumVertexIDs, 0, VertTriCountHint);		// estimate is low if there are mesh borders...
		for (int VertexID = 0; VertexID < NumVertexIDs; ++VertexID)
		{
			const InlineIndexList& VertTris = VertTriSets[VertexID];
			int NumNbrTris = VertTris.Size();
			if (NumNbrTris == 0)
				continue;

			InlineIndexList16 NbrVerts;
			for (int k = 0; k < NumNbrTris; ++k)
			{
				Index3i TriV;
				bool bIsTri = GetTriangleFunc(VertTris[k], TriV);
				gs_debug_assert(bIsTri);
				if (TriV.A != VertexID)
					NbrVerts.AddValueUnique(TriV.A);
				if (TriV.B != VertexID)
					NbrVerts.AddValueUnique(TriV.B);
				if (TriV.C != VertexID)
					NbrVerts.AddValueUnique(TriV.C);
			}

			VertexVertices.AppendList(VertexID, NbrVerts.Size(), NbrVerts.GetBuffer());
		}
	}


	// build per-vertex vertex-one-rings from edges information
	void BuildVertexVertices(
		int NumVertexIDs,
		const packed_int_lists& VertexEdges,
		const unsafe_vector<MeshTopology::Edge>& Edges,
		packed_int_lists& VertexVertices)
	{
		VertexVertices.Initialize(NumVertexIDs, 0, VertexEdges.NumListElements());
		unsafe_vector<int> OneRingVertsTmp;
		for (int VertexID = 0; VertexID < NumVertexIDs; ++VertexID)
		{
			if (VertexEdges.HasList(VertexID) == false)
				continue;
			int NumEdges;
			const int* OneRingEdges = VertexEdges.GetListItemsUnsafe(VertexID, NumEdges);
			OneRingVertsTmp.resize(NumEdges);
			int j = 0;
			for (int k = 0; k < NumEdges; ++k) {
				int eid = OneRingEdges[k];
				const MeshTopology::Edge& Edge = Edges[eid];
				int VertB = Edge.Vertices.GetOtherValue(VertexID);
				gs_debug_assert(VertB != VertexID);
				OneRingVertsTmp[j++] = VertB;
			}

			VertexVertices.AppendList(VertexID, (int)OneRingVertsTmp.size(), OneRingVertsTmp.raw_pointer());
		}
	}



	void BuildTriNeighbours(
		int NumTriangleIDs,
		FunctionRef<bool(int TriangleID, Index3i& TriVertices)> GetTriangleFunc,
		const MeshTopology& Topology,
		unsafe_vector<Index3i>& TriNeighbours,
		packed_int_lists& NonManifoldTriTriLists)
	{
		gs_debug_assert(Topology.HasVertexEdges());

		TriNeighbours.initialize(NumTriangleIDs, Index3i(-1, -1, -1));

		for (int TriangleID = 0; TriangleID < NumTriangleIDs; ++TriangleID)
		{
			Index3i TriV;
			if (!GetTriangleFunc(TriangleID, TriV))
				continue;

			Index3i TriNbrs(-1, -1, -1);
			for (int j = 0; j < 3; ++j) {
				int VertA = TriV[j], VertB = TriV[(j + 1) % 3];
				int EdgeID = Topology.FindEdgeID(VertA, VertB);
				if (EdgeID < 0)
					continue;		// edge doesn't exist
				const MeshTopology::Edge& Edge = Topology.Edges[EdgeID];
				if (Edge.IsManifold() == false)
				{
					gs_debug_assert(false);
					// extract tri list and encode index here
					TriNbrs[j] = -2;
					continue;
				}

				int OtherT = Edge.TriInfo.GetOtherValue(TriangleID);
				gs_debug_assert(OtherT != TriangleID);
				TriNbrs[j] = OtherT;
			}

			TriNeighbours[TriangleID] = TriNbrs;
		}

	}


	void BuildVertexEdges(
		int NumVertexIDs, int NumTriangleIDs,
		FunctionRef<bool(int TriangleID, Index3i& TriVertices)> GetTriangleFunc,
		MeshTopology& Topology)
	{
		unsafe_vector<InlineIndexList> VertEdgeSets;
		VertEdgeSets.resize(NumVertexIDs);

		// todo it would be nice to have a variant of packed_int_lists that is based on linkedlist...
		unsafe_vector<InlineIndexList> NonManifoldTriLists;

		// look up edge in VertexEdgeSets
		auto FindEdgeID = [&](int VertexA, int VertexB) {
			if (VertexB < VertexA)
				GS::SwapTemp(VertexA, VertexB);
			InlineIndexList& VertEdgeList = VertEdgeSets[VertexA];
			int NumVertEdges = VertEdgeList.Size();
			const int* KnownEdges = VertEdgeList.GetBuffer();
			for (int j = 0; j < NumVertEdges; ++j) {
				int eid = KnownEdges[j];
				if (Topology.Edges[eid].Vertices.A == VertexA && Topology.Edges[eid].Vertices.B == VertexB)
					return eid;
			}
			return -1;
		};

		int TotalVertEdgeCount = 0;
		int TotalNonManifoldTriCount = 0;
		for (int TriangleID = 0; TriangleID < NumTriangleIDs; ++TriangleID)
		{
			Index3i TriVerts;
			if (!GetTriangleFunc(TriangleID, TriVerts))
				continue;
			if (TriVerts.A < 0 || TriVerts.B < 0 || TriVerts.C < 0)		// catch invalid triangles
				continue;

			for (int j = 0; j < 3; ++j)
			{
				int VertA = TriVerts[j], VertB = TriVerts[(j+1)%3];
				if ( VertB < VertA )
					GS::SwapTemp(VertA, VertB);
				int FoundEdgeID = FindEdgeID(VertA, VertB);
				if (FoundEdgeID >= 0)
				{
					MeshTopology::Edge& Edge = Topology.Edges[FoundEdgeID];
					if (Edge.TriInfo.A == -1) {
						int ListID = Edge.TriInfo.B;
						NonManifoldTriLists[ListID].AddValue(TriangleID);
						TotalNonManifoldTriCount++;
					}
					else if (Edge.TriInfo.B != -1) {
						// needs to become nonmanifold
						int ListID = (int)NonManifoldTriLists.size();
						NonManifoldTriLists.resize(NonManifoldTriLists.size() + 1);
						NonManifoldTriLists[ListID].AddValue(Edge.TriInfo.A);
						NonManifoldTriLists[ListID].AddValue(Edge.TriInfo.B);
						NonManifoldTriLists[ListID].AddValue(TriangleID);
						TotalNonManifoldTriCount += 3;
						Edge.TriInfo.A = -1;
						Edge.TriInfo.B = ListID;
					}
					else
						Edge.TriInfo.B = TriangleID;
				}
				else 
				{
					MeshTopology::Edge NewEdge{ Index2i(VertA, VertB), Index2i{TriangleID, -1} };
					int NewEdgeID = (int)Topology.Edges.add(NewEdge);
					gs_debug_assert(NewEdgeID < Topology.Edges.size());
					VertEdgeSets[VertA].AddValue(NewEdgeID);
					VertEdgeSets[VertB].AddValue(NewEdgeID);
					TotalVertEdgeCount += 2;
				}
			}
		}

		Topology.VertexEdges.Initialize(NumVertexIDs, 0, TotalVertEdgeCount);
		for (int VertexID = 0; VertexID < NumVertexIDs; ++VertexID) {
			if (VertEdgeSets[VertexID].Size() > 0)
				Topology.VertexEdges.AppendList(VertexID, VertEdgeSets[VertexID].Size(), VertEdgeSets[VertexID].GetBuffer());
		}

		if (NonManifoldTriLists.size() > 0)
		{
			Topology.NonManifoldEdgeTriLists.Initialize((int)NonManifoldTriLists.size(), 0, TotalNonManifoldTriCount);
			for (int ListID = 0; ListID < NonManifoldTriLists.size(); ++ListID)
				Topology.NonManifoldEdgeTriLists.AppendList(ListID, NonManifoldTriLists[ListID].Size(), NonManifoldTriLists[ListID].GetBuffer());
		}
	}


}



void MeshTopology::Build(
	int NumVertexIDs,
	FunctionRef<bool(int)> IsVertexValidFunc,
	int NumTriangleIDs,
	FunctionRef<bool(int TriangleID, Index3i& TriVertices)> GetTriangleFunc,
	EMeshTopologyTypes WhichParts)
{
	if (WhichParts & EMeshTopologyTypes::VertexEdges) {
		BuildVertexEdges(NumVertexIDs, NumTriangleIDs, GetTriangleFunc, *this);

		if (WhichParts & EMeshTopologyTypes::VertexTriangles)
			BuildVertexTriangles(NumVertexIDs, VertexEdges, Edges, VertexTriangles);
		if (WhichParts & EMeshTopologyTypes::VertexVertices)
			BuildVertexVertices(NumVertexIDs, VertexEdges, Edges, VertexVertices);
		if (WhichParts & EMeshTopologyTypes::TriangleNeighbours)
			BuildTriNeighbours(NumTriangleIDs, GetTriangleFunc, *this, this->TriNeighbours, this->NonManifoldTriTriLists);

	}
	else {
		dynamic_buffer<InlineIndexList> VertTriSets;
		size_t VertTriSetsTotalCount = 0;
		BuildPerVertexTriangleSets(NumVertexIDs, NumTriangleIDs, GetTriangleFunc, VertTriSets, VertTriSetsTotalCount);

		if (WhichParts & EMeshTopologyTypes::VertexTriangles)
			BuildVertexTriangles(NumVertexIDs, VertTriSets, VertTriSetsTotalCount, VertexTriangles);
		if (WhichParts & EMeshTopologyTypes::VertexVertices)
			BuildVertexVertices(NumVertexIDs, GetTriangleFunc, VertTriSets, VertexVertices, VertTriSetsTotalCount);
		
		// not clear that we can do this very efficiently w/o edges...
		if (WhichParts & EMeshTopologyTypes::TriangleNeighbours)
			gs_debug_assert(false);	
	}

}





int MeshTopology::FindEdgeID(int VertexA, int VertexB) const
{
	if (VertexB < VertexA)
		GS::SwapTemp(VertexA, VertexB);
	if (VertexEdges.HasList(VertexA) == false)
		return -1;
	int NumVertEdges = -1;
	const int* KnownEdges = VertexEdges.GetListItemsUnsafe(VertexA, NumVertEdges);
	for (int j = 0; j < NumVertEdges; ++j) {
		int eid = KnownEdges[j];
		if (Edges[eid].Vertices.A == VertexA && Edges[eid].Vertices.B == VertexB)
			return eid;
	}
	return -1;
}


bool MeshTopology::CheckValidity() const
{
	if (VertexEdges.NumLists() > 0) 
	{
		int NumV = VertexEdges.NumLists(), NumE = (int)Edges.size();
		for (int VertexID = 0; VertexID < NumV; ++VertexID) {
			if (VertexEdges.HasList(VertexID)) {
				int EdgeCount = 0;
				const int* VertEdges = VertexEdges.GetListItemsUnsafe(VertexID, EdgeCount);
				for (int k = 0; k < EdgeCount; ++k) 
					gs_debug_assert(VertEdges[k] >= 0 && VertEdges[k] < NumE);
			}
		}
	}

	return true;
}