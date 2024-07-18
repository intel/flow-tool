/*
 * Copyright (c) 2024, Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 * http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Node;

class Edge {
public:
    Node* node_from;
    Node* node_to;
    std::string output;
    Edge(Node* node_from, Node* node_to, const std::string& output) : node_from(node_from), node_to(node_to), output(output) {}

};

class Node  {
public:
    std::string action;
    std::string param;
    std::vector<Edge*> edges;
    Node(const std::string& action, const std::string& param) : action(action), param(param){}

    void addEdge(Edge* edge) {
        edges.push_back(edge);
    }

    std::vector<Edge*> getEdges(){ 
        return edges; 
    }
};

std::unordered_map<std::string, Node*> flattenActionPaths(const std::string& fileinfo) {
    std::unordered_map<std::string, Node*> actions_dict;
    try {
        json json_data = json::parse(fileinfo);
        std::vector<json> actions_list = json_data["act"];

        for (int i = actions_list.size() - 1; i >= 0; i--) {
            json action_data = actions_list[i];
            std::string action = action_data["action"];
            std::string param = action_data["param"];
            Node* curr_node = new Node(action, param);
            actions_dict[action] = curr_node;
            std::cout << "Current action: " << action << std::endl;
            for (auto it = action_data.begin(); it != action_data.end(); ++it) {
                if (it.key() != "action" && it.key() != "param") {
                    std::string other_action = it.value();
                    Node* other_node = actions_dict[other_action];
                    Edge* edge = new Edge(curr_node, other_node, it.key());
                    std::cout << "Other action: " << other_action << ", Tag: " << it.key() << std::endl;
                    curr_node->addEdge(edge);
                }
            }
        }
    }
    catch (const json::exception& e) {
        std::cerr << "Error parsing JSON:" << e.what() << std::endl;
    }
    return actions_dict;
}

int main() {
    std::string fileinfo = R"({
        "act": [
            {"action": "INCLUDE_MANIFEST", "param": "manifest/ppd-manifest.json", "onsuccess": "STOP", "on_failure": "CONTINUE"},
            {"action": "STOP", "param": "manifest/ppd-manifest.json"},
            {"action": "CONTINUE", "param": "manifest/ppd-manifest.json"}
        ]
    })";
    std::unordered_map<std::string, Node*> actions_dict = flattenActionPaths(fileinfo);
    for (const auto& pair : actions_dict) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
}
