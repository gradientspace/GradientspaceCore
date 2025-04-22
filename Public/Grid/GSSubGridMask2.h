// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Core/FunctionRef.h"
#include "Math/GSIntVector2.h"
#include "Math/GSIntAxisBox2.h"


namespace GS
{


class GRADIENTSPACECORE_API SubGridMask2
{
public:
	//! dimensions of external outer grid
	Vector2i GridDimensions;
	//! dimensions of subgrid blocks (same width/height)
	int BlocksDim;
	//! number of blocks in width/X dimension
	int NumBlocksWidth;
	//! number of blocks in height/Y dimension
	int NumBlocksHeight;

	struct GridCell
	{
		uint8_t Mask;
	};

	//! allocated blocks
	unsafe_vector<GridCell> Cells;

	~SubGridMask2();

	void Initialize(int GridWidth, int GridHeight, int BlocksDimIn, uint8_t InitialMask = 0x0);

	inline Vector2i GridToBlock(Vector2i GridCoords) const {
		return Vector2i(GridCoords.X / BlocksDim, GridCoords.Y / BlocksDim);
	}
	inline int BlockToIndex(Vector2i BlockCoords) const {
		return BlockCoords.Y * NumBlocksWidth + BlockCoords.X;
	}
	inline Vector2i IndexToBlock(int Index) const {
		int Y = Index / NumBlocksWidth;
		return Vector2i(Index - Y * NumBlocksWidth, Y);
	}
	inline Vector2i BlockOriginToGrid(Vector2i BlockCoords) const {
		return Vector2i(BlockCoords.X * BlocksDim, BlockCoords.Y * BlocksDim);
	}

	inline GS::AxisBox2i BlockToGridSubRegion(Vector2i BlockCoords) const {
		Vector2i Origin = BlockOriginToGrid(BlockCoords);
		return GS::AxisBox2i(Origin, Vector2i(Origin.X + BlocksDim - 1, Origin.Y + BlocksDim - 1));
	}
	inline GS::AxisBox2i IndexToGridSubRegion(int Index) const {
		return BlockToGridSubRegion(IndexToBlock(Index));
	}

	inline void MarkAll(uint8_t SetValue) {
		for (GridCell& Cell : Cells)
			Cell.Mask = SetValue;
	}

	void MarkCell(Vector2i GridCoords, uint8_t SetValue = 0xFF) {
		Vector2i BlockCellIdx = GridToBlock(GridCoords);
		int Index = BlockToIndex(BlockCellIdx);
		GridCell& Cell = Cells[Index];
		Cell.Mask = SetValue;
	}

	void EnumerateMaskedRegions(
		FunctionRef<void(const GS::AxisBox2i& Region)> RegionFunc,
		uint8_t AndMask = 0xFF
	) const;
};


}
