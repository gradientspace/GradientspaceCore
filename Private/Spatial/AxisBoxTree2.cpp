// Copyright Gradientspace Corp. All Rights Reserved.
#include "Spatial/AxisBoxTree2.h"
#include "Mesh/MeshTypes.h"

using namespace GS;

//#include <vector>

template<typename RealType>
struct TSourceBox
{
	AxisBox2<RealType> Box;
	int32_t BoxID;
};

template<typename RealType>
struct TBoxGroup
{
	AxisBox2<RealType> Bounds;
	int32_t StartIndex = -1;
	int32_t EndIndex = -1;		// inclusive
	Index2i Children = Index2i(-1,-1);
	int32_t Count() const { return EndIndex - StartIndex + 1; }
	bool IsLeaf() const { return Children.A == -1; }
};


template<typename RealType>
int select_split_axis_longest(const AxisBox2<RealType>& Box)
{
	RealType W = Box.WidthX();
	RealType H = Box.HeightY();
	return (W >= H) ? 0 : 1;
}


template<typename RealType>
void split_group_trivial(
	unsafe_vector<TSourceBox<RealType>>& AllBoxes,
	const TBoxGroup<RealType>& Group,
	TBoxGroup<RealType>& LeftChild,
	TBoxGroup<RealType>& RightChild)
{
	gs_debug_assert(Group.Count() > 1);

	LeftChild.StartIndex = Group.StartIndex;
	LeftChild.EndIndex = Group.StartIndex + (Group.Count() / 2);
	LeftChild.Bounds = AxisBox2<RealType>::Empty();
	for (int32_t k = LeftChild.StartIndex; k <= LeftChild.EndIndex; ++k)
		LeftChild.Bounds.Contain(AllBoxes[k].Box);
	LeftChild.Children = Index2i(-1, -1);

	RightChild.StartIndex = LeftChild.EndIndex + 1;
	RightChild.EndIndex = Group.EndIndex;
	RightChild.Bounds = AxisBox2<RealType>::Empty();
	for (int32_t k = RightChild.StartIndex; k <= RightChild.EndIndex; ++k)
		RightChild.Bounds.Contain(AllBoxes[k].Box);
	RightChild.Children = Index2i(-1, -1);
}


template<typename RealType>
bool split_group_by_midpoint(
	unsafe_vector<TSourceBox<RealType>>& AllBoxes,
	const TBoxGroup<RealType>& Group,
	int32_t split_axis,
	RealType split_axis_origin,
	TBoxGroup<RealType>& LeftChild,
	TBoxGroup<RealType>& RightChild)
{
	gs_debug_assert(Group.Count() > 1);

	int32_t l = Group.StartIndex;
	int32_t r = Group.EndIndex;

	Vector2<RealType> LeftCenter, RightCenter;

	// walk from left to right, finding elements to swap
	while (l < r)
	{
		// if current left is <= splitpoint, step forward
		LeftCenter = AllBoxes[l].Box.Center();
		RealType lc = LeftCenter[split_axis];
		if (lc <= split_axis_origin) {
			l++;
			continue;
		}

		// ok at this point l is a box on the wrong side of the split

		while (r > l)
		{
			// if current right is > splitpoint, step to next
			RightCenter = AllBoxes[r].Box.Center();
			RealType rc = RightCenter[split_axis];
			if (rc > split_axis_origin) {
				r--;
				continue;
			} else
				break;	// ok now point r is on the wrong slide too, or we hit r == l
		}

		if (r == l)		// hit the midpoint, must be sorted
			break;

		GS::SwapTemp(AllBoxes[l], AllBoxes[r]);
		l++;
		r--;
	}

	if (l != r) {	// this can happen if we did a final swap right at midpoint, then l == r+1. just swap back
		GS::SwapTemp(l, r);
	} 
	else {			// l == r, need to make sure midpoint is on the right side
		Vector2<RealType> C = AllBoxes[l].Box.Center();
		if (C[split_axis] > split_axis_origin)
			l--;
	}

	// if all boxes ended up on the same side, we need to try something else.
	// for now just splitting in half arbitrarily.
	if (l == Group.StartIndex || r == Group.EndIndex)
	{
		split_group_trivial(AllBoxes, Group, LeftChild, RightChild);
		return true;
	}

	LeftChild.Bounds = AllBoxes[l].Box;
	for (int k = Group.StartIndex; k < l; ++k) {
		LeftChild.Bounds.Contain(AllBoxes[k].Box);
	}
	LeftChild.StartIndex = Group.StartIndex;
	LeftChild.EndIndex = l;
	LeftChild.Children = Index2i(-1, -1);

	RightChild.Bounds = AllBoxes[Group.EndIndex].Box;
	for (int k = l + 1; k < Group.EndIndex; ++k) {
		RightChild.Bounds.Contain(AllBoxes[k].Box);
	}
	RightChild.StartIndex = l+1;
	RightChild.EndIndex = Group.EndIndex;
	RightChild.Children = Index2i(-1, -1);

	return true;
}


template<typename RealType>
void GS::AxisBoxTree2<RealType>::Build(int32_t MaxBoxID, FunctionRef<bool(int BoxID, BoxType& Box)> GetBoxFunc, int32_t CountHint)
{
	static_assert(sizeof(ChildIndex) == sizeof(int32_t));

	using SourceBox = TSourceBox<RealType>;
	using BoxGroup = TBoxGroup<RealType>;

	BoxType CombinedBox = BoxType::Empty();
	unsafe_vector<SourceBox> BoxesList;
	BoxesList.reserve( (CountHint <= 0) ? MaxBoxID : CountHint );
	for (int k = 0; k < MaxBoxID; ++k) {
		SourceBox NewBox;
		if (GetBoxFunc(k, NewBox.Box)) {
			NewBox.BoxID = k;
			BoxesList.add_ref(NewBox);
			CombinedBox.Contain(NewBox.Box);
		}
	}
	int32_t NumBoxes = (int32_t)BoxesList.size();


	int NumChildrenInLeaf = 4;

	// trivial case - no tree, just a single box
	if (NumBoxes <= NumChildrenInLeaf) {
		LeafBoxLists.reserve(NumBoxes);
		int start_index = 0;
		for (int j = 0; j < NumBoxes; j++) {
			SourceBox& SrcBox = BoxesList[j];
			int new_index = (int)LeafBoxLists.add_ref(SourceBox2{ SrcBox.Box, SrcBox.BoxID });
		}
		this->RootBounds = CombinedBox;
		this->RootIndex.Index = 0;
		this->RootIndex.LeafCount = NumBoxes;
		return;
	}


	unsafe_vector<BoxGroup> GroupTree;
	GroupTree.reserve(NumBoxes / 2);
	BoxGroup RootGroup{ CombinedBox, 0, NumBoxes-1, Index2i(-1,-1) };
	GroupTree.add_ref(RootGroup);

	unsafe_vector<int32_t> SplitJobs;
	SplitJobs.reserve(NumBoxes / 8);		// seems completely arbitrary...
	SplitJobs.add(0);

	int NextIndex = -1;
	int NumInternalNodes = 0, NumLeafNodes = 0, TotalLeafItemCount = 0;
	while ( SplitJobs.pop_back(NextIndex) )
	{
		BoxGroup CurGroup = GroupTree[NextIndex];
		if (CurGroup.Count() > NumChildrenInLeaf)
		{
			NumInternalNodes++;

			// todo: maybe we want to split on bbox of center-points? 
			int split_axis = select_split_axis_longest(CurGroup.Bounds);
			RealType split_axis_origin = CurGroup.Bounds.Center()[split_axis];
			BoxGroup LeftChild, RightChild;
			split_group_by_midpoint(BoxesList, CurGroup, split_axis, split_axis_origin, LeftChild, RightChild);

			// update current group in tree (note: add_ref may resize!! be careful w/ references!!)
			CurGroup.Children.A = (int)GroupTree.add_ref(LeftChild);
			CurGroup.Children.B = (int)GroupTree.add_ref(RightChild);
			GroupTree[NextIndex] = CurGroup;

			// push split jobs
			SplitJobs.add(CurGroup.Children.A);
			SplitJobs.add(CurGroup.Children.B);
		}
		else
		{
			// this is a leaf group, just leave it and we will process later
			NumLeafNodes++;
			TotalLeafItemCount += CurGroup.Count();
		}
	}
	int NumTreeNodes = (int)GroupTree.size();
	gs_debug_assert(TotalLeafItemCount == NumBoxes);
	
	// reassign linear indices to internal nodes. Just do this in StartIndex as we don't need that anymore, for internal nodes
	int LinearIndex = 0;
	for (int i = 0; i < NumTreeNodes; ++i) {
		if ( ! GroupTree[i].IsLeaf() )
			GroupTree[i].StartIndex = LinearIndex++;
	}
	gs_debug_assert(LinearIndex == NumInternalNodes);

	// is this just BoxesList but in a different order?
	LeafBoxLists.reserve(TotalLeafItemCount);		
	int LinearBoxIndex = 0;
	for (int i = 0; i < NumTreeNodes; ++i) {
		BoxGroup& Group = GroupTree[i];
		if (!Group.IsLeaf())
			continue;

		int start_index = (int)LeafBoxLists.size(), count = Group.Count();
		for (int j = Group.StartIndex; j <= Group.EndIndex; j++) 
		{
			SourceBox& SrcBox = BoxesList[j];
			int new_index = (int)LeafBoxLists.add_ref(SourceBox2{ SrcBox.Box, SrcBox.BoxID });
		}
		Group.StartIndex = start_index; Group.EndIndex = count;
	}
	//BoxesList.clear();		// reclaim memory
	gs_debug_assert(LeafBoxLists.size() == (size_t)NumBoxes);

	// at this point, we have built our tree and child sets...rewrite them into more compact data structures
	this->RootIndex = ChildIndex{ 0,0 };
	this->RootBounds = CombinedBox;
	NodeTree.reserve(NumInternalNodes);

	for (int i = 0; i < NumTreeNodes; ++i)
	{
		const BoxGroup& Group = GroupTree[i];
		if (Group.IsLeaf())
			continue;		// don't have to do anything for leaf nodes

		InteriorNode NewInteriorNode;

		const BoxGroup& LeftChildGroup = GroupTree[Group.Children.A];
		const BoxGroup& RightChildGroup = GroupTree[Group.Children.B];
		
		NewInteriorNode.LeftBounds = LeftChildGroup.Bounds;
		NewInteriorNode.LeftChild.Index = LeftChildGroup.StartIndex;		// this is correct for both interior & leaf
		NewInteriorNode.LeftChild.LeafCount = LeftChildGroup.IsLeaf() ? LeftChildGroup.EndIndex : 0;

		NewInteriorNode.RightBounds = RightChildGroup.Bounds;
		NewInteriorNode.RightChild.Index = RightChildGroup.StartIndex;
		NewInteriorNode.RightChild.LeafCount = RightChildGroup.IsLeaf() ? RightChildGroup.EndIndex : 0;

		int NewIndex = (int)NodeTree.add_ref(NewInteriorNode);
		gs_debug_assert(NewIndex == Group.StartIndex);
	}

	// todo compute and save max depth? useful for stack queries..
	// (could also compute worst-case stack for depth-first traversal?)


	unsafe_vector<ChildIndex> stack;
	stack.reserve(16);

	size_t MaxStackSize = 0;
	stack.add(RootIndex);
	ChildIndex next;
	while (stack.pop_back(next))
	{
		if (next.LeafCount == 0) {
			const InteriorNode& Node = NodeTree[next.Index];
			stack.push_back(Node.LeftChild);
			stack.push_back(Node.RightChild);
		}
		MaxStackSize = GS::Max(MaxStackSize, stack.size());
	}


	// check that tree traversal finds all IDs
	//{
	//	std::vector<int> boxids;
	//	unsafe_vector<ChildIndex> stack;
	//	stack.push_back(RootIndex);
	//	ChildIndex next;
	//	while (stack.pop_back(next))
	//	{
	//		if (next.LeafCount > 0) {
	//			for (uint32_t j = 0; j < next.LeafCount; ++j)
	//				boxids.push_back(LeafBoxLists[next.Index + j].BoxID);
	//			continue;
	//		}

	//		const InteriorNode& Node = NodeTree[next.Index];
	//		stack.push_back(Node.LeftChild);
	//		stack.push_back(Node.RightChild);
	//	}
	//	gs_debug_assert(boxids.size() == LeafBoxLists.size());
	//}
}



template<typename RealType>
int GS::AxisBoxTree2<RealType>::PointContainmentQuery(
	Vector2<RealType> Point,
	FunctionRef<bool(int)> ElementTestFunc) const
{
	// use 32-elem small list here?? for 3 million tris the stack is never deeper than 21 levels...
	// but need to make it support pop  (currently it's Resize() is very bad!!)
	//TInlineSmallList<ChildIndex, 32> stack;
	unsafe_vector<ChildIndex> stack;
	stack.reserve(32);

	if (RootBounds.Contains(Point) == false)
		return -1;

	stack.push_back(RootIndex);
	ChildIndex next;
	while (stack.pop_back(next))
	{
		if (next.LeafCount > 0) {
			for (uint32_t j = 0; j < next.LeafCount; ++j) {
				const SourceBox2& Box = LeafBoxLists[next.Index + j];
				if (Box.Box.Contains(Point)) {
					if (ElementTestFunc(Box.BoxID))
						return Box.BoxID;
				}
			}
		} 
		else
		{
			const InteriorNode& Node = NodeTree[next.Index];
			if (Node.LeftBounds.Contains(Point))
				stack.push_back(Node.LeftChild);
			if (Node.RightBounds.Contains(Point))
				stack.push_back(Node.RightChild);
		}
	}
	return -1;
}


template<typename RealType>
bool GS::AxisBoxTree2<RealType>::PointContainmentQuery_FindAll(
	Vector2<RealType> Point,
	FunctionRef<bool(int)> ElementTestFunc,
	FunctionRef<void(int)> FoundElementFunc ) const
{
	//TInlineSmallList<ChildIndex, 32> stack;
	unsafe_vector<ChildIndex> stack;
	stack.reserve(32);

	if (RootBounds.Contains(Point) == false)
		return false;

	int NumFound = 0;
	stack.push_back(RootIndex);
	ChildIndex next;
	while (stack.pop_back(next))
	{
		if (next.LeafCount > 0) {
			for (uint32_t j = 0; j < next.LeafCount; ++j) {
				const SourceBox2& Box = LeafBoxLists[next.Index + j];
				if (Box.Box.Contains(Point)) {
					if (ElementTestFunc(Box.BoxID)) {
						FoundElementFunc(Box.BoxID);
						NumFound++;
					}
				}
			}
		}
		else
		{
			const InteriorNode& Node = NodeTree[next.Index];
			if (Node.LeftBounds.Contains(Point))
				stack.push_back(Node.LeftChild);
			if (Node.RightBounds.Contains(Point))
				stack.push_back(Node.RightChild);
		}
	}
	return NumFound > 0;
}



template<typename RealType>
DistanceResult2<RealType> GS::AxisBoxTree2<RealType>::PointDistanceQuery(
	Vector2<RealType> QueryPoint,
	FunctionRef<DistanceResult2<RealType>(int BoxID, const Vector2<RealType>& QueryPoint)> ElementDistanceSqrFunc,
	DistanceQueryOptions<RealType> Options) const
{
	struct StackBox
	{
		ChildIndex Index;
		AxisBox2<RealType> Box;
	};

	TInlineSmallList<StackBox, 32> stack;
	//unsafe_vector<StackBox> stack;
	//stack.reserve(32);

	int NumBoxTested = 0, NumTriTested = 0;

	RealType MaxDistSqr = Options.MaxDistance * Options.MaxDistance;
	RealType MinDistSqr = RootBounds.DistanceSquared(QueryPoint);
	DistanceResult2<RealType> MinDistResult;
	if (MinDistSqr > MaxDistSqr)
		return DistanceResult2<RealType>();
	MinDistSqr = MaxDistSqr;

	stack.push_back( {RootIndex, RootBounds} );
	StackBox nextBox;
	while (stack.pop_back(nextBox))
	{
		// test box again because we may have found a smaller min-distance while this node was on the stack
		// (is this worth it? often we are just re-testing a box we just pushed...)
		if (nextBox.Box.DistanceSquared(QueryPoint) > MinDistSqr)
			continue;

		ChildIndex next = nextBox.Index;

		if (next.LeafCount > 0) {
			for (uint32_t j = 0; j < next.LeafCount; ++j) {
				const SourceBox2& Box = LeafBoxLists[next.Index + j];
				RealType BoxDistSqr = Box.Box.DistanceSquared(QueryPoint);
				NumBoxTested++;
				if (BoxDistSqr < MinDistSqr) {
					DistanceResult2<RealType> ElemDistSqrResult = ElementDistanceSqrFunc(Box.BoxID, QueryPoint);
					NumTriTested++;
					if (ElemDistSqrResult.DistanceSqr < MinDistSqr) {
						MinDistSqr = ElemDistSqrResult.DistanceSqr;
						MinDistResult = ElemDistSqrResult;
						MinDistResult.ElementID = Box.BoxID;
					}
				}
			}
		}
		else
		{
			const InteriorNode& Node = NodeTree[next.Index];
			StackBox Children[2] = { {Node.LeftChild,Node.LeftBounds},  {Node.RightChild,Node.RightBounds} };
			RealType ChildDists[2] = { Node.LeftBounds.DistanceSquared(QueryPoint), Node.RightBounds.DistanceSquared(QueryPoint)};
			if (ChildDists[0] > ChildDists[1]) {
				GS::SwapTemp(ChildDists[0], ChildDists[1]);
				GS::SwapTemp(Children[0], Children[1]);
			}
			if (ChildDists[0] < MinDistSqr) {
				stack.push_back(Children[0]);
				if (ChildDists[1] < MinDistSqr)		// Y is larger
					stack.push_back(Children[1]);
			}
		}
	}
	
	return MinDistResult;
}





template<typename RealType>
void check_boxes(const AxisBox2<RealType>& A, const AxisBox2<RealType>& B)
{
	RealType AreaA = A.Area();
	RealType AreaB = B.Area();
	gs_debug_assert(ToleranceEqual(AreaA, AreaB));
	AxisBox2<RealType> Combined = A;
	A.Contains(B);
	RealType AreaC = Combined.Area();
	gs_debug_assert(ToleranceEqual(AreaA, AreaC));
}


template<typename RealType>
void GS::AxisBoxTree2<RealType>::Validate()
{
	AxisBox2<RealType> ComputedBounds;
	validate_internal_node(RootIndex, ComputedBounds);
	check_boxes(RootBounds, ComputedBounds);
}



template<typename RealType>
void GS::AxisBoxTree2<RealType>::validate_leaf_child(ChildIndex Index, AxisBox2<RealType>& ComputedBounds)
{
	gs_debug_assert(Index.LeafCount > 0);
	ComputedBounds = AxisBox2<RealType>::Empty();
	for (uint32_t j = 0; j < Index.LeafCount; ++j) {
		ComputedBounds.Contain(LeafBoxLists[Index.Index + j].Box);
	}
}


template<typename RealType>
void GS::AxisBoxTree2<RealType>::validate_internal_node(ChildIndex Index, AxisBox2<RealType>& ComputedBounds)
{
	gs_debug_assert(Index.LeafCount == 0);
	const InteriorNode& Node = NodeTree[Index.Index];
	
	AxisBox2<RealType> LeftBox, RightBox;
	if (Node.LeftChild.LeafCount > 0)
		validate_leaf_child(Node.LeftChild, LeftBox);
	else
		validate_internal_node(Node.LeftChild, LeftBox);
	check_boxes(LeftBox, Node.LeftBounds);

	if (Node.RightChild.LeafCount > 0)
		validate_leaf_child(Node.RightChild, RightBox);
	else
		validate_internal_node(Node.RightChild, RightBox);
	check_boxes(RightBox, Node.RightBounds);

	ComputedBounds = LeftBox;
	ComputedBounds.Contain(RightBox);
}


// explicit instantiation
template class GRADIENTSPACECORE_API GS::AxisBoxTree2<float>;
template class GRADIENTSPACECORE_API GS::AxisBoxTree2<double>;
