// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/FunctionRef.h"
#include "Core/unsafe_vector.h"
#include "Math/GSVector2.h"
#include "Math/GSAxisBox2.h"
#include "Math/GSIndex2.h"
#include "Math/GSIndex4.h"
#include "Spatial/SpatialResult2.h"

namespace GS
{


template<typename RealType>
struct DistanceQueryOptions
{
	RealType MaxDistance = RealConstants<RealType>::SafeMaxValue();
};


template<typename RealType>
class AxisBoxTree2
{
public:
	using BoxType = typename AxisBox2<RealType>;

	void Build(
		int32_t MaxBoxID, 
		FunctionRef<bool(int, AxisBox2<RealType>& Box)> GetBoxFunc,
		int32_t CountHint = 0);

	//! find ElementID of box that contains Point and passes ElementTestFunc
	int PointContainmentQuery(
		Vector2<RealType> Point,
		FunctionRef<bool(int)> ElementTestFunc) const;

	//! call FoundElementFunc for all ElementIDs with boxes that contain Point and pass ElementTestFunc
	//! returns false if no elements found
	bool PointContainmentQuery_FindAll(
		Vector2<RealType> Point,
		FunctionRef<bool(int)> ElementTestFunc,
		FunctionRef<void(int)> FoundElementFunc ) const;

	DistanceResult2<RealType> PointDistanceQuery(
		Vector2<RealType> Point,
		FunctionRef<DistanceResult2<RealType>(int BoxID, const Vector2<RealType>& QueryPoint)> ElementDistanceSqrFunc,
		DistanceQueryOptions<RealType> Options = DistanceQueryOptions<RealType>() ) const;

	void Validate();

public:

	struct ChildIndex {
		uint32_t LeafCount : 4;		// max 16 elements in a leaf. If > 0, Index is a leaf-node-index
		uint32_t Index : 28;		// max 268M internal or leaf nodes
	};

	struct InteriorNode {
		ChildIndex LeftChild;
		ChildIndex RightChild;
		BoxType LeftBounds;
		BoxType RightBounds;
	};

	struct SourceBox2
	{
		AxisBox2<RealType> Box;
		int32_t BoxID;
	};

	ChildIndex RootIndex;
	BoxType RootBounds;
	unsafe_vector<InteriorNode> NodeTree;
	unsafe_vector<SourceBox2> LeafBoxLists;


private:
	void validate_leaf_child(ChildIndex Index, AxisBox2<RealType>& ComputedBounds);
	void validate_internal_node(ChildIndex Index, AxisBox2<RealType>& ComputedBounds);
};


typedef AxisBoxTree2<float> AxisBoxTree2f;
typedef AxisBoxTree2<double> AxisBoxTree2d;


// explicit instantiation
extern template class AxisBoxTree2<float>;
extern template class AxisBoxTree2<double>;

} // end namespace GS