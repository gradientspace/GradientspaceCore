// Copyright Gradientspace Corp. All Rights Reserved.
#include "Grid/GSSubGridMask2.h"

using namespace GS;

void SubGridMask2::Initialize(int GridWidth, int GridHeight, int BlocksDimIn, uint8_t InitialMask)
{
	GridDimensions = Vector2i(GridWidth, GridHeight);
	BlocksDim = BlocksDimIn;
	NumBlocksWidth = (GridWidth / BlocksDim);
	if ((NumBlocksWidth * BlocksDim) < GridWidth)
		NumBlocksWidth++;
	NumBlocksHeight = (GridHeight / BlocksDim);
	if ((NumBlocksHeight * BlocksDim) < GridHeight)
		NumBlocksHeight++;
	Cells.resize(NumBlocksWidth * NumBlocksHeight);

	MarkAll(InitialMask);
}

SubGridMask2::~SubGridMask2() 
{
	Cells.clear(true);
}


void SubGridMask2::EnumerateMaskedRegions(
	FunctionRef<void(const GS::AxisBox2i& Region)> RegionFunc,
	uint8_t AndMask) const
{
	auto EmitBox = [&](GS::AxisBox2i Box) {
		Box.Max.X = GS::Min(Box.Max.X, GridDimensions.X - 1);
		Box.Max.Y = GS::Min(Box.Max.Y, GridDimensions.Y - 1);
		RegionFunc(Box);
	};

	for (int yi = 0; yi < NumBlocksHeight; ++yi) {
		int ri = yi * NumBlocksWidth;
		AxisBox2i CurAccumBox = AxisBox2i::Empty();
		for (int xi = 0; xi < NumBlocksWidth; ++xi) {
			int cellidx = ri + xi;
			if ( (Cells[cellidx].Mask & AndMask) != 0 ) {
				AxisBox2i GridRegion = BlockToGridSubRegion(Vector2i(xi, yi));
				CurAccumBox.Contain(GridRegion);
			}
			else
			{
				if (CurAccumBox.AreaCount() > 0) {
					EmitBox(CurAccumBox);
					CurAccumBox = GS::AxisBox2i::Empty();
				}
			}
		}
		if (CurAccumBox.AreaCount() > 0) {
			EmitBox(CurAccumBox);
			CurAccumBox = GS::AxisBox2i::Empty();
		}
	}

}