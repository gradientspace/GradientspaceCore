// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Grid/GSFixedGrid3.h"

namespace GS
{

template<typename ElemType, int DimensionX, int DimensionY, int DimensionZ>
class AtomicFixedGrid3 : public FixedGridBase3<DimensionX, DimensionY, DimensionZ>
{
public:
	using BaseType = typename FixedGridBase3<DimensionX, DimensionY, DimensionZ>;

	std::array<std::atomic<ElemType>, DimensionX* DimensionY* DimensionZ> Data;

public:

	void SetAll(const ElemType& InitialValue)
	{
		for (size_t k = 0; k < DimensionX * DimensionY * DimensionZ; ++k)
			Data[k].store(InitialValue);
	}
	void Initialize(const ElemType& InitialValue)
	{
		// atomic grid allocates on construction
		SetAll(InitialValue);
	}

	constexpr ElemType operator[](int64_t LinearIndex) const
	{
		return Data[LinearIndex].load();
	}
	constexpr ElemType operator[](const Vector3i& VecIndex) const
	{
		return Data[BaseType::ToLinearIndex(VecIndex)].load();
	}

	constexpr const ElemType Get(int64_t LinearIndex) const
	{
		return Data[LinearIndex].load();
	}
	constexpr const ElemType Get(const Vector3i& VecIndex) const
	{
		return Data[BaseType::ToLinearIndex(VecIndex)].load();
	}

	void Set(const Vector3i& VecIndex, const ElemType& NewValue)
	{
		return Data[BaseType::ToLinearIndex(VecIndex)].store(NewValue);
	}
	void Set(int64_t LinearIndex, const ElemType& NewValue)
	{
		return Data[LinearIndex].store(NewValue);
	}

};






template<int DimensionX, int DimensionY, int DimensionZ>
class AtomicFixedFlagGrid3 : public FixedGridBase3<DimensionX, DimensionY, DimensionZ>
{
public:
	using BaseType = typename FixedGridBase3<DimensionX, DimensionY, DimensionZ>;

	std::array<std::atomic_flag, DimensionX* DimensionY* DimensionZ> Data;

public:

	void SetAll(bool bSetValue)
	{
		if (bSetValue)
		{
			for (size_t k = 0; k < DimensionX * DimensionY * DimensionZ; ++k)
				Data[k].test_and_set();
		}
		else
		{
			for (size_t k = 0; k < DimensionX * DimensionY * DimensionZ; ++k)
				Data[k].clear();
		}
	}

};




}
