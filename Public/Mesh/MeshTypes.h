// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSIndex3.h"
#include "Color/GSIntColor4.h"
#include "Core/unsafe_vector.h"
#include "Core/buffer_view.h"

#include <array>

namespace GS
{

template<typename ValueType>
struct TTriVtxTuple
{
	// todo maybe this should be ValueType[3] ?  can we guarantee dense packing of all types here??
	ValueType A;
	ValueType B;
	ValueType C;

	ValueType& operator[](int index) {
		return static_cast<ValueType*>(&A)[index];
	}

	ValueType operator[](int index) const {
		return static_cast<const ValueType*>(&A)[index];
	}

	TTriVtxTuple() {}
	TTriVtxTuple(const ValueType& DefaultValue) { A = B = C = DefaultValue; }
	TTriVtxTuple(const ValueType& a, const ValueType& b, const ValueType& c) {
		A = a; B = b; C = c;
	}
};
typedef TTriVtxTuple<Vector3f> TriVtxNormals;
typedef TTriVtxTuple<Vector2f> TriVtxUVs;
typedef TTriVtxTuple<Color4b> TriVtxColors;
typedef TTriVtxTuple<Vector3d> TriVtxPositions;



template<typename ValueType>
struct TQuadVtxTuple
{
	// todo maybe this should be ValueType[4] ?  can we guarantee dense packing of all types here??
	ValueType A;
	ValueType B;
	ValueType C;
	ValueType D;

	ValueType& operator[](int index) {
		return static_cast<ValueType*>(&A)[index];
	}

	ValueType operator[](int index) const {
		return static_cast<const ValueType*>(&A)[index];
	}

	TQuadVtxTuple() {}
	TQuadVtxTuple(const ValueType& DefaultValue) { A = B = C = D = DefaultValue; }
	TQuadVtxTuple(const ValueType& a, const ValueType& b, const ValueType& c, const ValueType& d) {
		A = a; B = b; C = c; D = d;
	}
};
typedef TQuadVtxTuple<Vector3f> QuadVtxNormals;
typedef TQuadVtxTuple<Vector2f> QuadVtxUVs;
typedef TQuadVtxTuple<Color4b> QuadVtxColors;
typedef TQuadVtxTuple<Vector3d> QuadVtxPositions;






template<typename ValueType, int MaxInlineValues>
class TInlineSmallList
{
protected:
	std::array<ValueType, MaxInlineValues> InlineValues;
	ValueType* HeapValues = nullptr;
	ValueType* UsingValues = nullptr;		// could get rid of this and shrink size of struct by 8 bytes...but branch at every access...
	int32_t NumValues = 0;
	int32_t HeapAllocated = 0;
public:
	TInlineSmallList() {
		SetSize(0);
	}
	TInlineSmallList(size_t Count) {
		SetSize(Count);
	}
	~TInlineSmallList() {
		if (HeapAllocated > 0) 
			delete [] HeapValues;
	}

	TInlineSmallList(const TInlineSmallList& copy) {
		*this = copy;
	}
	TInlineSmallList(TInlineSmallList&& moved) {
		*this = std::move(moved);
	}
	TInlineSmallList& operator=(const TInlineSmallList& copy)
	{
		int N = copy.Size();
		SetSize(N);
		for (int k = 0; k < N; ++k)
			UsingValues[k] = copy.UsingValues[k];
		return *this;
	}
	TInlineSmallList& operator=(TInlineSmallList&& moved) {
		InlineValues = moved.InlineValues;
		HeapValues = moved.HeapValues;
		NumValues = moved.NumValues;
		HeapAllocated = moved.HeapAllocated;
		UsingValues = (NumValues >= MaxInlineValues) ? HeapValues : &InlineValues[0];

		moved.HeapValues = nullptr;
		moved.HeapAllocated = 0;
		moved.NumValues = 0;
		moved.UsingValues = &moved.InlineValues[0];
	}


	void SetSize(size_t Count)
	{
		if (Count >= MaxInlineValues) {
			set_heap_size((int)Count);
			UsingValues = HeapValues;
		} else
			UsingValues = &InlineValues[0];
		NumValues = (int)Count;
	}

	int Size() const {
		return NumValues;
	}

	ValueType& operator[](int index) {
		return UsingValues[index];
	}

	ValueType operator[](int index) const {
		return UsingValues[index];
	}

	ValueType& operator[](size_t index) {
		return UsingValues[index];
	}

	ValueType operator[](size_t index) const {
		return UsingValues[index];
	}

	ValueType* GetBuffer() {
		return UsingValues;
	}
	const ValueType* GetBuffer() const {
		return UsingValues;
	}

	const_buffer_view<ValueType> GetBufferView() const {
		return const_buffer_view<ValueType>(UsingValues, NumValues);
	}

	void SetValues3(const ValueType& A, const ValueType& B, const ValueType& C) {
		InlineValues[0] = A; InlineValues[1] = B; InlineValues[2] = C;
		UsingValues = &InlineValues[0];
		NumValues = (int)3;
	}
	void SetValues4(const ValueType& A, const ValueType& B, const ValueType& C, const ValueType& D) {
		InlineValues[0] = A; InlineValues[1] = B; InlineValues[2] = C; InlineValues[3] = D;
		UsingValues = &InlineValues[0];
		NumValues = (int)4;
	}

	void AddValue(const ValueType& NewValue)
	{
		if (NumValues < MaxInlineValues) {
			InlineValues[NumValues] = NewValue;
			NumValues++;
		} else {
			add_heap(NewValue);
			UsingValues = HeapValues;
			NumValues++;
		}
	}
	void AddValues3(const ValueType& A, const ValueType& B, const ValueType& C) {	
		// todo be smarter
		AddValue(A); AddValue(B); AddValue(C);
	}
	void AddValues4(const ValueType& A, const ValueType& B, const ValueType& C, const ValueType& D) {
		// todo be smarter
		AddValue(A); AddValue(B); AddValue(C); AddValue(D);
	}

	void push_back(const ValueType& NewValue) {
		AddValue(NewValue);
	}
	bool pop_back(ValueType& Out)
	{
		if (NumValues == 0) return false;
		Out = UsingValues[NumValues - 1];
		NumValues--;
		return true;
	}

	bool ContainsValue(const ValueType& Val) const {
		for (int j = 0; j < NumValues; ++j)
			if (UsingValues[j] == Val)
				return true;
		return false;
	}

	template<typename CompareFunc>
	bool ContainsValue(const ValueType& Val, CompareFunc CompareF) const {
		for (int j = 0; j < NumValues; ++j)
			if (CompareF(UsingValues[j], Val))
				return true;
		return false;
	}

	bool AddValueUnique(const ValueType& NewValue)
	{
		if (ContainsValue(NewValue))
			return false;
		AddValue(NewValue);
		return true;
	}

	template<typename CompareFunc>
	bool AddValueUnique(const ValueType& NewValue, CompareFunc Func)
	{
		if (ContainsValue(NewValue, Func))
			return false;
		AddValue(NewValue);
		return true;
	}

protected:
	void set_heap_size(int new_heap_alloc_size)
	{
		ValueType* new_heap_values = new ValueType[new_heap_alloc_size];
		for (int k = 0; k < NumValues; ++k)
			new_heap_values[k] = UsingValues[k];
		if (UsingValues == HeapValues)
			UsingValues = new_heap_values;
		if (HeapValues != nullptr)
			delete[] HeapValues;
		HeapValues = new_heap_values;
		HeapAllocated = new_heap_alloc_size;
	}
	void add_heap(const ValueType& NewValue)
	{
		if ( NumValues >= HeapAllocated )
			set_heap_size(NumValues + MaxInlineValues);
		HeapValues[NumValues] = NewValue;
	}
};
typedef TInlineSmallList<int, 8> InlineIndexList;
typedef TInlineSmallList<Vector3d, 8> InlineVec3dList;
typedef TInlineSmallList<Vector2d, 8> InlineVec2dList;
typedef TInlineSmallList<Vector2f, 8> InlineVec2fList;
typedef TInlineSmallList<Index3i, 8> InlineIndex3List;

typedef TInlineSmallList<int, 16> InlineIndexList16;
typedef TInlineSmallList<int, 32> InlineIndexList32;


} // end namespace GS
