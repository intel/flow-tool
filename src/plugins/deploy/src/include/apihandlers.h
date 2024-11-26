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

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>

#include "manifestDataStructure.h"
#include "commandreference.h"
#include "actions.h"

/*! Class to handle APIs */
class CAPIHandlers {
    
private:

    /** manifest datastructure for parsing */
    CPkgManifest* _pManifest = NULL;
    
    /*! target directory to archive */
    std::string sArchiveDir;
    
    /** initialize */
    int init();
    
    /** deinitialize */
    int deinit();
    
    /** trim string */
    std::string trimString(const std::string &s);
    
    /** Log message- inline function*/
    inline void logMsg(std::string msg){
        //std::cout << __FILE__ << "(" << __LINE__ << "): " << msg << std::endl;
        
        //todo fix inline fuction.
        std::cout << msg << std::endl;
    }
    
    /** commands to refer */
    CCommandReference *pCmdDict = NULL;
    CPkgActions *pPkgAction = NULL;
    
           
    /**Handles mulitple manifest paths*/
    std::queue<std::string> qIncludedManifests;
    std::string sManifestParentPath;
    std::string sPkgPath;

    // First Node of the graph (Root Node)
    Node* SNode;
    std::string succFailVar;
        
    int handleAction(CManifestActData *pActItem, bool bPrepare);
    int coreHandler(std::string manifestPath, bool bPrepare, std::string archivePath);
    
    //! private variable 
    /*! function pointer */            
    typedef int(CAPIHandlers::*fPtr)(CManifestActData *pActItem, bool bPrepare);    
    /*! map of functional pointers and custom action string */            
    std::map<std::string, fPtr> map_string_fPtr;

    // Return 0 if the expected value is the same as return value; otherwise 1s
    int rValue(CManifestActData *pActItem, bool bPrepare);     
    
    //handle custom action which is normally not a standard action defined by commands
    int handleCustomAction(CManifestActData *pActItem, bool bPreapre);
    
    bool checkandStartAction(std::string strAction);        
    
    //custom function for kernel version

    int kernelCheck(CManifestActData *pActItem, bool bPrepare);    
    int executeScript(CManifestActData *pActItem, bool bPrepare);
        
    bool evalCondition(std::string sSource, std::string sTarget, std::string sCondition);
    bool _bCancel = false; 
    void cancelPackage();
    bool getApplicablityData(std::string pkgname);
    
    int m_continueCount;
    std::string pltName;
  
public:

    CAPIHandlers();
    ~CAPIHandlers();
    
 
    /** deploy components from package manifest
    *   param: manifest path
    *   param: continueCount - play actions after reboot.
    *   returns success (0)/Errors(invalid path, corrupted file, permission issues, incomplete manifest)
    */
    int startDeploy(std::string manifestPath, int continueCount);
};

