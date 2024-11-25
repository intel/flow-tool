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

#include "apihandlers.h"
#include "sysinfo.h"


CAPIHandlers::CAPIHandlers() {
    init();
}

CAPIHandlers::~CAPIHandlers() {
    deinit();
}
   
/**
 * @brief Initializes the CAPIHandlers object.
 * 
 * @return 0 on success.
 */
int CAPIHandlers::init() {
    //logMsg("init");
 
    _bCancel = false; 
    m_continueCount = 0;
    
    map_string_fPtr["EXECUTE"] = &CAPIHandlers::executeScript;

    pCmdDict = new CCommandReference();
    pPkgAction = new CPkgActions();
          
    return 0;
}

/**
 * @brief Deinitializes the CAPIHandlers object.
 *
 * @return 0 on success.
 */
int CAPIHandlers::deinit() {
    logMsg("deinit");

    if (pCmdDict) {
        delete pCmdDict;
        pCmdDict = NULL;
    }
    
    if(pPkgAction) {
        delete pPkgAction;
        pPkgAction = NULL;
    }
  
    if (_pManifest){
        delete _pManifest;
        _pManifest = NULL; 
    }  
       
  return 0;
}

/**
 * @brief Trims leading and trailing whitespace characters from a string.
 * 
 * @param s The string to be trimmed.
 * @return The trimmed string.
 */
std::string CAPIHandlers::trimString(const std::string &s) {
   
    return s;
}

/**
 * Starts the deployment process.
 *
 * @param manifestPath The path to the manifest file.
 * @param continueCount The number of times the deployment process should continue.
 * @return Returns the result of the coreHandler function.
 */
int CAPIHandlers::startDeploy(std::string manifestPath, int continueCount) {
    //logMsg("startDeploy : " + manifestPath); 
    
    m_continueCount = continueCount;
    
    sArchiveDir = std::filesystem::path(manifestPath).parent_path();
    qIncludedManifests.push(manifestPath);//support multiple manifests....

    //logMsg("startDeploy : " + manifestPath);  
    _bCancel = false; 
    return coreHandler(manifestPath, false, sArchiveDir);
}

/**
 * @brief Handles the core functionality of the API handlers.
 * @param manifestPath The path to the manifest file.
 * @param bPrepare A boolean flag indicating whether to prepare for deployment or not.
 * @param archivePath The path to the archive file.
 * @return Returns 0 if the core handling is successful, 1 otherwise.
 */
int CAPIHandlers::coreHandler(std::string manifestPath, bool bPrepare, std::string archivePath) {
    logMsg("manifest file: " + manifestPath);
    //std::cout << "processing manifest :" << manifestPath <<  std::endl;
    
    bool bSuccess = true;
    
    try {
        
        if(!manifestPath.empty() && !std::filesystem::exists(manifestPath)) {
            //todo check for permissions, validate against schema
            //todo handle error. open: should we continue to other manifest or stop here.
            return 1;
        }
        
        sManifestParentPath = std::filesystem::path(manifestPath).parent_path();
        sPkgPath = std::filesystem::path(sManifestParentPath).parent_path();//todo: may be wrong path - please revisit.
        std::cout << "sManifestParentPath is: " << sManifestParentPath << std::endl;  
        std::cout << "sPkgPath is: " << sPkgPath << std::endl;  
        _pManifest = new CPkgManifest();
        
        if(_pManifest) {
            _pManifest->readFromFile(manifestPath);

            cout << endl;
            cout << "########################## FOLLOWING A PATH ###############################" << endl;
            cout << endl;
            cout << endl;

            SNode = _pManifest->graph.vNodes[0];
            while (SNode != _pManifest->graph.endNode) {

                int tempVariable = rValue(static_cast<CManifestActData*>(SNode->pDataRef), bPrepare);

                Node* getNextNode = _pManifest->graph.receive_next_node(SNode, tempVariable);

                SNode = getNextNode;

                cout << "Next Node Param: " << SNode->data->param << endl;
                cout << endl;
            }
            
            //std::cout << "updating status " << std::endl;
            
            //CStatusInfo::getInstance()->setStatus(DEPLOY_STATUS, "{ \"status\": \"deploy\",\"desc\":\"manifest file loaded\" ,\"progress\":\"10\"}");
            
            //todo: clean up
            /*if(pCmdDict && _pManifest->vPkgReferData.size() > 0) {
                pCmdDict->updateDict(_pManifest->vPkgReferData);
            }*/
            //todo: print active dict to confirm.

            //std::cout << "handle actions" << std::endl;
            //preact

            cout << endl;
            cout << "########################## FOLLOWING SEQUENTIAL ORDER ###############################" << endl;
            cout << endl;
            cout << endl;

            //std::cout << "handle actions" << std::endl;
            //preact
            std::vector<CManifestActData*>::iterator pre_it;
            int rebootActionCounter = 0;
            for(pre_it = _pManifest->vPkgPreActData.begin(); pre_it != _pManifest->vPkgPreActData.end(); pre_it++ )    {
                CManifestActData *pData = (CManifestActData*)*pre_it;
                if(pData) {
                    if(strcmp("REBOOT", pData->action.c_str()) == 0) {
                        continue; //reboot not allowed in pre and post act - ignore
                    }
                    //precheck for system information 
                    if(strcmp("PRE_CHECK", pData->action.c_str()) == 0) {
                        std::string pkgset = pData->param.c_str();
                       
                        if(!getApplicablityData(pkgset)){
                           return 0;
                        }
                    }

                    handleAction(pData, bPrepare);
                }
                if (_bCancel == true){
                            
                    break; 
                }
            }
            if (_bCancel != true){

                //act
                std::vector<CManifestActData*>::iterator act_it;
                for(act_it = _pManifest->vPkgActData.begin(); act_it != _pManifest->vPkgActData.end(); act_it++ )    {
                    CManifestActData *pData = (CManifestActData*)*act_it;
                    if(pData) {
                        //logic: if count == 0 execute till first reboot or end of manifest.
                        //if count == n skip actions until n reboots or end; execute till n+1 reboot or end. skip remaining actions if any
                        //example: if count == 1 skip actions until reboot or end; execute till next reboot if not end or until end.
                        
                        if (m_continueCount == 0 ) {
                            if(strcmp("REBOOT", pData->action.c_str()) == 0) {
                                 logMsg("## Please reboot to proceed ## " );
                                //break; //stop execution of whole manifest here. to be continued after reboot.//throw expection and catch
                                throw; //stop execution of whole manifest here. to be continued after reboot
                            }
                        } else if (m_continueCount > rebootActionCounter) {
                            if(strcmp("REBOOT", pData->action.c_str()) != 0) {
                                continue;//if action is not reboot - skip
                            } else {
                                rebootActionCounter++;
                                 if ((m_continueCount + 1) <= rebootActionCounter) {
                                    throw; //stop execution of whole manifest here. to be continued after reboot
                                } 
                            }
                        }
                    
                        handleAction(pData, bPrepare);
                    }
                    if (_bCancel == true){
                            
                        break; 
                    }                
                }
            }
            
            if (_bCancel != true){            
               
                //postact
                std::vector<CManifestActData*>::iterator post_it;
                for(post_it = _pManifest->vPkgPostActData.begin(); post_it != _pManifest->vPkgPostActData.end(); post_it++ )    {
                    CManifestActData *pData = (CManifestActData*)*post_it;
                    if(pData) {
                        if(strcmp("REBOOT", pData->action.c_str()) == 0) {
                            continue; //reboot not allowed in pre and post act - ignore
                        }
                        handleAction(pData, bPrepare);
                    }
                    if (_bCancel == true){
                       
                        break; 
                    }                   
                }
                
            }
         }
    } catch (const std::exception &e) {
        std::cout << e.what() << "error executing actions" << std::endl;
        bSuccess = false;
    } catch (...) {
        std::cout << "Reboot exception - program " << std::endl;       
    }
    
    //cleanup
    if (_pManifest) {
        delete _pManifest;
        _pManifest = NULL;
        _bCancel = false; 
    }
    
    if(bSuccess == false) {
        return 1;
    }
    //remove process manifest file.
    if(!qIncludedManifests.empty()){
        std::cout << "remove processed manifest file" << std::endl;
        qIncludedManifests.pop();

        if(!qIncludedManifests.empty()){
            std::cout << "recursive function call to handle included manifests....." << std::endl;        
            coreHandler(qIncludedManifests.front(), bPrepare, archivePath);
        }
    }
    
    return 0;
}

/**
 * Handles the action specified in the given `CManifestActData` object.
 * 
 * @param pActItem A pointer to the `CManifestActData` object containing the action data.
 * @param bPrepare A boolean indicating whether the action is being prepared or executed.
 * @return Returns 0 if the action is successfully handled, otherwise returns 1.
 */
int CAPIHandlers::handleAction(CManifestActData *pActItem, bool bPrepare) {
    
    if (!pActItem) {
       logMsg("error: invalid data");
       return 1;
    }
   
    try {
       
        std::vector<std::string> vCommands = pCmdDict->getCommandsForVerb(pActItem->action);
      
        bool bCustomTask = false;
        bool bRunCmd = false;
        
        switch(pPkgAction->toEnum(pActItem->action)) {

            case CPkgActions::eActionVerbs::eAUTO_REMOVE:
            case CPkgActions::eActionVerbs::eUPDATE:
            case CPkgActions::eActionVerbs::eUPGRADE:
            case CPkgActions::eActionVerbs::eREBOOT:
            case CPkgActions::eActionVerbs::eMODULE_LIST:
            case CPkgActions::eActionVerbs::eREBOOT2:
            {
                /** for all this actions simply run the command */
                if(bPrepare) {
                    /** These are standard commands with no params - no need to prepare*/
                    //std::cout << "prepare needs no action" << std::endl;
                } else {
                    bRunCmd = true;
                }
            }
            break;

            case CPkgActions::eActionVerbs::eCUSTOM_TASK:
                bCustomTask = true; 
                break; 
            case CPkgActions::eActionVerbs::ePRE_CHECK:
                break; 
             //precheck handled seperately                
            default:
                logMsg("error: action not supported");
                return 1;
        }

        if (bCustomTask){
            //todo: what do we do if failed?
            logMsg("action : " + pActItem->action + " ; custom command to run: " );            

            handleCustomAction(pActItem, bPrepare); 
        } else {
            std::stringstream ss_command;
            for (size_t i = 0; i < vCommands.size(); i++) {
                ss_command << vCommands[i] << " ";
            }

            if(bRunCmd == true && !ss_command.str().empty()) {
                //std::cout << ss_command.str() << std::endl;
                //todo: add result checking                    
                //std::cout << "command to run: " << ss_command.str() << std::endl;
                logMsg("action : " + pActItem->action + " ; command to run: " + ss_command.str());
                std::string strRes = Helper::runCmd(ss_command.str(), 0);
                if (!pActItem->expected.empty()){ //non empty means we need to check the result            
                        
                    //std::cout << "command expected: " << pActItem->expected<< std::endl;                     
                    trimString(strRes); 
                    bool bSuccess = true;                         
                    if (strRes == pActItem->expected) { //if onSuccess
                        if (!pActItem->onSuccess.empty()) {
                            checkandStartAction(pActItem->onSuccess);
                        }
                    } else if (!pActItem->condition.empty()) { //if there is a condition for result
                        bSuccess = evalCondition(pActItem->expected, strRes, pActItem->condition);
                    } else {
                        bSuccess = false;
                    }
                         //if onFailure
                    if (!bSuccess && !pActItem->onFailure.empty()) {
                        checkandStartAction(pActItem->onFailure);
                    }                        
                    
                }
            } else {
                //std::cout << "skipped command to run: " << ss_command.str() << std::endl;
                logMsg("action : " + pActItem->action + " ; skipped command to run: " + ss_command.str());
            }
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << "error executing action command"<< std::endl;        
        return 1;
    }
    
    //std::cout << " done" << std::endl;

    return 0;
}


/**
 * @brief Checks if the given action is valid and starts the corresponding action.
 * 
 * @param strAction The action to be checked and started.
 * @return True if the action is found and started, false otherwise.
 */
bool CAPIHandlers::checkandStartAction(std::string strAction)
{
    bool bFound = false; 
    
    if (strAction == "PACKAGE_STOP" || strAction == "MANIFEST_EXIT" ){
        //this action means we will exit the current manifest actions
        bFound = true; 
        cancelPackage(); 
    }
    
    //check act data first
    std::vector<CManifestActData*>::iterator act_it;
    for(act_it = _pManifest->vPkgActData.begin(); act_it != _pManifest->vPkgActData.end(); act_it++ )    {
        CManifestActData *pData = (CManifestActData*)*act_it;
        if(pData->action == strAction) {
            bFound = true; 
            handleAction(pData, false);
            break;
        }
    }

    
    return bFound;
}

// Return 0 if the expected value is the same as return value; otherwise 1
int CAPIHandlers::rValue(CManifestActData *pActItem, bool bPrepare) {

    if (pActItem->action.empty()) { 	
		std::cout << "ERROR: No action specified" <<  std::endl;
		return -1; 		
	} else {
		std::map<std::string, fPtr>::iterator it = map_string_fPtr.find(pActItem->action);
		if (it != map_string_fPtr.end()) {
			fPtr func_addr = it->second;
			int returnVal = (this->*func_addr)(pActItem, bPrepare);

			if (pActItem->expected != ""){
			    if (pActItem->expected == std::to_string(returnVal)) {
                    return 0;
                } else {
                    return 1;
                }
            }
        } else {
		    std::cout << "ERROR: Action not supported " << std::endl; 
		    return -1; 
		}
        return 0;
    }
}

/**
 * Handles a custom action specified in the manifest.
 *
 * @param pActItem A pointer to the CManifestActData object representing the custom action.
 * @param bPrepare A boolean indicating whether the action is being prepared or executed.
 * @return Returns 0 if the custom action is successfully handled, -1 otherwise.
 */
int CAPIHandlers::handleCustomAction(CManifestActData *pActItem, bool bPrepare){

	if (pActItem->action.empty()){	
		//log error
		std::cout << "No action specified for custom action" <<  std::endl;
		return -1; 	
		
	} else {
    	        std::cout << "handle custom action: " <<  pActItem->action <<  std::endl;
		std::map<std::string, fPtr>::iterator it = map_string_fPtr.find(pActItem->action);
		if (it != map_string_fPtr.end()) {
			fPtr func_addr = it->second;
			int returnVal = (this->*func_addr)(pActItem, bPrepare);
			if (pActItem->expected != ""){
			    if (pActItem->expected == std::to_string(returnVal)){
			        //check on_success
			        std::cout << "custom action pass: " <<  std::to_string(returnVal) << std::endl;
			        if ((pActItem->onSuccess).empty())
                        return 0; 
                    else{
                        //call handleAction again
                        bool bFound = checkandStartAction(pActItem->onSuccess);
                        
                        if (!bFound){
                            std::cout << "No found action for success case" << std::endl; 
                            return -1; 
                        }
                        return 0;                 
                    }  
			    } else {
			        //check on_failure
			        std::cout << "custom action fail: " <<  std::to_string(returnVal) << std::endl;			    
			        if ((pActItem->onFailure).empty()){
        			    std::cout << "no onFailure action specified" <<  std::endl;				    
                        return -1; 
                    }
                    else{
                        //call handleAction again
        			    std::cout << "onFailure action specified, call: " <<  pActItem->onFailure << std::endl;		                    
                        
                        bool bFound = checkandStartAction(pActItem->onFailure);                   
                        
                        if (!bFound){
                            std::cout << "No found action for failure case" << std::endl; 
                            return -1; 
                        }
                        return 0; 
                    }
			    }
			}
		} else {
		    //log error
		    std::cout << "custom action not supported " << std::endl; 
		    return -1; 
		}
		return 0; 
	} 

}

/**
 * Executes a script specified by the given `pActItem` parameter.
 * 
 * @param pActItem A pointer to the `CManifestActData` object representing the script to be executed.
 * @param bPrepare A boolean value indicating whether the script is being executed in preparation mode.
 * @return An integer value indicating the success or failure of the script execution.
 *         A return value of `0` indicates successful execution, while a non-zero value indicates failure.
 */
int CAPIHandlers::executeScript(CManifestActData *pActItem, bool bPrepare) {

    int retVal = 0;

    try {
        std::string sScriptPath = std::filesystem::path(sManifestParentPath) / pActItem->param;
       // std::string sScriptPath = std::filesystem::path(sPkgPath) / pActItem->param;
        std::cout << "sScriptPath: " << sScriptPath  <<std::endl; 

        std::string terminal_command = "chmod +x " + sScriptPath;
        std::cout << "executeScript command: " << terminal_command  <<std::endl;
        if(bPrepare != true) {
            Helper::runCmd(terminal_command, 0);
        }
        
        retVal = system(sScriptPath.c_str());
        // std::cout << WEXITSTATUS(retVal) << std::endl; 
        
    } catch (...) {
        
        std::cout << "Exception" << std::endl; 
    }
    
    return WEXITSTATUS(retVal);
}

/**
 * Checks the kernel version and determines the platform name.
 * 
 * @param pActItem A pointer to the CManifestActData object.
 * @param bPrepare A boolean indicating whether the function is being called for preparation.
 * @return An integer representing the result code:
 */
int CAPIHandlers::kernelCheck(CManifestActData *pActItem, bool bPrepare){

    int resCode = -1; 
    std::string sVersion("Unsupported"); 
    std::string sCommand = "grep -oP \"model name\\K.*\" /proc/cpuinfo | head -1";  
    //std::cout << "run kernelCheck command " << sCommand << std::endl;        
    std::string sResult = Helper::runCmd(sCommand, 0);
    
    if (sResult.empty())
        return -1; 

          
    //parse the string e.g. : Intel(R) Core(TM) i7-8559U CPU @ 2.70GHz
    std::istringstream f(sResult);
    std::string s;    
    while (getline(f, s, ' ')) {
        //std::cout << s << std::endl;

        if (s.find("Ultra") != std::string::npos){
            //this is RPL-U
            resCode = 0;            
            sVersion = "MTL";
            resCode = 0;
            break; 
        }

        size_t index = s.find('-'); 
        if (s.find('i') == 0 && index != std::string::npos){
            index++;
            if (index +1 < s.size()) { 
                if (s.substr(index, 1) == "1" && (index+1 < s.size())){ 
                    index++;
                    int number = std::stoi(s.substr(index, 1));            
                    switch (number){
                        case 1:
                        sVersion = "TGL";
                        break;
                        
                        case 2:
                        sVersion = "ADL";                 
                        break; 
                        
                        case 3:                        
                        case 4:
                        sVersion = "RPL or MTL";
                        resCode = 0;                        
                        break;      
                                   
                        default: 
                        sVersion = "NotSupported";  
                        break; 
                    }
                    
                }
            }
        } 
    }
   // std::cout << "kernel info: " << sVersion << std::endl;
    pltName = sVersion;
    return resCode; 
}


//for future use
/**
 * Evaluates a condition between two strings.
 * @param sSource The source string to compare.
 * @param sTarget The target string to compare.
 * @param sCondition The condition to apply for the comparison. Valid conditions are "<" (less than) and ">" (greater than).
 * 
 * @return `true` if the condition is satisfied, `false` otherwise.
 */
bool CAPIHandlers::evalCondition(std::string sSource, std::string sTarget, std::string sCondition){
    //todo: simple comparison for now, move to regex eval later         
    //sSource: pActItem->expected, sTarget: result
    std::cout << "compare the result condition " << sCondition << std::endl;    
    bool bSuccess = true;                 
    size_t n = sTarget.size(); 
    size_t n1 = sSource.size();  
    size_t size = 0;
    if (n <= n1)
        size = n;
    else 
        size = n1; 

    if (sCondition == "<"){
        for (size_t i =0; i< size; i++){
            if (sSource[i] != sTarget[i]){
                if (std::atoi((const char*)&sTarget[i]) > std::stoi((const char*)&sSource[i])) {
                    bSuccess = false; 
                    break; 
                }
            }
        }
                                   
    } else if (sCondition == ">"){

        for (size_t i =0; i< size; i++){
            if (sSource[i] != sTarget[i]){
                if (std::atoi((const char*)&sTarget[i]) < std::stoi((const char*)&sSource[i])) {
                    bSuccess = false; 
                    break; 
                }
            }
        }
                                                   
    } 
    
    return bSuccess;
}


//for future use
void CAPIHandlers::cancelPackage(){
    _bCancel = true; 
}

/**
 * Retrieves the applicability data for a given package name.
 *
 * @param pkgname The name of the package.
 * @return True if the applicability data was successfully retrieved, false otherwise.
 */
bool CAPIHandlers::getApplicablityData(std::string pkgname)
{
    sysinfoData sysData;
    
    sysData.distro =  _pManifest->pPkgApplicabilityData->os;
    sysData.kversion =  _pManifest->pPkgApplicabilityData->kernel;
    sysData.distroVersion = _pManifest->pPkgApplicabilityData->vDistros;
    sysData.platform = _pManifest->pPkgApplicabilityData->vPlatforms;
    sysData.pkgsets = pkgname;
    //sysData.hardware = _pManifest->pPkgApplicabilityData->vHW;
    bool retVal = CSysInfo::getSysInfo(sysData);
    return retVal;
}
