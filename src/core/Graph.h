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
            if (nodes[i].id == id)
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
        nodes.push_back(newNode);
        nodeCount++;
    }

    // add directed edge
    void addEdge(int fromId, int toId, double weight) {
        int index = findIndex(fromId);
        if (index == -1) return;
        Edge e(toId, weight);
        nodes[index].edges.pushBack(e);
    }

    // add undirected edge (both ways)
    void addUndirectedEdge(int fromId, int toId, double weight) {
        addEdge(fromId, toId, weight);
        addEdge(toId, fromId, weight);
    }

    // get node by id
    GraphNode* getNode(int id) {
        int index = findIndex(id);
        if (index == -1) return nullptr;
        return &nodes[index];
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
    // returns total distance from start to end (-1 if unreachable)
    double dijkstra(int startId, int endId, DynamicArray<int>& pathOut) {
        int n = nodeCount;

        DynamicArray<double> dist;
        DynamicArray<int>    prev;
        DynamicArray<bool>   visited;

        // init distances
        for (int i = 0; i < n; i++) {
            dist.push_back(1e18);
            prev.push_back(-1);
            visited.push_back(false);
        }

        int startIndex = findIndex(startId);
        if (startIndex == -1) return -1;
        dist[startIndex] = 0.0;

        // relax all nodes
        for (int iter = 0; iter < n; iter++) {

            // pick unvisited node with smallest dist
            int u = -1;
            for (int i = 0; i < n; i++) {
                if (!visited[i]) {
                    if (u == -1 || dist[i] < dist[u])
                        u = i;
                }
            }

            if (u == -1 || dist[u] >= 1e18) break;
            visited[u] = true;

            // relax neighbors
            ListNode<Edge>* curr = nodes[u].edges.getHead();
            while (curr != nullptr) {
                int v = findIndex(curr->data.toNode);
                if (v != -1 && !visited[v]) {
                    double newDist = dist[u] + curr->data.weight;
                    if (newDist < dist[v]) {
                        dist[v] = newDist;
                        prev[v] = u;
                    }
                }
                curr = curr->next;
            }
        }

        // build path
        int endIndex = findIndex(endId);
        if (endIndex == -1 || dist[endIndex] >= 1e18) return -1;

        // trace back from end to start
        DynamicArray<int> reversed;
        int curr = endIndex;
        while (curr != -1) {
            reversed.push_back(nodes[curr].id);
            curr = prev[curr];
        }

        // reverse into pathOut
        for (int i = reversed.getSize() - 1; i >= 0; i--)
            pathOut.push_back(reversed[i]);

        return dist[endIndex];
    }

    // print all nodes and edges (debug)
    void print() {
        for (int i = 0; i < nodeCount; i++) {
            std::cout << "Node " << nodes[i].id
                      << " (" << nodes[i].label << ") -> ";
            ListNode<Edge>* curr = nodes[i].edges.getHead();
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