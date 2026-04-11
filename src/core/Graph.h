#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include "DynamicArray.h"
#include "LinkedList.h"

// one edge to a neighbor
struct Edge {
    int toNode;
    double weight;

    Edge() {
        toNode = 0;
        weight = 0.0;
    }

    Edge(int to, double w) {
        toNode = to;
        weight = w;
    }
};

// one intersection/node
struct GraphNode {
    int id;
    std::string label;
    LinkedList<Edge> edges;

    GraphNode() {
        id = -1;
        label = "";
    }

    GraphNode(int i, const std::string& l) {
        id = i;
        label = l;
    }
};

// adjacency list graph
class Graph {
private:
    DynamicArray<GraphNode> nodes;
    int nodeCount;

    // find index of node by id
    int findIndex(int id) {
        for (int i = 0; i < nodeCount; i++) {
            if (nodes.get(i).id == id)
                return i;
        }
        return -1;
    }

public:
    // init empty graph
    Graph() {
        nodeCount = 0;
    }

    // add new node
    void addNode(int id, const std::string& label) {
        GraphNode newNode(id, label);
        nodes.pushBack(newNode);
        nodeCount++;
    }

    // add directed edge
    void addEdge(int fromId, int toId, double weight) {
        int index = findIndex(fromId);
        if (index == -1) return;
        Edge e(toId, weight);
        nodes.get(index).edges.pushBack(e);
    }

    // add undirected edge
    void addUndirectedEdge(int fromId, int toId, double weight) {
        addEdge(fromId, toId, weight);
        addEdge(toId, fromId, weight);
    }

    // get node by id
    GraphNode* getNode(int id) {
        int index = findIndex(id);
        if (index == -1) return nullptr;
        return &nodes.get(index);
    }

    // total node count
    int getNodeCount() {
        return nodeCount;
    }

    // check node exists
    bool hasNode(int id) {
        return findIndex(id) != -1;
    }

    // dijkstra shortest path
    double dijkstra(int startId, int endId, DynamicArray<int>& pathOut) {
        int n = nodeCount;

        DynamicArray<double> dist;
        DynamicArray<int>    prev;
        DynamicArray<bool>   visited;

        // init distances
        for (int i = 0; i < n; i++) {
            dist.pushBack(1e18);
            prev.pushBack(-1);
            visited.pushBack(false);
        }

        int startIndex = findIndex(startId);
        if (startIndex == -1) return -1;
        dist.set(startIndex, 0.0);

        // relax all nodes
        for (int iter = 0; iter < n; iter++) {

            // pick smallest unvisited
            int u = -1;
            for (int i = 0; i < n; i++) {
                if (!visited.get(i)) {
                    if (u == -1 || dist.get(i) < dist.get(u))
                        u = i;
                }
            }

            if (u == -1 || dist.get(u) >= 1e18) break;
            visited.set(u, true);

            // relax neighbors
            ListNode<Edge>* curr = nodes.get(u).edges.getHead();
            while (curr != nullptr) {
                int v = findIndex(curr->data.toNode);
                if (v != -1 && !visited.get(v)) {
                    double newDist = dist.get(u) + curr->data.weight;
                    if (newDist < dist.get(v)) {
                        dist.set(v, newDist);
                        prev.set(v, u);
                    }
                }
                curr = curr->next;
            }
        }

        // build path
        int endIndex = findIndex(endId);
        if (endIndex == -1 || dist.get(endIndex) >= 1e18) return -1;

        // trace back from end
        DynamicArray<int> reversed;
        int curr = endIndex;
        while (curr != -1) {
            reversed.pushBack(nodes.get(curr).id);
            curr = prev.get(curr);
        }

        // reverse into pathOut
        for (int i = reversed.size() - 1; i >= 0; i--)
            pathOut.pushBack(reversed.get(i));

        return dist.get(endIndex);
    }

    // print all nodes and edges (debug)
    void print() {
        for (int i = 0; i < nodeCount; i++) {
            std::cout << "Node " << nodes.get(i).id
                      << " (" << nodes.get(i).label << ") -> ";
            ListNode<Edge>* curr = nodes.get(i).edges.getHead();
            while (curr != nullptr) {
                std::cout << curr->data.toNode
                          << " (w=" << curr->data.weight << ") ";
                curr = curr->next;
            }
            std::cout << std::endl;
        }
    }
};

#endif