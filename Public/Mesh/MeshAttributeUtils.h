// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Core/dynamic_buffer.h"
#include "Math/GSIndex2.h"
#include "Math/GSIndex3.h"
#include "Math/GSIndex4.h"

#include <unordered_map>
#include <vector>

namespace GS
{


/**
 * AttributeCompressor can be used to extract a unique set AttribType values
 * from an external list of values that might contain repeats. For example
 * if per-triangle colors are given, they will be compressed into a unique
 * list with indexing. 
 */
template<typename AttribType>
struct AttributeCompressor
{
	std::unordered_map<AttribType, int> Index;
	GS::unsafe_vector<AttribType> UniqueValues;

	int GetIndexForValue(const AttribType& Value, bool bAddIfMissing)
	{
		auto found = Index.find(Value);
		if (found == Index.end())
		{
			if (bAddIfMissing == false) return -1;

			int NewIndex = (int)UniqueValues.size();
			UniqueValues.add(Value);
			Index.insert({ Value, NewIndex });
			return NewIndex;
		}
		return found->second;
	}

	void InsertValue(const AttribType& Value)
	{
		GetIndexForValue(Value, true);
	}
};


/**
 * AttributeVertexCombiner blends multiple attribute values at a vertex
 * into a single value.
 */
template<typename AttribType>
struct AttributeVertexCombiner
{
	struct CombinedValue
	{
		AttribType Value;
		double WeightSum = 0;
	};
	std::vector<CombinedValue> VertexValues;

	void Initialize(int NumVertices, const AttribType& InitialValue)
	{
		VertexValues.resize(NumVertices);
		for (int k = 0; k < NumVertices; ++k)
			VertexValues[k] = { InitialValue, 0.0 };
	}

	void AccumulateValue(int VertexIndex, const AttribType& Value, double Weight = 1.0)
	{
		CombinedValue& C = VertexValues[VertexIndex];
		C.Value += Value;
		C.WeightSum += Weight;
	}

	void NormalizeAllValues()
	{
		size_t N = VertexValues.size();
		for (size_t i = 0; i < N; ++i)
		{
			CombinedValue& C = VertexValues[i];
			if (C.WeightSum > 0)
			{
				C.Value /= C.WeightSum;
				C.WeightSum = 1.0;
			}
		}
	}

	AttribType GetVertexValue(int VertexIndex) const
	{
		const CombinedValue& C = VertexValues[VertexIndex];
		return (C.WeightSum > 0) ? (C.Value / C.WeightSum) : C.Value;
	}
};






template<typename AttribType>
struct VertexElementMapper
{
	std::unordered_map<Index2i, int> Index;
	GS::unsafe_vector<AttribType> Values;

	int FindOrInsertValue(int ElementID, int VertexID, const AttribType& ElementValue)
	{
		Index2i Key(VertexID, ElementID);
		auto found = Index.find(Key);
		if (found == Index.end())
		{
			int NewIndex = (int)Values.size();
			Values.add(ElementValue);
			Index.insert({ Key,NewIndex });
			return NewIndex;
		}
		return found->second;
	}

	int FindIndex(int ElementID, int VertexID) const
	{
		auto found = Index.find(Index2i(VertexID, ElementID));
		if (found == Index.end())
			return -1;
		return found->second;
	}
};



}
