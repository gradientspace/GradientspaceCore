// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/dynamic_buffer.h"
#include "Math/GSVector3.h"

#include <array>
#include <atomic>

namespace GS
{

template<int DimensionX, int DimensionY, int DimensionZ>
class FixedGridBase3
{
public:
	static constexpr Vector3i TypeDimensions() { return Vector3i(DimensionX, DimensionY, DimensionZ); }

public:
	bool IsValidIndex(const Vector3i& Index) const
	{
		return Index.X >= 0 && Index.Y >= 0 && Index.Z >= 0
			&& Index.X < DimensionX && Index.Y < DimensionY && Index.Z < DimensionZ;
	}

	Vector3i Dimensions() const
	{
		return Vector3i(DimensionX, DimensionY, DimensionZ);
	}

	size_t Size() const
	{
		return DimensionX * DimensionY * DimensionZ;
	}

	constexpr int64_t ToLinearIndex(const Vector3i& Index) const
	{
		return ((Index.Z * DimensionY + Index.Y) * DimensionX) + Index.X;
	}
	constexpr int64_t ToLinearIndex(int x, int y, int z) const
	{
		return ((z * DimensionY + y) * DimensionX) + x;
	}
	constexpr Vector3i ToVectorIndex(int64_t LinearIndex) const
	{
		return Vector3i( (int)(LinearIndex % DimensionX), 
						 (int)((LinearIndex / DimensionX) % DimensionY), 
						 (int)(LinearIndex / (DimensionX * DimensionY)) );
	}

};




template<typename ElemType, int DimensionX, int DimensionY, int DimensionZ>
class FixedGrid3 : public FixedGridBase3<DimensionX,DimensionY,DimensionZ>
{
public:
	using BaseType = typename FixedGridBase3<DimensionX, DimensionY, DimensionZ>;

	dynamic_buffer<ElemType> Data;

public:

	// todo: constructors that initialize data, move constructors

	void Initialize()
	{
		Data.resize(DimensionX * DimensionY * DimensionZ);
	}

	void Initialize(const ElemType& InitialValue)
	{
		Data.resize_init(DimensionX * DimensionY * DimensionZ, InitialValue, false);
	}

	void SetAll(const ElemType& InitialValue)
	{
		int64_t N = (int64_t)Data.size();
		for (int64_t k = 0; k < N; ++k) 
			Data.set_value(k, InitialValue);
	}

	template<typename Func>
	void SetFromMapped(Func GetValueFromLinearIndexFunc)
	{
		int64_t N = (int64_t)Data.size();
		for (int64_t k = 0; k < N; ++k) {
			ElemType Value = GetValueFromLinearIndexFunc(k);
			Data.set_value(k, Value);
		}
	}

	constexpr ElemType& operator[](int64_t LinearIndex)
	{
		return Data[LinearIndex];
	}
	constexpr const ElemType& operator[](int64_t LinearIndex) const
	{
		return Data[LinearIndex];
	}

	constexpr ElemType& operator[](const Vector3i& VecIndex)
	{
		return Data[BaseType::ToLinearIndex(VecIndex)];
	}
	constexpr const ElemType& operator[](const Vector3i& VecIndex) const
	{
		return Data[BaseType::ToLinearIndex(VecIndex)];
	}


	constexpr const ElemType& Get(const Vector3i& VecIndex) const
	{
		return Data.get_value(BaseType::ToLinearIndex(VecIndex));
	}
	constexpr const ElemType& Get(int x, int y, int z) const
	{
		return Data.get_value(BaseType::ToLinearIndex(x, y, z));
	}

	constexpr ElemType& GetRef(const Vector3i& VecIndex)
	{
		return Data.get_value_ref(BaseType::ToLinearIndex(VecIndex));
	}
	constexpr ElemType& GetRef(int x, int y, int z)
	{
		return Data.get_value_ref(BaseType::ToLinearIndex(x, y, z));
	}


	void Set(int x, int y, int z, const ElemType& NewValue)
	{
		return Data.set_value(BaseType::ToLinearIndex(x, y, z), NewValue);
	}
	void Set(const Vector3i& VecIndex, const ElemType& NewValue)
	{
		return Data.set_value(BaseType::ToLinearIndex(VecIndex), NewValue);
	}
	void Set(int64_t LinearIndex, const ElemType& NewValue)
	{
		return Data.set_value(LinearIndex, NewValue);
	}

	// EnumerateFunc takes arguments (size_t LinearIndex, const ElemType& Element)
	template<typename EnumerateFunc>
	void EnumerateAllCells(EnumerateFunc Func) const
	{
		size_t MaxIndex = BaseType::Size();
		for (size_t LinearIndex = 0; LinearIndex < MaxIndex; ++LinearIndex)
		{
			Func(LinearIndex, Data.get_value(LinearIndex));
		}
	}
};









} // end namespace GS
