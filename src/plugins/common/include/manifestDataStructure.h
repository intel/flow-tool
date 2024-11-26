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
 
# pragma once

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "graph.h"

//todo: support overwrite through config file.
#define MANIFEST_SCHEMA_FILE "/usr/etc/kit/schema/manifest.schema.json"

/** Meta data*/
class CManifestMetaData {
public:
    std::string date;
    std::string identifier;
    std::string version;
    std::string desc;
    std::string title;
    std::string name;
    
    std::string tojsonString();
    int setValues(nlohmann::json jTag);
};

class CManifestPropData {
public:
    int retry = 0;
    std::string order;
   
    std::string tojsonString();
    int setValues(nlohmann::json jTag);
};

/** Applicability data -- metrics list supported in discovery */
class CManifestApplicabilityData {
public:
    std::vector<std::string> vPlatforms;
    std::string os;
    std::vector<std::string> vDistros;
    std::string arch;
    std::string kernel;
    //std::string hw;
    //std::vector<std::string> vHW;

    std::string tojsonString();
    int setValues(nlohmann::json jTag);
};

/** act tag data*/
class CManifestActData {
public:
    std::string action;
    std::string param;
    std::string second_param;
    //std::vector<std::string> param;
    std::string path;
    std::string targetPath; //for file copy
    std::string remotePath;
    std::string reference;
    std::string onFailure;
    std::string onSuccess;    
    std::string expected; 
    std::string condition;
    std::string desc;
    
    std::string tojsonString();
    int setValues(nlohmann::json jTag);
};

/** refer tag data*/
class CManifestReferData {
public:
    std::string item;
    std::string value;
    std::string path;
    std::string param;
    std::vector<std::string> vCommands;
    std::string applicability;
    std::string remotePath;
    std::string reference;
    std::string onFailure;
    std::string onSuccess;
    
    //std::string getCommands();
    std::string tojsonString();
    //std::string setValues(std::string sTag);
    int setValues(nlohmann::json jTag);
};

/** graph datastructure -- to define different paths of action */
/*class CActionGraphNode;

class CActionGraphEdge {
    
public:
    CActionGraphNode* node_from;
    CActionGraphNode* node_to;
    std::string output;
    
    CActionGraphEdge(CActionGraphNode* node_from, CActionGraphNode* node_to, const std::string& output) : node_from(node_from), node_to(node_to), output(output) {}
};

class CActionGraphNode  {
    
public:
    std::string action;
    std::string param;
    
    CManifestActData* m_pActData;
    
    std::vector<std::shared_ptr<CActionGraphEdge>> edges;

    CActionGraphNode(const std::string& action, const std::string& param) : action(action), param(param){}
    CActionGraphNode(CManifestActData *pActData) : m_pActData(pActData){}

    void addEdge(std::shared_ptr<CActionGraphEdge> edge) {
        edges.push_back(edge);
    }

     std::vector<std::shared_ptr<CActionGraphEdge>> getEdges(){ 
        return edges; 
    }

};*/

/** manifest data */
class CPkgManifest {
private:

   //coverity 
  CPkgManifest(CPkgManifest const&) = delete;
  void operator=(CPkgManifest const&) = delete;

    int init();
    void deinit();
    
    /** generates action graph to handle path diversions (on sucess, on failure handlers....)*/
    //int flattenActionPath();
   // std::unordered_map<std::string, CActionGraphNode*> flattenActionPaths();
    void flattenActionPaths();
public:
    CPkgManifest();
    ~CPkgManifest();

    CGraph graph;
   
    CManifestMetaData *pPkgMetaData {0};
    CManifestPropData *pPkgPropData {0};
    CManifestApplicabilityData* pPkgApplicabilityData {0};
    std::vector<CManifestReferData*> vPkgReferData;
    //std::vector<CManifestActData*> vPkgReferData;
    std::vector<CManifestActData*> vPkgPreActData;
    std::vector<CManifestActData*> vPkgActData;
    std::vector<CManifestActData*> vPkgPostActData;
    std::map<std::string, std::vector<CManifestActData*>> vPkgFaiSucActData; //custom tags to jump from on_failure and on_success cases
    
    int WriteToFile(std::string filepath);
    int readFromFile(std::string filepath);
    NodeData* convertManifestToNodeData(const CManifestActData& manifestData);
};
