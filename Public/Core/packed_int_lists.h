// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Core/buffer_view.h"


namespace GS
{

/**
 * packed_int_lists stores a list of small lists of integers, useful for (eg) storing neighbourhood information
 * in static index-based data structures like meshes. For example the one-ring triangle neighbours of a vertex, etc.
 * The data structure is meant to be constructed and then queried, it cannot be dynamically modified.
 * Each list is added via a call to AppendList(), which appends the provided set of items.
 *
 * There are two usage modes: 
 *  1) call Initialize() to set a fixed # of lists, and then AppendList with a specified ListID (index)
 *  2) call AppendList() without a ListID to append a new list (ie resize) and then return the new ListID
 * 
 */
class GRADIENTSPACECORE_API packed_int_lists
{
public:
	// indices into PackedLists
	unsafe_vector<int> ListPointers;
	// sequential packed lists, each list starts with list size and then elements
	unsafe_vector<int> PackedLists;

	int NumLists() const { return (int)ListPointers.size(); }
	int64_t NumListElements() const { return PackedLists.size(); }

	//! initialize with a fixed number of known ListIDs
	void Initialize(int NumListIDs, int ListSizeEstimate = 0, size_t KnownExactTotalListItems = 0);

	//! append a (non-empty) list at a given ListID. returns false if ListID's list is non-empty, or on invalid input.
	bool AppendList(int ListID, const_buffer_view<int> Items);

	//! append a (non-empty) list at a given ListID. returns false if ListID's list is non-empty, or on invalid input.
	bool AppendList(int ListID, int NumItems, const int* Items);

	//! append a (non-empty) list at a new ListID. returns new ListID, or -1 on invalid input.
	int AppendList(int NumItems, const int* Items);

	bool HasList(int ListID) const { return ListPointers[ListID] >= 0; }

	int GetListSizeUnsafe(int ListID) const { 
		return PackedLists[ListPointers[ListID]]; 
	}
	
	const int* GetListItemsUnsafe(int ListID) const { 
		return PackedLists.raw_pointer(ListPointers[ListID] + 1); 
	}

	const int* GetListItemsUnsafe(int ListID, int& SizeOut) const {
		const int* ptr = PackedLists.raw_pointer(ListPointers[ListID]);
		SizeOut = *ptr;
		return (ptr + 1);
	}

	const_buffer_view<int> GetListView(int ListID) const;
};




} // end namespace GS