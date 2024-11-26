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

#include <iostream>        // Provides standard input-output functionality for console output in tests.
#include <cassert>         // Includes support for assertions to validate test conditions.

#include "graph.h"       // Includes the CGraph class and Node structure for graph management.

/**
 * @class CGraphTest
 * @brief Test class to validate the functionality of the CGraph class.
 *
 * This class provides unit tests for the various methods in the CGraph class, including node creation,
 * edge addition, JSON parsing, and Directed Acyclic Graph (DAG) construction.
 */
class CGraphTest : private CGraph {
public:

    /**
     * @brief Test to check the creation of a node in the graph.
     *
     * This function validates that the `createNode` method correctly initializes a Node object
     * with the given action, parameter, and tags.
     */
    void testCreateNode() {
        NodeData* data = new NodeData{"install", "gimp", "", ""};
        Node* node = createNode(data);
        std::cout << "Node action: " << node->data->action << ", param: " << node->data->param << std::endl;
        assert(node->data->action == "install");
        assert(node->data->param == "gimp");
        std::cout << "testCreateNode passed!" << std::endl;
    }

    /**
     * @brief Test to check if an edge between two nodes is created correctly.
     *
     * This function verifies that the `addEdge` method successfully links two nodes in the graph.
     */
    void testAddEdge() {
        NodeData* data1 = new NodeData{"install", "gimp", "", ""};
        NodeData* data2 = new NodeData{"install", "vlc", "", ""};
        Node* node1 = createNode(data1);
        Node* node2 = createNode(data2);
        addEdge(node1, node2);

        assert(node1->next_nodes.size() == 1);
        assert(node1->next_nodes[0] == node2);
        std::cout << "testAddEdge passed!" << std::endl;
    }

    /**
     * @brief Test to check if a JSON manifest is correctly parsed into graph nodes.
     *
     * This function validates that the `parseJsonToNodes` method properly constructs graph nodes
     * from a JSON object.
     */
    void testParseJsonToNodes() {
        std::string manifest_str = R"({
            "act": [
                {"action": "install", "param": "gimp"},
                {"action": "install", "param": "vlc", "onfailure": "failure_tag"}
            ]
        })";

        json manifest = json::parse(manifest_str);

        parseJsonToNodes(manifest);

        assert(actions.size() == 2);
        assert(actions[0]->data->action == "install");
        assert(actions[0]->data->param == "gimp");
        assert(actions[1]->data->action == "install");
        assert(actions[1]->data->param == "vlc");
        assert(actions[1]->data->onfailure_tag == "failure_tag");
        std::cout << "testParseJsonToNodes passed!" << std::endl;
    }

    /**
     * @brief Test to validate if the DAG is correctly built from the node structure.
     *
     * This function checks that the `buildDAG` method correctly sets up the relationships between nodes.
     */
    void testBuildDAG() {
        std::string manifest_str = R"({
            "act": [
                {"action": "install", "param": "gimp"},
                {"action": "install", "param": "vlc", "onfailure": "failure_tag"}
            ]
        })";

        json manifest = json::parse(manifest_str);

        parseJsonToNodes(manifest);
        buildDAG();

        assert(actions[0]->next_nodes.size() == 1);
        assert(actions[0]->next_nodes[0] == actions[1]);
        std::cout << "testBuildDAG passed!" << std::endl;
    }
};

/**
 * @brief Entry function for the test program.
 *
 * Executes the various test functions to validate the functionality of the CGraph class.
 * @return 0 on successful execution of all tests.
 */
int main() {
    CGraphTest graphTest;
    graphTest.testCreateNode();
    graphTest.testAddEdge();
    graphTest.testParseJsonToNodes();
    graphTest.testBuildDAG();
 
    std::cout << "All tests passed!" << std::endl;
    return 0;
}