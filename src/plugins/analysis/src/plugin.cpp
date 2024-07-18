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
#include "analysis.h"
#include "logger.h"

/**
 * @brief Creates an instance of the plugin.
 * @return A pointer to the created instance of the CPlugin class.
 */
extern "C" CPluginInterface* create() {
    return new CPlugin;
}

/**
 * @brief Destroys the plugin object.
 * @param pPlugin A pointer to the plugin object to be destroyed.
 */
extern "C" void destroy(CPluginInterface* pPlugin) {
    delete pPlugin;
}

/**
 * \brief Default constructor for the CPlugin class.
 * \param[in] param A pointer to the parameter object.
 */
CPlugin::CPlugin() {
    init(NULL);
}

/**
 * @brief Destructor for the CPlugin class.
 * @param[in] None
 * @return None
 */
CPlugin::~CPlugin() {
    deinit(NULL);
}

/**
 * Retrieves information from the analysis plugin.
 *
 * @param pCxt A pointer to the program context.
 * @return A string containing the retrieved information.
 */
std::string CPlugin::getInfo(void* pCxt) {
    if( ((CProgramContext*)pCxt)->log_level == 7 ) {
        CLogger::getInstance().setLogLevel(PROGRAM_DEBUG_LEVEL);
    }
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, " analysis service getInfo");
    return pInstance->handleInfo("");
}

/**
 * \brief Entry point for the analysis plugin.
 * \param pCxt A void pointer representing the context.
 * \param sReq A string representing the request.
 * \return A string representing the response.
 */
std::string CPlugin::entry(void* pCxt, std::string sReq) {
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, " analysis service entry");
    return pInstance->handleEntry(sReq);
}

/**
 * Cancels the analysis plugin.
 * @param pCxt A pointer to the context.
 * @param sReq The request string.
 */
void CPlugin::cancel(void* pCxt, std::string sReq) {
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, " analysis service cancel");
    pInstance->handleCancel(sReq);
    return;
}

/**
 * @brief Initializes the CPlugin object.
 * @param pCxt A pointer to the context.
 * @return 0 on success.
 */
int CPlugin::init(void* pCxt) {
    
    CLogger::getInstance().setLogLevel(PROGRAM_ERROR_LEVEL);
        
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, " analysis service bringup");
    pInstance = new CAnalysis();

    return 0;
}
  
/**
 * @brief Deinitializes the CPlugin instance.
 * @param pCxt A pointer to the context.
 */
void CPlugin::deinit(void* pCxt) {
    CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, " analysis service teardown");
    if(pInstance) {
        delete pInstance;
        pInstance = NULL;
    }
    return;
}

