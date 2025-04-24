// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/object_priority_queue.h"
#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSFrame3.h"
#include "Math/GSIndex3.h"
#include "Math/GSMatrix2.h"
#include "Core/unsafe_vector.h"
#include "Core/packed_int_lists.h"

namespace GS
{


namespace ExpMapUtil
{
    //! project Position into tangent/normal LocalFrame
    static Vector2d ComputeLocalFrameUV(const Frame3d& LocalFrame, Vector3d Position)
    {
        Position -= LocalFrame.Origin;
        return Vector2d( LocalFrame.X().Dot(Position), LocalFrame.Y().Dot(Position) );
    }

    //! compute UV at Position by propagating NbrUV from NbrFrame, relative to SeedFrame
    static Vector2d propagate_uv(const Vector3d& Position, const Vector2d& NbrUV, const Frame3d& NbrFrame, const Frame3d& SeedFrame)
    {
        Vector2d local_uv = ComputeLocalFrameUV(NbrFrame, Position);

        Frame3d fSeedToLocal = SeedFrame;
        fSeedToLocal.AlignAxis(2, NbrFrame.Z());

        Vector3d vAlignedSeedX = fSeedToLocal.X();
        Vector3d vLocalX = NbrFrame.X();

        double fCosTheta = vLocalX.Dot(vAlignedSeedX);

        // compute rotated min-dist vector for this particle
        double fTmp = 1 - fCosTheta * fCosTheta;
        if (fTmp < 0)
            fTmp = 0;     // need to clamp so that sqrt works...
        double fSinTheta = GS::Sqrt(fTmp);
        Vector3d vCross = GS::Cross(vLocalX, vAlignedSeedX);
        if (vCross.Dot(NbrFrame.Z()) < 0)    // get the right sign...
            fSinTheta = -fSinTheta;

        Matrix2d FrameRotation(fCosTheta, fSinTheta, -fSinTheta, fCosTheta);

        return NbrUV + FrameRotation * local_uv;
    }

}




/**
 * Discrete Exponential/Log Map implementation where all the necessary data is stored internally.
 * Usage:
 * 
 * - call Initialize(MaximumNodeIndex) to allocate internal arrays
 * - call InitializeNode(Index) for each 3D vertex, passing position, normal, and nbrs
 * - call ComputeToMaxDistance() or variants
 * - call GetUV(Index) for each index to collect results
 * 
 * Initialized nodes can be sparse, but dense buffers are allocated O(N)-iterated during ComputeToMaxDistance()
  */
class GenericExpMap
{

public:


    void Initialize(int MaxNodeIndex)
    {
        Nodes.clear();
        Nodes.resize(MaxNodeIndex+1);
        NodeNeighbours.Initialize(MaxNodeIndex + 1, 8);

        Queue.Reset(false);
    }

    void InitializeNode(int Index, Vector3d Position, Vector3d Normal, 
        const_buffer_view<int> Neighbours, int ExternalID = -1)
    {
        gs_debug_assert(Index >= 0 && Index < Nodes.size());

        GraphNode newNode;
        newNode.initialize();
        newNode.Index = Index;
        newNode.ExternalID = ExternalID;
        newNode.Position = Position;
        newNode.Normal = Normal;

        for (int nbr : Neighbours)
            gs_debug_assert(nbr >= 0 && nbr < Nodes.size());
        bool bOK = NodeNeighbours.AppendList(Index, Neighbours);

        Nodes[Index] = newNode;
    }

    bool Validate()
    {
        // todo - check that neighbours and nodes are all initialized
    }


    void ComputeToMaxDistance(Frame3d seedFrame, Index3i SeedNbrs, double MaxGraphDistance)
    {
        reset_for_compute();
        SeedFrame = seedFrame;

        for ( int j = 0; j < 3; ++j ) 
        {
            int node_index = SeedNbrs[j];
            GraphNode& g = Nodes[node_index];
            g.uv = ExpMapUtil::ComputeLocalFrameUV(SeedFrame, g.Position);
            g.graph_distance = g.uv.Length();
            g.frozen = true;

            gs_debug_assert(Queue.Contains(&g) == false);

            Queue.Enqueue(&g, (float)g.graph_distance);

            gs_debug_assert(Queue.Contains(&g));
        }

        while (Queue.Count() > 0)
        {
            GraphNode* g = Queue.Dequeue();
            cur_max_graph_distance = GS::Max(g->graph_distance, cur_max_graph_distance);
            if (cur_max_graph_distance > MaxGraphDistance )
                return;

            if (g->ParentIndex != -1) {
                update_uv_upwind_expmap(g);
            }

            double uv_dist_sqr = g->uv.SquaredLength();
            if (uv_dist_sqr > cur_max_uv_distance)
                cur_max_uv_distance = uv_dist_sqr;

            g->frozen = true;

            // recompute nbr distances and insert or update in queue
            update_neighbours(g);
        }

        cur_max_uv_distance = GS::Sqrt(cur_max_uv_distance);
    }


    double MaxComputedGraphDistance() const{
        return cur_max_graph_distance;
    }
    double MaxComputedUVDistance() const {
        return cur_max_uv_distance;
    }

    ResultOrFail<Vector2d> GetUV(int Index) const 
    {
        if (Index < 0 || Index >= Nodes.size())
            return ResultOrFail<Vector2d>();
        const GraphNode& g = Nodes[Index];
        if (g.frozen == false)
            return ResultOrFail<Vector2d>();
        return g.uv;
    }



protected:

    Frame3d SeedFrame;
    double cur_max_graph_distance;
    double cur_max_uv_distance;

    struct GraphNode : GS::QueueUtil::base_queue_node
    {
        int Index;
        int ExternalID;
        Vector3d Position;
        Vector3d Normal;

        // these values are populated during parameterization
        int ParentIndex;
        double graph_distance;
        Vector2d uv;
        bool frozen;

        void initialize() {
            Index = -1;
            ExternalID = -1;
            Position = Vector3d::Zero();
            Normal = Vector3d::UnitZ();
            reset();
        }

        void reset() {
            ParentIndex = -1;
            graph_distance = GS::Mathd::SafeMaxExtent();
            uv = GS::Vector2d::Zero();
            frozen = false;
            queue_reset();
        }
    };

    unsafe_vector<GraphNode> Nodes;
    packed_int_lists NodeNeighbours;

    object_priority_queue<GraphNode> Queue;


    void reset_for_compute()
    {
        for (GraphNode& node : Nodes)
            node.reset();
        cur_max_graph_distance = 0;
        cur_max_uv_distance = 0;
        Queue.Reset(false);
    }

    // todo refactor this into standalone function in ExpMapUtil...
    void update_uv_upwind_expmap(GraphNode* node)
    {
        Vector2d avg_uv = Vector2d::Zero();
        double fWeightSum = 0;
        int nbr_count = 0;
        const_buffer_view<int> Neighbours = NodeNeighbours.GetListView(node->Index);
        for (int nbr_index : Neighbours )
        {
            GraphNode& nbr_node = Nodes[nbr_index];
            if (nbr_node.frozen) {

                Frame3d nbr_frame(nbr_node.Position, nbr_node.Normal);

                Vector2d nbr_uv = ExpMapUtil::propagate_uv(node->Position, nbr_node.uv, nbr_frame, SeedFrame);
                
                double fWeight = 1.0 / ( node->Position.DistanceSquared(nbr_node.Position) + Mathd::ZeroTolerance() );
                avg_uv += fWeight * nbr_uv;
                fWeightSum += fWeight;
                nbr_count++;
            }
        }
        gs_debug_assert(nbr_count > 0);

        //avg_uv /= (float)nbr_count;
        avg_uv /= fWeightSum;
        node->uv = avg_uv;
    }




    void update_neighbours(GraphNode* parentNode)
    {
        Vector3d parentPos = parentNode->Position;
        double parentDist = parentNode->graph_distance;

        const_buffer_view<int> ParentNeighbours = NodeNeighbours.GetListView(parentNode->Index);
        for (int nbr_index : ParentNeighbours)
        {
            GraphNode& nbr = Nodes[nbr_index];
            if (nbr.frozen)
                continue;

            double parent_nbr_dist = parentDist + parentPos.Distance(nbr.Position);
            if (Queue.Contains(&nbr)) {
                if (parent_nbr_dist < nbr.graph_distance) {
                    nbr.ParentIndex = parentNode->Index;
                    nbr.graph_distance = parent_nbr_dist;
                    Queue.UpdatePriority(&nbr, (float)nbr.graph_distance);
                }
            } else {
                nbr.ParentIndex = parentNode->Index;
                nbr.graph_distance = parent_nbr_dist;
                Queue.Enqueue(&nbr, (float)nbr.graph_distance);
            }
        }
    }

};




} // end namespace GS