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

/**
 * Class to register plugin
 */
 
#include "plugin.h"
#include "deploy.h"
#include "logger.h"

//extern std::string getInfoC(void* pCtx);

extern "C" CPluginInterface* create() {
    return new CPlugin;
}

extern "C" void destroy(CPluginInterface* pPlugin) {
    delete pPlugin;
}

CPlugin::CPlugin() {
    init(NULL);
}

CPlugin::~CPlugin() {
    deinit(NULL);
}

/**
 * Retrieves information about the deploy service.
 *
 * @param pCxt A pointer to the program context.
 * @return A string containing the information.
 */
std::string CPlugin::getInfo(void* pCxt) {
    if( ((CProgramContext*)pCxt)->log_level == 7 ) {
        CLogger::getInstance().setLogLevel(PROGRAM_DEBUG_LEVEL);
    }
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "deploy service getInfo");
    return pInstance->handleInfo("");
}

/**
 * \brief Entry point for the deploy service.
 * \param pCxt A pointer to the context.
 * \param sReq The request string.
 * \return The response string.
 */
std::string CPlugin::entry(void* pCxt, std::string sReq) {
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "deploy service entry");
    return pInstance->handleEntry(sReq);
}

void CPlugin::cancel(void* pCxt, std::string sReq) {
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "deploy service cancel");
    pInstance->handleCancel(sReq);
    return;
}

/**
 * @brief Initializes the CPlugin object.
 * @param pCxt A pointer to the context object.
 * @return 0 on success, non-zero on failure.
 */
int CPlugin::init(void* pCxt) {
    
    CLogger::getInstance().setLogLevel(PROGRAM_ERROR_LEVEL);
        
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "deploy service bringup");
    pInstance = new CDeploy();

    return 0;
}
  
/**
 * @brief Deinitializes the CPlugin instance.
 * 
 * @param pCxt A pointer to the context.
 */
void CPlugin::deinit(void* pCxt) {
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "deploy service teardown");
    if(pInstance) {
        delete pInstance;
        pInstance = NULL;
    }
    return;
}

