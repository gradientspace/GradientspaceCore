// Copyright Gradientspace Corp. All Rights Reserved.
#include "Core/packed_int_lists.h"

using namespace GS;

void packed_int_lists::Initialize(int NumListIDs, int ListSizeEstimate, size_t KnownExactTotalListItems)
{
	ListPointers.initialize(NumListIDs, -1);
	PackedLists.resize(0);
	if (KnownExactTotalListItems > 0)
		PackedLists.reserve(KnownExactTotalListItems + (size_t)NumListIDs);
	else if (ListSizeEstimate > 0)
		PackedLists.reserve(NumListIDs * (ListSizeEstimate+1));
}

bool packed_int_lists::AppendList(int ListID, const_buffer_view<int> Items)
{
	gs_debug_assert(ListPointers[ListID] == -1);
	if (ListPointers[ListID] != -1)
		return false;
	int NumItems = (int)Items.size();
	if (NumItems == 0)
		return false;

	PackedLists.reserve(PackedLists.size() + NumItems + 1);
	ListPointers[ListID] = (int)PackedLists.size();
	PackedLists.add(NumItems);
	for (int k = 0; k < NumItems; ++k)
		PackedLists.add(Items[k]);

	return true;
}

bool packed_int_lists::AppendList(int ListID, int NumItems, const int* Items)
{
	return AppendList(ListID, const_buffer_view<int>(Items, NumItems));
}

int packed_int_lists::AppendList(int NumItems, const int* Items)
{
	if (NumItems == 0 || Items == nullptr)
		return -1;
	int NewListID = (int)ListPointers.add(-1);
	AppendList(NewListID, const_buffer_view<int>(Items, NumItems));
	return (int)NewListID;
}


const_buffer_view<int> packed_int_lists::GetListView(int ListID) const
{
	int ListIndex = ListPointers[ListID];
	if (ListIndex < 0)
		return const_buffer_view<int>();
	const int* ptr = PackedLists.raw_pointer(ListIndex);
	return const_buffer_view<int>(ptr + 1, ptr[0]);
}
