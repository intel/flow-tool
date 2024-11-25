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

#include<fstream>
#include "validator.h"
#include "manifestDataStructure.h"
#include "graph.h"

CPkgManifest::CPkgManifest() {
    init();
}

CPkgManifest::~CPkgManifest() {
    deinit();
}

/**
 * @brief Initializes the CPkgManifest object.
  * @return An integer value indicating the success or failure of the initialization process.
 */
int CPkgManifest::init() {
    return 0;
}

/**
 * @brief Deinitializes the CPkgManifest object by deleting all allocated memory and clearing data structures.
 */
void CPkgManifest::deinit() {
    if(pPkgMetaData) {
        delete pPkgMetaData;
        pPkgMetaData = NULL;
    }
    
    if(pPkgApplicabilityData) {
        delete pPkgApplicabilityData;
        pPkgApplicabilityData = NULL;
    }
    
    if(pPkgPropData) {
        delete pPkgPropData;
        pPkgPropData = NULL;
    }
    
    std::vector<CManifestReferData*>::iterator r_it;
    for(r_it = vPkgReferData.begin(); r_it != vPkgReferData.end(); r_it++ )    {
        CManifestReferData *pRefData = (CManifestReferData*)*r_it;
        if(pRefData) {
            delete pRefData;
            pRefData = NULL;
        }
    }
    vPkgReferData.clear();
    
    std::vector<CManifestActData*>::iterator it;
    for(it = vPkgPreActData.begin(); it != vPkgPreActData.end(); it++ )    {
        CManifestActData *pData = (CManifestActData*)*it;
        if(pData) {
            delete pData;
            pData = NULL;
        }
    }
    vPkgPreActData.clear();
    
    
    //std::vector<CManifestActData*>::iterator it;
    for(it = vPkgActData.begin(); it != vPkgActData.end(); it++ )    {
        CManifestActData *pData = (CManifestActData*)*it;
        if(pData) {
            delete pData;
            pData = NULL;
        }
    }
    vPkgActData.clear();
    
    //std::vector<CManifestActData*>::iterator it;
    for(it = vPkgPostActData.begin(); it != vPkgPostActData.end(); it++ )    {
        CManifestActData *pData = (CManifestActData*)*it;
        if(pData) {
            delete pData;
            pData = NULL;
        }
    }
    vPkgPostActData.clear();            
                
    return;
}

/*****************************************************************************/

/**
 * Converts the manifest metadata to a JSON string representation.
 *
 * @return The JSON string representation of the manifest metadata.
 */
std::string CManifestMetaData::tojsonString() {
    
    nlohmann::json jData;
    if(!date.empty()) {jData["build"]["date"] = date;}
    if(!identifier.empty()) {jData["id"] = identifier;}
    if(!version.empty()) {jData["version"] = version;}
    if(!desc.empty()) {jData["desc"] = desc;}
    if(!title.empty()) {jData["title"] = title;}
    if(!name.empty()) {jData["name"] = name;}
    
    nlohmann::json jTag;
    jTag["meta"] = jData;
    
    return jTag.dump();
}

/**
 * @brief Sets the values of the manifest metadata based on the provided JSON object.
 * @param jTag The JSON object containing the manifest metadata.
 * @return 0 if the values are successfully set, otherwise an error code.
 */
int CManifestMetaData::setValues(nlohmann::json jTag) {
    if(jTag.contains("build")) {
        nlohmann::json jbuildtag = jTag.at("build");
        if(jbuildtag.contains("date")) { date = jbuildtag.at("date").get<std::string>(); }
    }
    if(jTag.contains("title")) { title = jTag.at("title").get<std::string>(); }
    if(jTag.contains("name")) { title = jTag.at("name").get<std::string>(); }
    if(jTag.contains("desc")) { desc = jTag.at("desc").get<std::string>(); }
    if(jTag.contains("version")) { version = jTag.at("version").get<std::string>(); }
    //if(jTag.contains("id")) { identifier = jTag.at("id").get<std::string>(); }
    jTag.at("id").get_to(identifier);
    
    return 0;
}

/***********************************************************************************/

std::string CManifestApplicabilityData::tojsonString() {
    
    nlohmann::json jData;
    
    jData["os"] = os;
    jData["arch"] = arch;
    
    /*std::stringstream ssPlatform; 
    for (size_t i = 0; i < vPlatforms.size(); i++) {
        if (i != 0) {
            ssPlatform << ", ";
        }
        ssPlatform << vPlatforms[i];
    }    
    jData["platform"] = ssPlatform.str();*/
    nlohmann::json jPlatformArray = vPlatforms;
    jData["platform"] =jPlatformArray;
 
    nlohmann::json jDistroArray = vDistros;
    jData["distro"] = jDistroArray;
    
    nlohmann::json jappTag;
    jappTag["applicability"] = jData;
    
    return jappTag.dump();
}

int CManifestApplicabilityData::setValues(nlohmann::json jTag) {
    
    if(jTag.contains("OS")) { os= jTag.at("OS").get<std::string>(); }
    if(jTag.contains("kernel")) {
       kernel = jTag.at("kernel").get<std::string>();
  
      } 
    if(jTag.contains("arch")) { arch = jTag.at("arch").get<std::string>(); }
    if(jTag.contains("platform")) {
        auto items = jTag.at("platform").get<std::vector<std::string>>();
        for (auto item : items) {
            vPlatforms.push_back(item);
        }
    }
    if(jTag.contains("distro")) {
        auto items = jTag.at("distro").get<std::vector<std::string>>();
        for (auto item : items) {
            vDistros.push_back(item);
        }
    }
    /*if(jTag.contains("HW")) {
        auto items = jTag.at("HW").get<std::vector<std::string>>();
        for (auto item : items) {
            vHW.push_back(item);
        }
    }*/
        
    return 0;
}

/***************************************************************************************/

std::string CManifestPropData::tojsonString() {
    nlohmann::json jData;
    jData["retry"] = retry;
    return jData.dump();
}

int CManifestPropData::setValues(nlohmann::json jTag) {
    if(jTag.contains("retry")) { retry = jTag.at("retry").get<std::int16_t>(); }
    return 0;
}

/***************************************************************************************/
int CManifestActData::setValues(nlohmann::json jTag) {
    try {
        if(jTag.contains("action")) { action = jTag.at("action").get<std::string>(); /*std::cout << param << std::endl;*/}
        if(jTag.contains("param")) { param = jTag.at("param").get<std::string>(); /*std::cout << param << std::endl;*/}
        if(jTag.contains("param2")) { second_param = jTag.at("param2").get<std::string>(); /*std::cout << param2 << std::endl;*/}
        //if(jTag.contains("param2")) { second_param = jTag.at("param2").get<std::vector<std::string>>(); }
        if(jTag.contains("path")) { path = jTag.at("path").get<std::string>(); }
        if(jTag.contains("desc")) { desc = jTag.at("desc").get<std::string>(); }
        if(jTag.contains("target_path")) { targetPath = jTag.at("target_path").get<std::string>(); }
        if(jTag.contains("remote_path")) { remotePath = jTag.at("remote_path").get<std::string>(); }
        if(jTag.contains("reference")) { reference = jTag.at("reference").get<std::string>(); }
        if(jTag.contains("on_failure")) { onFailure = jTag.at("on_failure").get<std::string>(); }
        if(jTag.contains("on_success")) { onSuccess = jTag.at("on_success").get<std::string>(); }
        expected = "0";
        if(jTag.contains("expected")) { expected = jTag.at("expected").get<std::string>(); }
    
	if(jTag.contains("condition")) { expected = jTag.at("condition").get<std::string>(); }    
    } catch (const std::exception &e) {
        std::cout << e.what() << "error set values" << std::endl;        
        return 1;
    }
    
    return 0;
}

std::string CManifestActData::tojsonString() {
    
    nlohmann::json jData;
    
    jData["action"] = action;
    jData["param"] = param;
    jData["param2"] = second_param;
    jData["path"] = path;
    jData["target_path"] = targetPath;    
    jData["remote_path"] = remotePath;
    jData["reference"] = reference;
    jData["on_failure"] = onFailure;
    jData["onSuccess"] = onSuccess;
    jData["expected"] = expected;    
    jData["condition"] = condition;     
        
    return jData.dump();
}

int CManifestReferData::setValues(nlohmann::json jTag) {
    if(jTag.contains("item")) { item = jTag.at("item").get<std::string>(); }
    if(jTag.contains("param")) { param = jTag.at("param").get<std::string>(); }
    if(jTag.contains("path")) { path = jTag.at("path").get<std::string>(); }
    if(jTag.contains("remote_path")) { remotePath = jTag.at("remote_path").get<std::string>(); }
    if(jTag.contains("reference")) { reference = jTag.at("reference").get<std::string>(); }
    if(jTag.contains("on_failure")) { onFailure = jTag.at("on_failure").get<std::string>(); }
    if(jTag.contains("on_success")) { onSuccess = jTag.at("on_success").get<std::string>(); }
    
    return 0;
}

std::string CManifestReferData::tojsonString() {
    
    nlohmann::json jData;
    
    jData["item"] = item;
    nlohmann::json jCommandArray = vCommands;
    jData["command"] =jCommandArray;
    jData["param"] = param;    
    jData["path"] = path;
    jData["applicability"] = applicability;    
    jData["remote_path"] = remotePath;
    jData["reference"] = reference;
    jData["on_failure"] = onFailure;
    jData["on_success"] = onSuccess;
        
    return jData.dump();
}

/**
 * Flattens the action paths in the package manifest.
 * This function creates a dictionary of actions and their corresponding nodes,
 * and adds edges between nodes based on their relationships.
 */
void CPkgManifest::flattenActionPaths() {
    
    cout << endl;
    cout << "Graph Building ---- Printing All Nodes" << endl;
    graph.printgraphnodes();

    cout << endl;
    cout << "Graph Building ---- Printing All Paths" << endl;

    graph.buildingDAG();
    graph.print_all_paths();
    cout << endl;
}


/**
 * Writes the package manifest data to a file.
 *
 * @param filepath The path of the file to write the manifest data to.
 * @return Returns 0 if the manifest data is successfully written to the file, otherwise returns 1.
 */
int CPkgManifest::WriteToFile(std::string filepath) {
    
    std::ofstream outFile;
    outFile.open(filepath.c_str(), std::ofstream::out);
    if (outFile.is_open()) {
        try {
            //meta tag
            if(pPkgMetaData) {
                outFile << pPkgMetaData->tojsonString() << "," << std::endl;
            }
            
            //applicability tag
            if(pPkgApplicabilityData) {
                outFile << pPkgApplicabilityData->tojsonString() << "," << std::endl;
            }

            //refer tag
            nlohmann::json jreferDataArray, jReferTag;
            std::vector<CManifestReferData*>::iterator it;
            int i = 0;
            for(it = vPkgReferData.begin(); it != vPkgReferData.end(); it++,i++ )    {
                CManifestReferData *pData = (CManifestReferData*)*it;
                nlohmann::json item = pData->tojsonString();
                jreferDataArray.push_back(item);
            }
            jReferTag["refer"] = jreferDataArray;
            outFile << jReferTag.dump()  << "," << std::endl;
 
            //preact tag
            nlohmann::json jpreactDataArray, jpreactTag;
            std::vector<CManifestActData*>::iterator preact_it;
            i = 0;
            for(preact_it = vPkgPreActData.begin(); preact_it != vPkgPreActData.end(); preact_it++,i++ )    {
                CManifestActData *pData = (CManifestActData*)*preact_it;
                nlohmann::json item = pData->tojsonString();
                jpreactDataArray.push_back(item);
            }
            jpreactTag["preact"] = jpreactDataArray;
            outFile << jpreactTag.dump()  << "," << std::endl;
            
            //act tag
            nlohmann::json jactDataArray, jactTag;
            std::vector<CManifestActData*>::iterator act_it;
            i = 0;
            for(act_it = vPkgActData.begin(); act_it != vPkgActData.end(); act_it++,i++ )    {
                CManifestActData *pData = (CManifestActData*)*act_it;
                nlohmann::json item = pData->tojsonString();
                jactDataArray.push_back(item);
            }
            jactTag["act"] = jactDataArray;
            outFile << jactTag.dump()  << "," << std::endl;
            
            //postact tag
            nlohmann::json jpostactDataArray, jpostactTag;
            std::vector<CManifestActData*>::iterator postact_it;
            i = 0;
            for(postact_it = vPkgPostActData.begin(); postact_it != vPkgPostActData.end(); postact_it++,i++ )    {
                CManifestActData *pData = (CManifestActData*) *postact_it;
                nlohmann::json item = pData->tojsonString();
                jpostactDataArray.push_back(item);
            }
            jpostactTag["postact"] = jpostactDataArray;
            outFile << jpostactTag.dump()  << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        
        outFile.close();
    } else {
        return 1;
    }
    return 0;
}

/**
 * @brief Reads the manifest data from a file and populates the data structure.
 * @param filepath The path to the manifest file.
 * @return Returns 0 if the file is successfully read and parsed, otherwise returns 1.
 */
int CPkgManifest::readFromFile(std::string filepath) {
    
    nlohmann::json jsonManifestFileData;
    try {
        //std::cout << "opening file" << std::endl;
        std::ifstream fileHandler;
        fileHandler.open(filepath);
        if (fileHandler.is_open()) {
            fileHandler >> jsonManifestFileData;
            fileHandler.close();
        } else {
            //todo: return error
            std::cout << "unable to open manifest file" << std::endl;
            return 1;
        }
        
        //fill in datastructure
        if(jsonManifestFileData.contains("meta")) {
            pPkgMetaData = new CManifestMetaData();
            if(pPkgMetaData) {
                pPkgMetaData->setValues(jsonManifestFileData["meta"]);
            }
        }
        
        //std::cout << "meta tag done" << std::endl;
        
        if(jsonManifestFileData.contains("applicability")) {
            pPkgApplicabilityData = new CManifestApplicabilityData();
            if(pPkgApplicabilityData) {
                pPkgApplicabilityData->setValues(jsonManifestFileData["applicability"]);
            }
        }
        
        if(jsonManifestFileData.contains("prop")) {
            pPkgPropData = new CManifestPropData();
            if(pPkgPropData) {
                pPkgPropData->setValues(jsonManifestFileData["prop"]);
            }
        }
        
        
        if(jsonManifestFileData.contains("refer")) {
            for (auto &it_referitem : jsonManifestFileData["refer"]) {
                //std::cout << it_referitem.dump() << '\n';
                CManifestReferData *pReferDataItem = new CManifestReferData();
                if(pReferDataItem) {
                    pReferDataItem->setValues(it_referitem);
                    vPkgReferData.push_back(pReferDataItem);
                }
            }
        }
        
        //std::cout << "refer tag done" << std::endl;
        
        if(jsonManifestFileData.contains("preact")) {
            for (auto &it_preactitem : jsonManifestFileData["preact"]) {
                if(!it_preactitem.contains("action")) { continue; } //if action key not present then ignore the item.
                CManifestActData *pPreActDataItem = new CManifestActData();
                if(pPreActDataItem) {
                    pPreActDataItem->setValues(it_preactitem);
                    vPkgPreActData.push_back(pPreActDataItem);
                    graph.insertNewNode(pPreActDataItem, convertManifestToNodeData(*pPreActDataItem));
                }
               
            }
        }
        
        //std::cout << "preact tag done" << std::endl;
        
        if(jsonManifestFileData.contains("act")) {
            for (auto &it_actitem : jsonManifestFileData["act"]) {
                if(!it_actitem.contains("action")) { continue; } //if action key not present then ignore the item.
                CManifestActData *pActDataItem = new CManifestActData();
                if(pActDataItem) {
                    pActDataItem->setValues(it_actitem);
                    vPkgActData.push_back(pActDataItem);
                    graph.insertNewNode(pActDataItem, convertManifestToNodeData(*pActDataItem));
                }
               
            }
        }
        
        //std::cout << "act tag done" << std::endl;
        
        if(jsonManifestFileData.contains("postact")) {
            for (auto &it_postactitem : jsonManifestFileData["postact"]) {
                if(!it_postactitem.contains("action")) { continue; } //if action key not present then ignore the item.
                CManifestActData *pPostActDataItem = new CManifestActData();
                if(pPostActDataItem) {
                    pPostActDataItem->setValues(it_postactitem);
                    vPkgPostActData.push_back(pPostActDataItem);
                    graph.insertNewNode(pPostActDataItem, convertManifestToNodeData(*pPostActDataItem));
                }
            }
        }
        
        /*custom tags parsing*/
        for (const auto& [tag, nodes] : jsonManifestFileData.items())
        {
            if (tag != "meta" && tag != "applicability" && tag != "prop" && tag != "refer" && tag != "preact" && tag != "act" && tag != "postact") {
                for (const auto& act : nodes) {
                    if(!act.contains("action")) { continue; }
                    CManifestActData *pFaiSucActDataItem = new CManifestActData();
                    if (pFaiSucActDataItem) {
                        pFaiSucActDataItem->setValues(act);
                        vPkgFaiSucActData[tag].push_back(pFaiSucActDataItem);
                        graph.insertNewNodeTags(pFaiSucActDataItem, tag, convertManifestToNodeData(*pFaiSucActDataItem));
                    }
                }
            }
        }
        
        /**flatten the path*/
        flattenActionPaths();
        
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

NodeData* CPkgManifest::convertManifestToNodeData(const CManifestActData& manifestData) {
    NodeData* nodeData = new NodeData();
    nodeData->action = manifestData.action;
    nodeData->param = manifestData.param;
    nodeData->onfailure_tag = manifestData.onFailure;
    nodeData->onsuccess_tag = manifestData.onSuccess;
    return nodeData;
}