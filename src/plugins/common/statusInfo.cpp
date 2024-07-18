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

#include "statusInfo.h"


CStatusInfo* CStatusInfo::instance = NULL;


/**
 * Retrieves the status associated with the given API.
 *
 * @param strAPI The API for which the status is to be retrieved.
 * @return The status associated with the given API. If no status is found, an empty string is returned.
 */
std::string CStatusInfo::getStatus(std::string strAPI) {
    
    if ((!strAPI.empty()) && (!map_status.empty())) {
        std::map<std::string, std::string>::iterator it = map_status.find(strAPI);
        if (it != map_status.end()) {
            return std::string(it->second); 
        }
    }
    return "";
}

/**
 * Sets the status for a given API.
 * 
 * @param strAPI The API for which the status is being set.
 * @param strStatus The status to be set.
 * @return 0 if the status is set successfully, 1 if either strAPI or strStatus is empty.
 */
int CStatusInfo::setStatus(std::string strAPI, std::string strStatus) {

    if (strAPI.empty() || strStatus.empty()) {
        return 1;
    }
    
    if (map_status.empty()) {
        map_status.insert(std::make_pair(strAPI, strStatus));
    } else {
        std::map<std::string, std::string>::iterator it = map_status.find(strAPI);
        if (it == map_status.end()) {
            map_status.insert(std::make_pair(strAPI, strStatus));
        } else {
	        map_status.insert_or_assign(strAPI, strStatus);
        }
    }
    
    return 0;
}

/**
 * Retrieves the status details for a given status.
 *
 * @param strStatus The status for which to retrieve the details.
 * @return The status details as a string. If the status is not found or if the status is empty, an empty string is returned.
 */
std::string CStatusInfo::getStatusDetails(std::string strStatus) {

    if ((!strStatus.empty()) && (!map_statusDetails.empty())) {
        std::map<std::string, std::string>::iterator it = map_statusDetails.find(strStatus);
        if (it != map_statusDetails.end()) {
            return std::string(it->second); 
        }
    }
    
    return "";
}

/**
 * @brief Sets the status details for a given status.
 * 
 * This function sets the status details for a given status. If either the status or the details
 * is empty, the function returns 1. If the status already exists in the map, the details are updated.
 * Otherwise, a new entry is added to the map with the given status and details.
 * 
 * @param strStatus The status for which the details are to be set.
 * @param strDetails The details to be set for the given status.
 * @return 0 if the status details are set successfully, 1 if either the status or the details is empty.
 */
int CStatusInfo::setStatusDetails(std::string strStatus, std::string strDetails){

    if (strStatus.empty() || strDetails.empty()){
        return 1; 
    }
    
    if (map_statusDetails.empty()){
        map_statusDetails.insert(std::make_pair(strStatus, strDetails));
    }else {
        std::map<std::string, std::string>::iterator it;
        it = map_statusDetails.find(strStatus);
        
        if (it == map_statusDetails.end()) {
            map_statusDetails.insert(std::make_pair(strStatus, strDetails));
        }
        else {
	        map_statusDetails.insert_or_assign(strStatus, strDetails);
        }        
    }
    
    return 0;
}      

/**
 * @brief Clears all status information.
 *
 * This function clears the status and status details of the CStatusInfo object.
 * It calls the `clearStatus` and `clearStatusDetails` functions internally.
 */
void CStatusInfo::clearAllStatus() {
    clearStatus();
    clearStatusDetails();
}

/**
 * @brief Clears the status information stored in the CStatusInfo object.
 *
 * This function clears the map_status container, removing all status information
 * stored in the CStatusInfo object.
 */
void CStatusInfo::clearStatus() {
    map_status.clear();
}

/**
 * @brief Clears the status details map.
 *
 * This function clears the map of status details in the CStatusInfo class.
 * After calling this function, the map will be empty.
 */
void CStatusInfo::clearStatusDetails() {
    map_statusDetails.clear(); 
}
