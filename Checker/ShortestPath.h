////////////////////////////////
/// usage : 1.	shortest path algorithms.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_SZX_CHECKER_SHORTEST_PATH_H
#define SMART_SZX_CHECKER_SHORTEST_PATH_H


#include <algorithm>
#include <vector>

#include "PriorityQueue.h"


// [off] use compare base priority queue instead of bucket/radix based one.
//       it will disable lazy remove, which keep the relaxed node in the queue. it requires the priority queue
//       implementation tracks the original priority and the number of items in queue is irrelevant to its performance.
//       bucket queue may be a good reason to enable this switch.
#define SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE  0


namespace szx {

//using ID = int;
//using Weight = int;
template<typename ID = int, typename Weight = int>
class Dijkstra {
public:
    struct AdjNode {
        AdjNode(ID dstNodeId, Weight edgeWeight = 0) : dst(dstNodeId), weight(edgeWeight) {}

        ID dst;
        Weight weight;
};
    using AdjVec = std::vector<AdjNode>;
    using AdjList = std::vector<AdjVec>;
    using Path = std::vector<ID>;

    #if SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
    using PriorityQueueImp = impl::PriorityQueue::SetImpl<ID, Cmp>;
    #else
    using PriorityQueueImp = impl::PriorityQueue::BucketL1Impl<ID>;
    #endif // SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE


    static constexpr ID InvalidId = -1;
    static constexpr Weight NoLinkWeight = (1 << 24); // avoid max+max overflow.


    Dijkstra(const AdjList &adjacencyList, ID source, Weight maxWeight = NoLinkWeight)
        : adjList(adjacencyList), src(source), minCost(adjacencyList.size()),
        prevNode(adjacencyList.size()), nodeVisited(adjacencyList.size()),
        #if SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
        unvisitedNodes(Cmp(&minCost)) {
        #else
        nodeQueued(adjacencyList.size()), unvisitedNodes(maxWeight) {
        #endif // SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
        clear();
    }

    void clear() {
        std::fill(minCost.begin(), minCost.end(), NoLinkWeight);
        minCost[src] = 0;
        std::fill(prevNode.begin(), prevNode.end(), InvalidId);
        unvisitedNodes.clear();
        std::fill(nodeVisited.begin(), nodeVisited.end(), false);

        #if !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
        std::fill(nodeQueued.begin(), nodeQueued.end(), false);
        nodeNumInQueue = 0;
        #endif // !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE

        nodeVisited[src] = true;

        #if !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
        nodeQueued[src] = true;
        #endif // !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE

        for (auto adjNode = adjList[src].begin(); adjNode != adjList[src].end(); ++adjNode) {
            Weight newDist = adjNode->weight;
            #if !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
            nodeQueued[adjNode->dst] = true;
            ++nodeNumInQueue;
            #endif // !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE

            minCost[adjNode->dst] = newDist;
            prevNode[adjNode->dst] = src;

            unvisitedNodes.push(adjNode->dst, newDist);
        }
    }

    void reset(ID source) {
        src = source;
        clear();
    }
    void reset(const std::vector<bool> &isNodeVisited) {
        clear();
        nodeVisited = isNodeVisited;
    }
    void reset(const std::vector<bool> &isNodeVisited, ID source) {
        src = source;
        reset(isNodeVisited);
    }


    template<typename Pred>
    ID next(const Pred &isTarget) {
        while (!unvisitedNodes.empty()) {
            ID nearestNode = unvisitedNodes.top();

            #if !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
            if (nodeVisited[nearestNode]) { unvisitedNodes.pop(); continue; }
            if (nodeNumInQueue <= 0) { return InvalidId; }
            #endif // !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE

            if (isTarget(nearestNode)) { return nearestNode; }

            nodeVisited[nearestNode] = true;
            unvisitedNodes.pop();

            #if !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
            --nodeNumInQueue;
            #endif // !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE

            for (auto adjNode = adjList[nearestNode].begin(); adjNode != adjList[nearestNode].end(); ++adjNode) {
                if (nodeVisited[adjNode->dst]) { continue; } // assume if a node is visited, `newDist < minCost[adjNode->dst]` will always be false.

                Weight newDist = minCost[nearestNode] + adjNode->weight;
                if (newDist < minCost[adjNode->dst]) {
                    #if !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
                    if (!nodeQueued[adjNode->dst]) {
                        nodeQueued[adjNode->dst] = true;
                        ++nodeNumInQueue;
                    }
                    #else
                    unvisitedNodes.remove(adjNode->dst);
                    #endif // !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE

                    minCost[adjNode->dst] = newDist;
                    prevNode[adjNode->dst] = nearestNode;

                    unvisitedNodes.push(adjNode->dst, newDist);
                }
            }
        }

        return InvalidId;
    }
    ID nextClosest() {
        // OPTIMIZE[szx][8]: record prevClosest node to save the trailing next() call.
        if (unvisitedNodes.empty()) { return InvalidId; }
        ID nearestNode = unvisitedNodes.top();
        next([nearestNode](ID node) { return (node != nearestNode); });
        return nearestNode;
    }
    bool findPointToPointPath(ID dst) { return (next([dst](ID node) { return (node == dst); }) != InvalidId); }
    void findSingleSourcePaths() { next([](ID) { return false; }); }

    bool pathFound(ID dst) const { return (prevNode[dst] != InvalidId); }

    Path getReversePath(ID dst, bool withoutSrc = true, bool withoutDst = true) const {
        Path path;
        path.reserve(adjList.size());
        ID endNode = withoutSrc ? src : InvalidId;
        if (withoutDst) { dst = prevNode[dst]; }
        for (; dst != endNode; dst = prevNode[dst]) { path.push_back(dst); }
        return path;
    }
    Path getPath(ID dst, bool withoutSrc = true, bool withoutDst = true) const {
        Path path(getReversePath(dst, withoutSrc, withoutDst));
        std::reverse(path.begin(), path.end());
        return path;
    }

    const std::vector<Weight>& getDistList() const { return minCost; }
    Weight getDist(ID dst) const { return minCost[dst]; }

    ID getPrevNode(ID node) const { return prevNode[node]; }
    const std::vector<ID>& getPrevNodeList() const { return prevNode; }

    const AdjList& getAdjacencyList() const { return adjList; }
    ID getSource() const { return src; }
    const std::vector<bool>& getNodeVisited() const { return nodeVisited; }
    const PriorityQueueImp& getUnvisitedNodes() const { return unvisitedNodes; }
    ID getLastNode() const { return (const_cast<Dijkstra*>(this))->unvisitedNodes.top(); }

protected:
    /// input.
    const AdjList &adjList;
    ID src;
    /// output.
    std::vector<Weight> minCost; // `minCost[node]` is the min cost from source to `node`.
    std::vector<ID> prevNode; // `prevNode[node]` is where the `node` came from in the shortest path.
    std::vector<bool> nodeVisited;

    #if !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE
    std::vector<bool> nodeQueued; // `nodeQueued[node]` means if the node has been enqueued.
    ID nodeNumInQueue; // number of nodes that is in queue currently.
    #endif // !SZX_CPPUTILIBS_SHORTEST_PATH_CMP_BASED_QUEUE

    PriorityQueueImp unvisitedNodes;

private:
    Dijkstra & operator=(const Dijkstra &) = delete;
};


}


#endif // SMART_SZX_CHECKER_SHORTEST_PATH_H
