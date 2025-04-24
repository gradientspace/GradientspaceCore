// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "Core/unsafe_vector.h"
#include "Math/GSIndex3.h"
#include "Math/GSIndex4.h"


namespace GS 
{

template<typename ElementType>
struct IndexedAttributeBase
{
	uint8_t NumSets = 0;
	Index4i SetCounts = Index4i::Zero();
	Index4i SetOffsets = Index4i::Zero();
	unsafe_vector<ElementType> Values;

	void ClearBase()
	{
		NumSets = 0;
		SetCounts = Index4i::Zero();
		SetOffsets = Index4i::Zero();
		Values.clear();
	}

	int GetNumSets() const {
		return NumSets;
	}

	int AddSet(size_t NumElements)
	{
		if (NumSets == 4)
		{
			gs_runtime_assert(false);
			return -1;
		}
		int NewSetIndex = NumSets;
		NumSets++;
		SetCounts[NewSetIndex] = (int)NumElements;
		SetOffsets[NewSetIndex] = (int)Values.size();
		Values.grow(NumElements);
		return NewSetIndex;
	}

	//! add an element to the last set, and return it's index, in that set
	int AppendElementToLastSet(const ElementType& Value)
	{
		uint8_t LastSetIndex = NumSets - 1;
		SetCounts[LastSetIndex]++;
		int NewIndex = (int)Values.add(Value);
		return NewIndex - SetOffsets[LastSetIndex];
	}

	//! add num elements to last set. return index of first new element, in that set.
	int AppendElementsToLastSet(int NumNewElements)
	{
		uint8_t LastSetIndex = NumSets - 1;
		SetCounts[LastSetIndex] += NumNewElements;
		int StartIndex = (int)Values.size() - SetOffsets[LastSetIndex];
		Values.grow(NumNewElements);
		return StartIndex;
	}


	int GetElementCount(int SetIndex) const
	{
		gs_debug_assert(SetIndex >= 0 && SetIndex < NumSets);
		return SetCounts[SetIndex];
	}

	const ElementType& GetElement(int ElementIndex, int SetIndex = 0) const {
		gs_debug_assert(SetIndex >= 0 && SetIndex < NumSets && ElementIndex >= 0 && ElementIndex < SetCounts[SetIndex]);
		int Index = SetOffsets[SetIndex] + ElementIndex;
		return Values[Index];
	}

	void SetElement(int ElementIndex, const ElementType& NewValue, int SetIndex = 0) {
		gs_debug_assert(SetIndex >= 0 && SetIndex < NumSets && ElementIndex >= 0 && ElementIndex < SetCounts[SetIndex]);
		int Index = SetOffsets[SetIndex] + ElementIndex;
		Values[Index] = NewValue;
	}
};

template<typename ElementType>
struct IndexedPolyMeshAttribute : public IndexedAttributeBase<ElementType>
{
	using IndexedAttributeBase<ElementType>::NumSets;

	unsafe_vector<Index3i> Triangles;
	unsafe_vector<Index4i> Quads;

	void Clear()
	{
		IndexedAttributeBase<ElementType>::ClearBase();
		Triangles.clear();
		Quads.clear();
	}

	void AddTriangle(const Index3i* AllElementTriangles,
					 bool bSkipInitialization)
	{
		int start_index = (int)Triangles.grow(NumSets);
		if (AllElementTriangles)
		{
			for (int j = 0; j < NumSets; ++j)
				Triangles[start_index + j] = AllElementTriangles[j];
		}
		else if (!bSkipInitialization) 
		{
			for (int j = 0; j < NumSets; ++j)
				Triangles[start_index + j] = Index3i(-1);
		}
	}

	void SetTriangle(int TriangleIndex, const Index3i* ElementTriangles, int SingleSetIndex = -1)
	{
		if (SingleSetIndex == -1) {
			for (int j = 0; j < NumSets; ++j)
				Triangles[TriangleIndex*NumSets + j] = ElementTriangles[j];
		}
		else
			Triangles[TriangleIndex*NumSets + SingleSetIndex] = ElementTriangles[0];
	}
	void SetTriangle(int TriangleIndex, const Index3i& ElementTriangle, int SingleSetIndex)
	{
		Triangles[TriangleIndex*NumSets + SingleSetIndex] = ElementTriangle;
	}

	void AddQuad(const Index4i* AllElementQuads,
		bool bSkipInitialization)
	{
		int start_index = (int)Quads.grow(NumSets);
		if (AllElementQuads)
		{
			for (int j = 0; j < NumSets; ++j)
				Quads[start_index + j] = AllElementQuads[j];
		}
		else if (!bSkipInitialization)
		{
			for (int j = 0; j < NumSets; ++j)
				Quads[start_index + j] = Index4i(-1);
		}
	}

	void SetQuad(int QuadIndex, const Index4i* ElementQuads, int SingleSetIndex = -1)
	{
		if (SingleSetIndex == -1) {
			for (int j = 0; j < NumSets; ++j)
				Quads[QuadIndex*NumSets + j] = ElementQuads[j];
		}
		else
			Quads[QuadIndex*NumSets + SingleSetIndex] = ElementQuads[0];
	}
	void SetQuad(int QuadIndex, const Index4i& ElementQuad, int SingleSetIndex)
	{
		Quads[QuadIndex*NumSets + SingleSetIndex] = ElementQuad;
	}

	int GetFaceVertexElementIndex(int FaceType, int FaceTypeIndex, int FaceVertexIndex, int SetIndex = 0) const
	{
		if (FaceType == 0) {
			return Triangles[FaceTypeIndex*NumSets + SetIndex][FaceVertexIndex];
		}
		else if (FaceType == 1) {
			return Quads[FaceTypeIndex*NumSets + SetIndex][FaceVertexIndex];
		}
		return -1;
	}

	bool GetFaceElementIndices(int FaceType, int FaceTypeIndex, int* IndexBuffer, int SetIndex = 0) const
	{
		if (FaceType == 0) {
			const Index3i& Tri = Triangles[FaceTypeIndex*NumSets + SetIndex];
			IndexBuffer[0] = Tri.A; IndexBuffer[1] = Tri.B; IndexBuffer[2] = Tri.C;
			return true;
		}
		else if (FaceType == 1) {
			const Index4i& Quad = Quads[FaceTypeIndex*NumSets + SetIndex];
			IndexBuffer[0] = Quad.A; IndexBuffer[1] = Quad.B; IndexBuffer[2] = Quad.C; IndexBuffer[3] = Quad.D;
			return true;
		}
		return false;
	}
};



} // end namespace GS
