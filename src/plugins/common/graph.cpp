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

#include <fstream>        // Provides functionality for file input and output, used to read the JSON manifest file.

#include "graph.h"      // Includes the header file defining the CGraph class.

/**
 * @brief Initializes the graph by reading and parsing a JSON file from the specified path.
 * @param file_path The path to the file in JSON format.
 * @return 0 on success, and non-zero on failure.
 */
int CGraph::init(const string& file_path) {
    ifstream file(file_path);

    // Check if the file can be opened
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << file_path << endl;
        return 1;  // Return error code 1 if file cannot be opened
    }

    json manifest;

    // Try parsing the JSON file
    try {
        file >> manifest;
    } catch (const nlohmann::json::parse_error& e) {
        cerr << "JSON parsing error in file " << file_path << ": " << e.what() << endl;
        file.close();  // Ensure the file is closed on error
        return 2;  // Return error code 2 if there is a parsing issue
    }

    // Close the file after successfully reading it
    file.close();

    // Call the function to parse the JSON into graph nodes
    try {
        parseJsonToNodes(manifest);  // This function may also throw exceptions
    } catch (const std::exception& e) {
        std::cerr << "Error during node parsing: " << e.what() << std::endl;
        return 3;  // Return error code 3 for node parsing errors
    }

    // Call the function to build the directed acyclic graph (DAG)
    try {
        buildDAG();
    } catch (const std::exception& e) {
        std::cerr << "Error during DAG construction: " << e.what() << std::endl;
        return 4;  // Return error code 4 for DAG construction errors
    }

    return 0;  // Return 0 for success
}

/**
 * @brief De-initializes the graph, freeing all dynamically allocated memory.
 */
void CGraph::deinit() {

    // Deallocate nodes inside tagMap's vectors
    for (auto& entry : tagMap) {
        if (!entry.second.empty()) {              // Ensure the vector is not empty before trying to delete
            for (Node* node : entry.second) {
                if (node) {                       // Ensure the node pointer is not null
                    delete node;                  // Delete each Node* inside the vector
                    node = nullptr;
                }
            }
            entry.second.clear();                 // Clear the vector after deleting its contents
        }
    }

    // Clear the entire tagMap after all nodes have been deallocated
    tagMap.clear();

    // Deallocate the endNode
    if (endNode) {
        delete endNode;
        endNode = nullptr;
    }

    // Clear the actions and tagMap to remove dangling pointers
    actions.clear();
}

/**
 * @brief Returns the next node based on the result of the previous node.
 * @param result will receive either 1 or 0. 0 means success and 1 means failure
 */
Node* CGraph::receive_next_node(Node* current, int result) {
    cout << endl;
    cout << "Current Node Executed: " << current->data->param << endl;
    cout << endl;

    return traverseAndFindNextNode(vNodes[0], current, result);
}

/**
 * @brief Helper function to perform the traversal
 * @param result will receive either 1 or 0. 0 means success and 1 means failure
 */
Node* CGraph::traverseAndFindNextNode(Node* node, Node* current, int result) {
    if (!node) return endNode; // Return endNode if node is null

    // If the current node is found, check for success or failure tags
    if (node == current) {
        // Case 1: If the node has onsuccess and the result is "success"
        // result == 0 means the result is "success"
        if (!node->data->onsuccess_tag.empty() && result == 0) {
            if (!node->next_nodes.empty()) {
                return node->next_nodes[0]; // Return the first next node
            } else {
                return endNode; // No next nodes, return endNode
            }
        }

        // Case 2: If the node has onfailure
        if (!node->data->onfailure_tag.empty()) {
            // result == 1 means the result is "failure"
            if (result == 1) {
                if (!node->next_nodes.empty()) {
                    return node->next_nodes[0]; // Return the first next node
                } else {
                    return endNode; // No next nodes, return endNode
                }
            }
            // result == 0 means the result is "success"
            else if (result == 0) {
                if (!node->next_nodes.empty()) {
                    return node->next_nodes[1]; // Return the second next node
                } else {
                    return endNode; // No next nodes, return endNode
                }
            }
        }

        // Case 3: If no tags match or no tags are present, return the next node
        if (!node->next_nodes.empty()) {
            return node->next_nodes[0]; // Return the first next node
        } else {
            return endNode; // No next nodes, return endNode
        }
    }

    // Recursively search in next nodes
    for (Node* next : node->next_nodes) {
        Node* resultNode = traverseAndFindNextNode(next, current, result);
        if (resultNode != nullptr && resultNode != endNode) {
            return resultNode; // Found the next node, return it
        }
    }

    return endNode; // If no next node is found, return endNode
}

/**
 * @brief Returns the next node based on the result of the previous node.
 * @param current The current node.
 * @param prev_node_result The result (success/failure) of the previous node.
 * @return The next node to be executed.
 */
// Node* CGraph::get_next_node(Node* current, const string& prev_node_result) {

//     if (!current) return nullptr;

//     if (!current->data->onsuccess_tag.empty() && prev_node_result == "success") {
//         // Look up the onsuccess tag in the mapGraphList to get the next node
//         if (mapGraphList.find(current->data->onsuccess_tag) != mapGraphList.end()) {
//             return mapGraphList[current->data->onsuccess_tag][0];
//         }
//     }

//     if (!current->data->onfailure_tag.empty()) {
//         if (prev_node_result == "failure") {
//             // If the previous result is "failure", follow the onfailure path
//             if (mapGraphList.find(current->data->onfailure_tag) != mapGraphList.end()) {
//                 return mapGraphList[current->data->onfailure_tag][0];
//             }
//         } else if (prev_node_result == "success") {
//             // If the previous result is "success", check the success path instead
//             if (!current->data->onsuccess_tag.empty() && mapGraphList.find(current->data->onsuccess_tag) != mapGraphList.end()) {
//                 return mapGraphList[current->data->onsuccess_tag][0];
//             }
//         }
//     }

//     // If no valid onsuccess/onfailure tags, move to the next sequential node in the vNodes array
//     for (size_t i = 0; i < vNodes.size(); ++i) {
//         if (vNodes[i] == current && i + 1 < vNodes.size()) {
//             return vNodes[i + 1];
//         }
//     }

//     // Handle the case where the current node comes from a tag
//     for (const auto& [tag, tagActions] : mapGraphList) {
//         // Check if the current node is in this tag's action list
//         for (size_t i = 0; i < tagActions.size(); ++i) {
//             if (tagActions[i] == current) {
//                 if (!current->data->onsuccess_tag.empty() && prev_node_result == "success") {
//                     // Look up the onsuccess tag in the mapGraphList to get the next node
//                     if (mapGraphList.find(current->data->onsuccess_tag) != mapGraphList.end()) {
//                         return mapGraphList[current->data->onsuccess_tag][0];
//                     }
//                 }
//                 if (!current->data->onfailure_tag.empty()) {
//                     if (prev_node_result == "failure") {
//                         // If the previous result is "failure", follow the onfailure path
//                         if (mapGraphList.find(current->data->onfailure_tag) != mapGraphList.end()) {
//                             return mapGraphList[current->data->onfailure_tag][0];
//                         }
//                     } else if (prev_node_result == "success") {
//                         // If the previous result is "success", check the success path
//                         if (!current->data->onsuccess_tag.empty() && mapGraphList.find(current->data->onsuccess_tag) != mapGraphList.end()) {
//                             return mapGraphList[current->data->onsuccess_tag][0];
//                         }
//                     }
//                 }

//                 // If the node has no success/failure tags
//                 if (i + 1 < tagActions.size()) {
//                     return tagActions[i + 1];
//                 }

//                 // If the current node is the last in the tag array, return the end node
//                 return endNode;
//             }
//         }
//     }
//     return endNode;
// }

/**
 * @brief Adds a directed edge from one node to another.
 * @param from The starting node.
 * @param to The destination node.
 */
void CGraph::addEdge(Node* from, Node* to) {
    // Check if the edge already exists
    for (Node* next : from->next_nodes) {
        if (next == to) {
            // cout << "already connected" << endl;
            return;
        }
    }
    // Add the edge if it doesn't exist
    from->next_nodes.push_back(to);

    return;
}

void CGraph::markAsLastEdge(Node* from) {
    Node* node = new Node();
    node->data = NULL;
    from->next_nodes.push_back(NULL);
}

/**
 * @brief Creates a new node by passing a pointer to NodeData struct.
 * @param data Pointer to NodeData struct containing details.
 * @return A pointer to the newly created node.
 */
Node* CGraph::createNode(NodeData* data) {
    Node* node = new Node();
    node->data = data;
    return node;              // Return the newly created node
}

/**
 * @brief Adds a tag to the node map.
 * @param tagArray Array of tags.
 * @param tagName Name of the tag.
 */
void CGraph::addTagToNodeMap(const json& tagArray, const string& tagName) {
    for (const auto& act : tagArray) {
        string actionName = act["action"];
        string param = act["param"];
        string onfailure = act.contains("onfailure") ? act["onfailure"].get<string>() : "";
        string onsuccess = act.contains("onsuccess") ? act["onsuccess"].get<string>() : "";

        Node* node = createNode(new NodeData{actionName, param, onfailure, onsuccess});

        // Append the node to the vector for this tag in the map
        tagMap[tagName].push_back(node);
    }
}

/**
 * @brief Parses the JSON manifest and constructs nodes for the graph.
 * @param manifest The JSON object containing graph structure.
 */
void CGraph::parseJsonToNodes(const json& manifest) {
    // Loop through the 'act' array
    for (const auto& action : manifest["act"]) {
        string actionName = action["action"];
        string param = action["param"];
        string onfailure = action.contains("onfailure") ? action["onfailure"].get<string>() : "";
        string onsuccess = action.contains("onsuccess") ? action["onsuccess"].get<string>() : "";

        Node* node = createNode(new NodeData{actionName, param, onfailure, onsuccess});
        actions.push_back(node);

        // Add node to the tag map
        tagMap[actionName + "_" + param].push_back(node);
    }
    // Handle the failure and success tags outside the 'act' array
    for (const auto& [tag, nodes] : manifest.items()) {
        if (tag != "act" && tag != "meta") {
            addTagToNodeMap(nodes, tag);
        }
    }
}

/** returns 0 on success*/
int CGraph::insertNewNode(void* data, NodeData* info) {

    Node* node = new Node();
    if(node) {
        node->data = info;
        node->pDataRef = data;
        vNodesTotal.push_back(node);
        vNodes.push_back(node);

        mapGraphList[std::to_string(mapGraphListUniqueId)].push_back(node);
        mapGraphListUniqueId = mapGraphListUniqueId + 1;
        return 0;
    }

    //cout << "error: creating node" << endl;
    return -1;
}

int CGraph::insertNewNodeTags(void* data, const string& tagName, NodeData* info) {

    Node* node = new Node();
    if(node) {
        node->data = info;
        node->pDataRef = data;
        vNodesTotal.push_back(node);
        mapGraphList[tagName].push_back(node);
        return 0;
    }

    //cout << "error: creating node" << endl;
    return -1;
}

/**
 * @brief Builds the directed acyclic graph (DAG) from the node structure.
 */
int CGraph::connectNodes() {
    for (size_t i = 0; i < vNodesTotal.size(); ++i) {
        Node* current = vNodesTotal[i];
        if (i + 1 < vNodesTotal.size()) {
            addEdge(current, vNodesTotal[i + 1]);
        }
    }
    mapGraphAdjList[adjListIndex++] = vNodesTotal;
    // cout << "mapGraphAdjList Size: " << mapGraphAdjList.size() << endl;
    //cout << "nodes connected" << endl;

    return 0;
}

/** if next_node is not null then recursive call will be made */
int CGraph::printNode(Node* curNode) {
    if(curNode != NULL) {
        //cout << "next_nodes_size = " << curNode->next_nodes.size() << endl;
        for (Node* next_node : curNode->next_nodes) {
            if(next_node != NULL /*&& next_node->pDataRef != NULL*/) {
                 cout << " next node <" << next_node->pDataRef << ">" ;
                 printNode(next_node);//recursive
            } else {
                cout << "; error -invalid data "<< endl;
            }
        }
    }

    return 0;
}

void CGraph::handlingTag(Node* current, const string& tag) {
    // Check if the tag exists in the map
    if (mapGraphList.find(tag) != mapGraphList.end()) {
        // Get the corresponding list of nodes for the tag
        vector<Node*>& totalTagActions = mapGraphList[tag];

        // Build a DAG for the actions inside the tag
        for (size_t i = 0; i < totalTagActions.size(); ++i) {
            Node* currentTagNode = totalTagActions[i];

            if (currentTagNode->data->onfailure_tag.empty() && currentTagNode->data->onsuccess_tag.empty()){
                if (i + 1 < totalTagActions.size()) {
                    addEdge(currentTagNode, totalTagActions[i + 1]);
                } else {
                    addEdge(currentTagNode, endNode);
                }
            }

            // Handle failure paths for actions within the tag
            if (!currentTagNode->data->onfailure_tag.empty()) {
                handlingTag(currentTagNode, currentTagNode->data->onfailure_tag);

                // Handle path (go to the next node)
                if (i + 1 < totalTagActions.size()) {
                    addEdge(currentTagNode, totalTagActions[i + 1]);
                } else {
                    addEdge(currentTagNode, endNode);
                }
            }

            if (!currentTagNode->data->onsuccess_tag.empty()) {
                handlingTag(currentTagNode, currentTagNode->data->onsuccess_tag);
            }
        }

        // Link the original node to the first action of the tag
        if (!totalTagActions.empty()) {
            addEdge(current, totalTagActions[0]);
        }
    }
}

void CGraph::buildingDAG() {
    for (size_t i = 0; i < vNodes.size(); ++i) {
        Node* current = vNodes[i];

        // If no success/failure tags, just go to the next node
        if (current->data->onfailure_tag.empty() && current->data->onsuccess_tag.empty()) {
            if (i + 1 < vNodes.size()) {
                addEdge(current, vNodes[i + 1]);
            } else {
                addEdge(current, endNode);
            }
        }

        // Handle failure paths
        if (!current->data->onfailure_tag.empty()) {
            // Handle the failure path by recursively following the tag
            handlingTag(current, current->data->onfailure_tag);

            // Handle path (go to the next node if not defined)
            if (i + 1 < vNodes.size()) {
                addEdge(current, vNodes[i + 1]);  // Success leads to next action
            } else {
                addEdge(current, endNode);  // If last action, link to "End"
            }
        }

        if (!current->data->onsuccess_tag.empty()) {
            // Handle success path by recursively following the tag
            handlingTag(current, current->data->onsuccess_tag);
        }
    }
}

int CGraph::printGraph() {

    /*cout << "print nodes" << endl;
    for (size_t i = 0; i < vNodesTotal.size(); ++i) {
        Node* curNode = vNodesTotal[i];
        if(curNode != NULL) {
            cout << "index: " << i << "; data Reference = " << curNode->pDataRef << endl;
        } else {
            cout << "index: " << i << "; error -invalid data " << endl;
        }
    }*/

    cout << "print adj list" << endl;
    // cout << "Size of Adj List " << adjListIndex << endl;
    for (int j = 0; j < adjListIndex; j++) {
        vector<Node*> pVecNode = mapGraphAdjList[j];
        // cout << "Vetor Size " << pVecNode.size() << endl;
        Node* curNode = pVecNode[0];
        cout << "first node <" << curNode->pDataRef << ">" ;
        printNode(curNode);
        cout << endl;
    }

    cout << "Total Map Size " << mapGraphList.size() << endl;
    cout << "Total Vector Size " << vNodesTotal.size() << endl;
    cout << "Vector Size " << vNodes.size() << endl;

    // cout << "Printing Map " << endl;
    // for (const auto& pair : mapGraphList) {
    //     std::cout << "Tag: " << pair.first << " Actions: ";
    //     std::cout << std::endl;

    // }

    // cout << "Printing Vector: " << endl;
    // for (const auto& element : vNodesTotal) {
    //     std::cout << element << " ";
    // }
    // std::cout << std::endl;

    return 0;
}

void CGraph::printgraphnodes() {

    // Printing the Total Nodes Vector
    for (const auto& node : vNodesTotal) {
        if (node != nullptr) {
            std::cout << node->data->action << " " << node->data->param << " -> ";
        }
    }
    cout << "END";
    cout << endl;
}

/**
 * @brief Handles tag transitions for a node.
 * @param current The current node.
 * @param tag The tag to handle.
 */
void CGraph::handleTag(Node* current, const string& tag) {
    // Check if the tag exists in the map
    if (tagMap.find(tag) != tagMap.end()) {
        // Get the corresponding list of nodes for the tag
        vector<Node*>& tagActions = tagMap[tag];

        // Build a DAG for the actions inside the tag
        for (size_t i = 0; i < tagActions.size(); ++i) {
            Node* currentTagNode = tagActions[i];

            if (currentTagNode->data->onfailure_tag.empty() && currentTagNode->data->onsuccess_tag.empty()){
                if (i + 1 < tagActions.size()) {
                    addEdge(currentTagNode, tagActions[i + 1]);
                } else {
                    addEdge(currentTagNode, endNode);
                }
            }

            // Handle failure paths for actions within the tag
            if (!currentTagNode->data->onfailure_tag.empty()) {
                handleTag(currentTagNode, currentTagNode->data->onfailure_tag);

                // Handle path (go to the next node)
                if (i + 1 < tagActions.size()) {
                    addEdge(currentTagNode, tagActions[i + 1]);
                } else {
                    addEdge(currentTagNode, endNode);
                }
            }

            if (!currentTagNode->data->onsuccess_tag.empty()) {
                handleTag(currentTagNode, currentTagNode->data->onsuccess_tag);
            }
        }

        // Link the original node to the first action of the tag
        if (!tagActions.empty()) {
            addEdge(current, tagActions[0]);
        }
    }
}

/**
 * @brief Builds the directed acyclic graph (DAG) from the node structure.
 */
void CGraph::buildDAG() {
    for (size_t i = 0; i < actions.size(); ++i) {
        Node* current = actions[i];

        // If no success/failure tags, just go to the next node
        if (current->data->onfailure_tag.empty() && current->data->onsuccess_tag.empty()) {
            if (i + 1 < actions.size()) {
                addEdge(current, actions[i + 1]);
            } else {
                addEdge(current, endNode);
            }
        }

        // Handle failure paths
        if (!current->data->onfailure_tag.empty()) {
            // Handle the failure path by recursively following the tag
            handleTag(current, current->data->onfailure_tag);

            // Handle path (go to the next node if not defined)
            if (i + 1 < actions.size()) {
                addEdge(current, actions[i + 1]);  // Success leads to next action
            } else {
                addEdge(current, endNode);  // If last action, link to "End"
            }
        }

        if (!current->data->onsuccess_tag.empty()) {
            // Handle success path by recursively following the tag
            handleTag(current, current->data->onsuccess_tag);
        }
    }
}

/**
 * @brief Prints all nodes in the graph.
 */
void CGraph::print_all_nodes() {
    try {
        // Check if tagMap is empty
        if (tagMap.empty()) {
            std::cerr << "Warning: No nodes to print. The graph is empty." << std::endl;
            return;
        }
        cout << "Printing All Nodes:" << endl;
        std::vector<std::vector<Node*>> nodeVectors;

        // Collect nodes from tagMap into nodeVectors
        for (const auto& [tag, nodes] : tagMap) {
            if (nodes.empty()) {
                std::cerr << "Warning: No nodes associated with tag " << tag << std::endl;
            }
            // Add nodes to the nodeVectors
            nodeVectors.push_back(nodes);
        }

        // Print all nodes by traversing the vectors
        traverseAndPrintAllNodes(nodeVectors);
        cout << "Printing All Possible Path: " << endl;

    } catch (const std::exception& e) {
        std::cerr << "Error while printing nodes: " << e.what() << std::endl;
    }
}

/**
 * @brief Prints all paths in the graph from the start to the end.
 */
void CGraph::print_all_paths() {

    try {
        // Check if the actions array is empty
        if (vNodes.empty()) {
            std::cerr << "Warning: No actions available to print paths. The graph is empty." << std::endl;
            return;
        }

        std::vector<std::string> path;

        // Traverse and print all paths starting from the first action node
        traverseAndPrintPaths(vNodes[0], path);

    } catch (const std::exception& e) {
        std::cerr << "Error while printing paths: " << e.what() << std::endl;
    }
}

/**
 * @brief Traverses and prints all nodes in the graph.
 * @param nodeVectors A vector containing node vectors.
 */
void CGraph::traverseAndPrintAllNodes(vector<vector<Node*>>& nodeVectors) {
    // Start printing from the last vector in nodeVectors
    for (auto it = nodeVectors.rbegin(); it != nodeVectors.rend(); ++it) {
        const vector<Node*>& vec = *it;

        for (size_t i = 0; i < vec.size(); ++i) {
            cout << vec[i]->data->action << " " << vec[i]->data->param << endl;  // Print each element on a new line
        }
    }
    cout << endl;
}

/**
 * @brief Traverses the graph and prints all possible paths.
 * @param node The current node being traversed.
 * @param path The current path being explored.
 */
void CGraph::traverseAndPrintPaths(Node* node, vector<string>& path) {
    if (!node) return;

    // Add current node to the path
    path.push_back(node->data->action + " " + node->data->param);

    // If this is a leaf node, print the path
    if (node->next_nodes.empty()) {
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i < path.size() - 1) {
                cout << " -> ";
            }
        }
        cout << endl;
    } else {
        // Recursively visit all next nodes
        for (Node* next : node->next_nodes) {
            traverseAndPrintPaths(next, path);
        }
    }
    // Backtrack: Remove the current node from the path before going back up
    path.pop_back();
}