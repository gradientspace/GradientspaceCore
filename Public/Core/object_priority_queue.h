// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Core/gs_debug.h"
#include "Core/FunctionRef.h"

namespace GS
{

namespace QueueUtil
{
	// NodeType in object_priority_queue<NodeType> should extend this type,
	// or define it's own type with the same two members first
	struct base_queue_node
	{
		float queue_priority = 0;
		int queue_index = 0;
		void queue_reset() {
			queue_index = -1;
			queue_priority = 0;
		}
	};
}





/**
 * object_priority_queue<T> implements a priority queue for an external set of allocated elements.
 * 
 * Priorities are specified as float values, with lower values meaning higher priority
 * 
 * The elements must have members .queue_priority and .queue_index   (ie object_priority_queue<QueueUtil::base_queue_node> is the minimal implementation)
 * Allocation and Deallocation of the elements must be managed externally, pointers passed to Enqueue()
 * will be referenced internally until they are returned by Dequeue(), Remove() is called, or Reset() is called
 * 
 * a linear list of pointers is used internally for the queue storage 
 */
template<typename NodeType>
class object_priority_queue
{
public:
	//! list of nodes. Index 0 is not used (for some reason?)
	unsafe_vector<NodeType*> nodes;
	//! number of nodes currently remaining in the queue. Should be equal to nodes.size()+1
	int num_nodes;

	//! set to true to enable validation checks (which will assert)
	bool EnableDebugChecks = false;


public:
	object_priority_queue()
	{
		num_nodes = 0;
	}

	//! number of nodes currently in queue
	int Count() const { return num_nodes; }

	//! return true if queue is empty
	bool IsEmpty() const { return num_nodes <= 0; }

	//! reset the queue to empty state. 
	void Reset(bool bFreeMemory = true) {
		nodes.clear(bFreeMemory);
		num_nodes = 0;
	}

	const NodeType* First() const {
		return nodes[1];
	}

	float FirstPriority() const {
		return nodes[1]->queue_priority;
	}

	//! return true if queue contains the node
	bool Contains(const NodeType* node) const {
		if (node->queue_index < 1 || node->queue_index >= nodes.size())
			return false;
		return (nodes[node->queue_index] == node);
	}

	//! add the specified node to the queue with the given priority. Node must not already be in the queue (use UpdatePriority in that case)
	void Enqueue(NodeType* node, float priority)
	{
		if (EnableDebugChecks)
			gs_debug_assert(Contains(node) == false);

		node->queue_priority = priority;
		num_nodes++;
		nodes.set_at(node, num_nodes);
		node->queue_index = num_nodes;
		move_up(nodes[num_nodes]);
	}

	//! remove and return the lowest-priority node at the top of the queue
	NodeType* Dequeue()
	{
		if (EnableDebugChecks)
			gs_debug_assert(num_nodes > 0);

		NodeType* topNode = nodes[1];
		Remove(topNode);
		return topNode;
	}

	//! explicitly remove a node from the queue
	void Remove(NodeType* nodeToRemove)
	{
		if (EnableDebugChecks)
			gs_debug_assert(Contains(nodeToRemove));

		// if the node is already the last node, we can remove it immediately
		if (nodeToRemove->queue_index == num_nodes) {
			nodes[num_nodes] = nullptr;
			num_nodes--;
			return;
		}

		// swap the node with the last node  (swap-remove)
		NodeType* formerLastNode = nodes[num_nodes];
		swap_nodes(nodeToRemove, formerLastNode);
		nodes[num_nodes] = nullptr;
		num_nodes--;

		// now bubble formerLastNode (which is no longer the last node) up or down as appropriate
		on_node_updated(formerLastNode);

		// mark noded as removed from queue (retain priority in case caller has a use for it)
		nodeToRemove->queue_index = 0;
	}

	//! update priority of a node in the queue. Node must be in the queue.
	void UpdatePriority(NodeType* node, float new_priority)
	{
		if (EnableDebugChecks)
			gs_debug_assert(Contains(node));
		if (node->queue_priority != new_priority) {
			node->queue_priority = new_priority;
			on_node_updated(node);
		}
	}

	//! call NodeFunc for each node in the queue
	void EnumerateNodes(FunctionRef<void(const NodeType* node)> NodeFunc)
	{
		for (int i = 1; i <= num_nodes; i++)
			NodeFunc(nodes[i]);
	}



protected:

	// swap node locations and indices
	void swap_nodes(NodeType* node1, NodeType* node2)
	{
		nodes[node1->queue_index] = node2;
		nodes[node2->queue_index] = node1;
		int temp = node1->queue_index;
		node1->queue_index = node2->queue_index;
		node2->queue_index = temp;
	}

	// move node up tree to correct position by iteratively swapping w/ parent
	void move_up(NodeType* node)
	{
		int parent = node->queue_index / 2;
		while (parent >= 1) {
			NodeType* parentNode = nodes[parent];
			if (has_higher_priority(parentNode, node))
				break;
			swap_nodes(node, parentNode);
			parent = node->queue_index / 2;
		}
	}

    // move node down tree branches to correct position, by iteratively swapping w/ children
    void move_down(NodeType* node)
    {
        // we will put input node at this position after we are done swaps (ie actually moves, not swaps)
        int cur_node_index = node->queue_index;

        while (true) 
		{
            NodeType* min_node = node;
            int iLeftChild = 2 * cur_node_index;

            // past end of tree, must be in the right spot
            if (iLeftChild > num_nodes) {
                // store input node in final position
                node->queue_index = cur_node_index;
                nodes[cur_node_index] = node;
                break;
            }

            // check if priority is larger than either child - if so we want to swap
			NodeType* left_child_node = nodes[iLeftChild];
            if (has_higher_priority(left_child_node, min_node)) {
                min_node = left_child_node;
            }
            int iRightChild = iLeftChild + 1;
            if (iRightChild <= num_nodes) {
                NodeType* right_child_node = nodes[iRightChild];
                if (has_higher_priority(right_child_node, min_node)) {
                    min_node = right_child_node;
                }
            }

            // if we found node with higher priority, swap with it (ie move it up) and take its place
            // (but we only write start node to final position, not intermediary slots)
            if (min_node != node) {
                nodes[cur_node_index] = min_node;

                int temp = min_node->queue_index;
                min_node->queue_index = cur_node_index;
                cur_node_index = temp;
            } else {
                // store input node in final position
                node->queue_index = cur_node_index;
                nodes[cur_node_index] = node;
                break;
            }
        }
    }

	// call after node is modified, to move it to correct position in queue
	void on_node_updated(NodeType* node)
	{
		int parentIndex = node->queue_index / 2;
		NodeType* parentNode = nodes[parentIndex];
		if (parentIndex > 0 && has_higher_priority(node, parentNode))
			move_up(node);
		else
			move_down(node);
	}

	/// returns true if priority at higher is less than at lower
	bool has_higher_priority(const NodeType* higher, const NodeType* lower) const
	{
		return (higher->queue_priority < lower->queue_priority);
	}



public:
	//void DebugPrint()
	//{
	//	for (int i = 1; i <= num_nodes; ++i)
	//		GS_LOG("{0} : p {1}  idx {2}", i, nodes[i]->priority, nodes[i]->queue_index);
	//}

};



} // end namespace GS