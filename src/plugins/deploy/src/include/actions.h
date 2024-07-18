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
#include <map>

/** class dedicated for Commands and Actions definitions */
class CPkgActions {
    
    /**
     * steps to add action.     
     * 1. define command for action in dictionary
     * 2. add action in enum
     * 3. map string in dict to action in enum
     */
    
public:
    /** Generic linux commands */
    const std::string sDictionary_linux_json = "{\
        \"MODULE_INSTALL\":[\"sudo\",\"insmod\"],\
        \"MODULE_REMOVE\":[\"sudo\",\"rmmod\"],\
        \"MODULE_LIST\":[\"sudo\",\"lsmod\"],\
        \"MODULE_LIST_BY_PKG\":[\"sudo\",\"lsmod | grep \"],\
        \"UNZIP\":[\"unzip\"],\
        \"UNTAR\":[\"tar\",\"xvf\"],\
        \"GIT_CLONE\":[\"git\", \"clone\"],\
        \"GIT_SYNC\" :[\"git\", \"sync\"],\
        \"SCRIPT\":[\"sudo\",\"-E\"],\
        \"SOURCE\":[\"source\"],\
        \"REBOOT\" :[\"sudo\", \"reboot_xyz\"],\
        \"REBOOT2\" :[\"sudo\", \"shutdown_xyz\", \"-r\", \"now\"],\
        \"DOWNLOAD\":[\"sudo\",\"wget\", \"-P \"],\
        \"KPATCH_APPLY\":[\"sudo\",\"kpatch\", \"load\"],\
        \"FILE_COPY\":[\"sudo\", \"cp\"],\
        \"FILE_MOVE\":[\"sudo\", \"mv\"],\
        \"FILE_REMOVE\":[\"sudo\",\"rm \"],\
        \"LINK_REMOVE\":[\"sudo\",\"unlink \"],\
        \"FOLDER_ADD\":[\"sudo\",\"mkdir \", \"-p \"],\
        \"FOLDER_COPY\":[\"sudo\", \"cp\"],\
        \"FOLDER_MOVE\":[\"sudo\", \"cp\"],\
        \"FOLDER_REMOVE\":[\"sudo\",\"rmdir \", \"-rf \"],\
        \"STOP_SERVICE\":[\"sudo\",\"systemctl\", \"stop \"],\
        \"START_SERVICE\":[\"sudo\",\"systemctl\", \"start \"],\
        \"RESTART_SERVICE\":[\"sudo\",\"systemctl\", \"restart \"],\
        \"UNMASK_SERVICE\":[\"sudo\",\"systemctl\", \"unmask\"],\
        \"RELOAD_DAEMON\":[\"sudo\", \"systemctl\", \"daemon-reload\"],\
        \"STATUS_SERVICE\":[\"sudo\",\"systemctl\", \"status \"],\
        \"KERNEL_VERSION\":[\"uname\", \"-r |grep -Eo\", \"'^.{1,3}'\"]\
    }";
    
    /** ubuntu specific - deb specific.*/
    const std::string sDictionary_ubuntu_deb_json = "{\
        \"INSTALL\":[\"sudo\", \"apt-get\", \"install\", \"-y\"],\
        \"REMOVE\" :[\"sudo\", \"apt-get\", \"remove\", \"--purge -y\"],\
        \"UPGRADE\":[\"sudo\",\"apt-get\",\"upgrade\"],\
        \"DIST_UPGRADE\":[\"sudo\", \"apt-get\", \"dist-upgrade\"],\
        \"UPDATE\":[\"sudo\",\"apt\",\"update\"],\
        \"AUTO_REMOVE\":[\"sudo\",\"apt\",\"autoremove\"],\
        \"LOCAL_INSTALL\":[\"sudo\",\"dpkg\", \"-i\"],\
        \"CHECK_DEPENDENCIES\":[\"sudo\",\"apt-cache\", \"depends\"],\
        \"STOP_SERVICE\":[\"sudo\",\"systemctl stop\"],\
        \"APT_CACHE_SEARCH\":[\"sudo\",\"apt-cache search \"],\
        \"APT_SEARCH\":[\"sudo\",\"apt list --installed | grep \"],\
        \"ADD_KEY\":[\"sudo\",\"apt-key add\"],\
        \"ADD_TO_SOURCES\":[\"sudo\",\"echo\", \"PARAM_1\",\" | sudo tee /etc/apt/sources.list.d/\"]\
    }";
        
    
    /*! supported action verbs */
    enum eActionVerbs {
        eUNDEFINED,
        
        /*! Dict - generic linux commands common for all distros*/
        eMODULE_INSTALL,
        eMODULE_REMOVE,
        eMODULE_LIST,  
        eMODULE_LIST_BY_PKG,
        
        eFILE_COPY,
        eFILE_REMOVE,
        eLINK_REMOVE,
        eFOLDER_ADD,
        eFOLDER_COPY,
        eFOLDER_MOVE,
        eFOLDER_REMOVE,
        
        eSOURCE,
        eEXECUTE,
        eSCRIPT,
        
        eUNTAR,
        eUNZIP,
        
        eREBOOT,
        eREBOOT2,
        
        eDOWNLOAD,
        eKPATCH_APPLY,
        
        eSTART_SERVICE,
        eSTOP_SERVICE,
        eRESTART_SERVICE,
        eUNMASK_SERVICE,
        eSTATUS_SERVICE,
        eRELOAD_DAEMON,
        
        /*! dict - deb specific */
        eINSTALL,
        eREMOVE,
        eUPGRADE,
        eDIST_UPGRADE,
        eUPDATE,
        eAUTO_REMOVE,
        eLOCAL_INSTALL,
        eCHECK_DEPENDENCIES,
        eAPT_CACHE_SEARCH,
        eAPT_SEARCH,
        eADD_KEY,
        eADD_TO_SOURCES,
        

        /*! dict - Tool custom */
        eREMOTE_INSTALL,
        eINCLUDE_MANIFEST,
        
        eKERNEL_VERSION,
        eCUSTOM_TASK ,
        ePRE_CHECK
    };
    
    // Map to associate the strings with the enum values
    std::map<std::string, eActionVerbs> s_mapStringVerbs;
    
    eActionVerbs toEnum(std::string sAction) {
        //std::cout << "toEnum : " << sAction << std::endl;
        if ( s_mapStringVerbs.find(sAction) != s_mapStringVerbs.end() ) {
            return s_mapStringVerbs[sAction];//found
        } else {
            //for debug: print map
            for(const auto& item : s_mapStringVerbs) {
               std::cout << item.first << " " << item.second << "\n";
            }
        }
        return eUNDEFINED;
    }

    CPkgActions() {

        /*! dict - linux*/
        s_mapStringVerbs["MODULE_INSTALL"] = eMODULE_INSTALL;
        s_mapStringVerbs["MODULE_REMOVE"] = eMODULE_REMOVE;
        s_mapStringVerbs["MODULE_LIST"] = eMODULE_LIST;   
        s_mapStringVerbs["MODULE_LIST_BY_PKG"] = eMODULE_LIST_BY_PKG;
        
        s_mapStringVerbs["FILE_COPY"] = eFILE_COPY;
        s_mapStringVerbs["FILE_REMOVE"] = eFILE_REMOVE;
        s_mapStringVerbs["LINK_REMOVE"] = eLINK_REMOVE;        
        s_mapStringVerbs["FOLDER_ADD"] = eFOLDER_ADD;
        s_mapStringVerbs["FOLDER_REMOVE"] = eFOLDER_REMOVE;
        
        s_mapStringVerbs["SCRIPT"] = eSCRIPT;
        s_mapStringVerbs["SOURCE"] = eSOURCE;
        s_mapStringVerbs["EXECUTE"] = eCUSTOM_TASK;
        
        s_mapStringVerbs["UNTAR"] = eUNTAR;
        s_mapStringVerbs["UNZIP"] = eUNZIP;
        
        s_mapStringVerbs["REBOOT"] = eREBOOT;
        s_mapStringVerbs["REBOOT2"] = eREBOOT2;
        
        s_mapStringVerbs["DOWNLOAD"] = eDOWNLOAD;
        s_mapStringVerbs["KPATCH_APPLY"] = eKPATCH_APPLY;    

        s_mapStringVerbs["START_SERVICE"] = eSTART_SERVICE;
        s_mapStringVerbs["STOP_SERVICE"] = eSTOP_SERVICE;
        s_mapStringVerbs["RESTART_SERVICE"] = eRESTART_SERVICE;
        s_mapStringVerbs["UNMASK_SERVICE"] = eUNMASK_SERVICE;
        s_mapStringVerbs["STATUS_SERVICE"] = eSTATUS_SERVICE;
        s_mapStringVerbs["RELOAD_DAEMON"] = eRELOAD_DAEMON;
        
        /*! dict - deb */
        s_mapStringVerbs["INSTALL"] = eINSTALL;
        s_mapStringVerbs["REMOVE"] = eREMOVE;
        s_mapStringVerbs["UPDATE"] = eUPDATE;
        s_mapStringVerbs["UPGRADE"] = eUPGRADE;
        s_mapStringVerbs["DIST_UPGRADE"] = eDIST_UPGRADE;
        s_mapStringVerbs["AUTO_REMOVE"] = eAUTO_REMOVE;
        s_mapStringVerbs["LOCAL_INSTALL"] = eLOCAL_INSTALL;
        s_mapStringVerbs["CHECK_DEPENDENCIES"] = eCHECK_DEPENDENCIES;
        s_mapStringVerbs["APT_CACHE_SEARCH"] = eAPT_CACHE_SEARCH;
        s_mapStringVerbs["APT_SEARCH"] = eAPT_SEARCH;
        s_mapStringVerbs["ADD_KEY"] = eADD_KEY;
        s_mapStringVerbs["ADD_TO_SOURCES"] = eADD_TO_SOURCES;
        
       
        /*! dict - tool custom */
        s_mapStringVerbs["REMOTE_INSTALL"] = eREMOTE_INSTALL;        
        s_mapStringVerbs["INCLUDE_MANIFEST"] = eINCLUDE_MANIFEST;  
        
        s_mapStringVerbs["KERNEL_VERSION"] = eKERNEL_VERSION;

        s_mapStringVerbs["KERNEL_CHECK"] = eCUSTOM_TASK;
        s_mapStringVerbs["EDIT_ETC_MODULE"] = eCUSTOM_TASK;        
        s_mapStringVerbs["MODULE_COPY"] = eCUSTOM_TASK;
        //s_mapStringVerbs["EXECUTE"] = eCUSTOM_TASK;
        s_mapStringVerbs["PRE_CHECK"] = ePRE_CHECK;
        
    }
};
