/*
 * Copyright (c) 2024, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */
// C++ Program to Implement a Graph Using Adjacency List
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>       // Stores key-value pairs and provides fast lookups.
#include <map>
#include <iostream>
#include <list>

#include <nlohmann/json.hpp>   // Parse and manage JSON objects.


using namespace std;
using json = nlohmann::json;   // Alias to simplify usage of the JSON library, enabling the type 'json' to refer to nlohmann::json.

/**
 * @struct NodeData
 * @brief Represents a node containing action details and transitions.
 */
struct NodeData {
    string action;                  // Action to be performed at this node.
    string param;                   // Parameters for the action.
    string onfailure_tag;           // Tag for the node to transition to on failure.
    string onsuccess_tag;           // Tag for the node to transition to on success.
};

/**
 * @struct Node
 * @brief Represents a node in the graph, containing data and next node.
 */
struct Node {
    NodeData* data;
    void* pDataRef;
    vector<Node*> next_nodes;       // List of subsequent nodes to traverse.
    //vector<Node*> vNextNodes;       // List of subsequent nodes to traverse.
};

/**
 * @class CGraph
 * @brief Representing a directed acyclic graph (DAG) for managing actions and transitions.
 */
class CGraph {

private:
    map<int, list<int> > adjList; // Adjacency list to store the graph

public:
    // Function to add an edge between vertices u and v of
    // the graph
    void add_edge(int u, int v)
    {
        // Add edge from u to v
        adjList[u].push_back(v);
    }

    // Function to print the adjacency list representation
    // of the graph
    void print()
    {
        cout << "Adjacency list for the Graph: " << endl;
        // Iterate over each vertex
        for (auto i : adjList) {
            // Print the vertex
            cout << i.first << " -> ";
            // Iterate over the connected vertices
            for (auto j : i.second) {
                // Print the connected vertex
                cout << j << " ";
            }
            cout << endl;
        }
    }


public:
    vector<Node*> actions;// List of actions represented as nodes in the graph.
    vector<Node*> vNodes; // Only nodes from Pre-Act, Act, Post-Act
    vector<Node*> vNodesTotal; // Total Nodes including Tags
    map<int, vector<Node*>> mapGraphAdjList; // Adjacency list to store the graph
    int adjListIndex = 0;

    Node* endNode = createNode(new NodeData{"End", "", "", ""});

    unordered_map<string, vector<Node*>> tagMap;     // Map associating tags with corresponding nodes.

    unordered_map<string, vector<Node*>> mapGraphList;
    int mapGraphListUniqueId = 0;

    /**
     * @brief Initializes the graph by reading and parsing a JSON file from the specified path.
     * @param file_path The path to the file in JSON format.
     * @return 0 on success, and non-zero on failure.
     */
    int init(const string& file_path);

    /**
     * @brief De-initializes the graph, freeing all dynamically allocated memory.
     */
    void deinit();

    /**
     * @brief Prints all nodes in the graph.
     */
    void print_all_nodes();

    /**
     * @brief Prints all paths in the graph from the start to the end.
     */
    void print_all_paths();

    /**
     * @brief Returns the next node based on the result of the previous node.
     * @param current The current node.
     * @param prev_node_result The result (success/failure) of the previous node.
     * @return The next node to be executed.
     */
    // Node* get_next_node(Node* current, const string& prev_node_result);

    Node* receive_next_node(Node* current, int result);
    Node* traverseAndFindNextNode(Node* node, Node* current, int result);

    int insertNewNode(void* data, NodeData* info);
    int insertNewNodeTags(void* data, const string& tagName, NodeData* info);

    /**
     * @brief Builds the directed acyclic graph (DAG) from the node structure.
     */
    int connectNodes();
    void buildDAG();

    void buildingDAG();

    /**
     * @brief Prints all nodes & paths in the graph from the start to the end.
     */
    int printGraph();
    void printgraphnodes();

protected: //for unit test case access

    int printNode(Node* curNode);

    void markAsLastEdge(Node* from);

    /**
     * @brief Adds a directed edge from one node to another.
     * @param from The starting node.
     * @param to The destination node.
     */
    void addEdge(Node* from, Node* to);

    /**
     * @brief Creates a new node by passing a pointer to NodeData struct.
     * @param data Pointer to NodeData struct containing details.
     * @return A pointer to the newly created node.
     */
    Node* createNode(NodeData* data);

    /**
     * @brief Adds a tag to the node map.
     * @param tagArray Array of tags.
     * @param tagName Name of the tag.
     */
    void addTagToNodeMap(const json& tagArray, const string& tagName);

    /**
     * @brief Parses the JSON manifest and constructs nodes for the graph.
     * @param manifest The JSON object containing graph structure.
     */
    void parseJsonToNodes(const json& manifest);

    /**
     * @brief Handles tag transitions for a node.
     * @param current The current node.
     * @param tag The tag to handle.
     */
    void handleTag(Node* current, const string& tag);

    void handlingTag(Node* current, const string& tag);

    /**
     * @brief Traverses and prints all nodes in the graph.
     * @param nodeVectors A vector containing node vectors.
     */
    void traverseAndPrintAllNodes(vector<vector<Node*>>& nodeVectors);

    /**
     * @brief Traverses the graph and prints all possible paths.
     * @param node The current node being traversed.
     * @param path The current path being explored.
     */
    void traverseAndPrintPaths(Node* node, vector<string>& path);

};

#endif