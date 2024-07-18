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
#include <filesystem>
#include <signal.h>
#include <condition_variable>
#include <mutex>

#include "pluginManager.h"
#include "logger.h"

std::condition_variable conditionVar;
sig_atomic_t volatile gRunning = true;

/** Application Terminate signal handler */
void signalHandler( int signum ) {
  PROGRAM_NOTICE("Interrupt signal (", signum, ") received.");
  if (signum == SIGSEGV) {
    gRunning = false;
      
    // un-register SIGSEGV handler
    signal(SIGSEGV, SIG_DFL);
    PROGRAM_ALERT("Segmentation fault has occured");
    std::cout.flush();
  }
  
  if (signum == SIGINT) {
    gRunning = false;
  }
  
  /**
   * do app specific stuff
   */

  conditionVar.notify_one();
}

/** Application entry */
int main(int argc, char** argv) {
        
    if (__cplusplus == 202101L) std::cout << "C++23";
    else if (__cplusplus == 202002L) std::cout << "C++20";
    else if (__cplusplus == 201703L) std::cout << "C++17";
    else if (__cplusplus == 201402L) std::cout << "C++14";
    else if (__cplusplus == 201103L) std::cout << "C++11";
    else if (__cplusplus == 199711L) std::cout << "C++98";
    else std::cout << "pre-standard C++." << __cplusplus;
    std::cout << "\n";


    //std::cout << "main " << argc << " arguments:" << "\n";
    CLogger::getInstance().setLogLevel(PROGRAM_ERROR_LEVEL);
    
    CProgramContext *pCtx = new CProgramContext();
    if(!pCtx) {
        std::cout << "memory issue" << std::endl;
        return 0;
    } else {
        pCtx->version = "1";
        pCtx->log_level = 0;
    }
    
    /** handle logs */
    for (int v = 0; v < argc; ++v) {
        std::string param = argv[v];
        if (param == "--verbose") {
            //set log level to debug
            CLogger::getInstance().setLogLevel(PROGRAM_DEBUG_LEVEL);
            pCtx->log_level = 7;
            break;
        } else 
          { 
            CLogger::getInstance().setLogLevel(PROGRAM_NOTICE_LEVEL);
          }
    }
    
#if defined(DEBUG)|defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    //load all plugins
    CPluginManager *pPulginMgr = new CPluginManager(pCtx);
        
    //register signal SIGINT and signal handler  
    signal(SIGINT, signalHandler); 
    signal(SIGSEGV, signalHandler);
#ifdef __linux__
    signal(SIGKILL, signalHandler);
    signal(SIGTERM, signalHandler);
#endif
    
    /** commands to configure */
    for (int j = 0; j < argc; ++j) {
        //std::cout << argv[j] << std::endl;
        std::string param = argv[j];
        /*if (param == "--verbose") {
            //set log level to debug
            CLogger::getInstance().setLogLevel(PROGRAM_DEBUG_LEVEL);
        } else*/ if (param == "--version") {
            //print version
            std::cout << "version: 1.1" << std::endl;
        } else if (param == "--info") {
            //return info
            std::cout << "version: 1.1" << std::endl;
            std::cout << "name: packaging kit" << std::endl;
            std::cout << "desc: deploys group of components" << std::endl;
        } else if (param == "--config") {
            //load config
             std::string configPath = argv[j+1];
             j++;
        } else if (param == "--help") {
            //print usage
            std::cout << "usage [action] [parameter]" << std::endl;
            std::cout << "if no parameter is provided, the default value will be used" << std::endl;
            std::cout << "--<plugin name> <json plugin param> " << std::endl;
        } else if (param == "--service") {
            //start as interactive service
        }
    }
    
    /** action commands */
    for (int i = 0; i < argc; ++i) {
        //std::cout << argv[i] << std::endl;
        std::string pluginName = argv[i];
        if (pluginName == "--deploy") {
            CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "handling --deploy");
            if (argc > i+1) {
                std::string pluginParam = argv[i+1];
                //std::cout << pluginParam << std::endl;
                PROGRAM_INFO(pluginParam);
                
                if(pPulginMgr) {
    
                    CPluginInterface* pPlugin = pPulginMgr->getPluginHandle("deploy");
                    if(pPlugin != NULL) {
                        //CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "got plugin handle");
                        PROGRAM_DEBUG("got plugin handle");
                        std::string retString = pPlugin->entry((void*)pCtx, pluginParam);
                        CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "results: " + retString);
                        #if(PROGRAM_NOTICE_LEVEL == 5)
			 PROGRAM_NOTICE(retString);
		        #endif
                    } else {
                        CLogger::getInstance().log(PROGRAM_ERROR_LEVEL, "Unable to get plugin handle");
                    }
    
                } else {
                    PROGRAM_ERROR("Memory unavailable");
                }
            }
            
            i++;// skip next item - plugin param
        } else {
            if(!pPulginMgr) {
                PROGRAM_ERROR("invalid plugin loader handler.");
            } else {
                //change --sample to sample
                std::string namePlugin = pluginName;
                //example --deploy to deploy
                #if __cplusplus > 202101L
                    namePlugin.erase (std::remove(namePlugin.begin(), namePlugin.end(), '-'), namePlugin.end());
                #else
                    namePlugin.erase (0, 2);
                #endif
                CPluginInterface* pPlugin = pPulginMgr->getPluginHandle(namePlugin);
                if(pPlugin != NULL) {
                    PROGRAM_DEBUG("got plugin handle");
                    std::string pluginParam = argv[i+1];
                    i++;// skip next item - plugin param
                    PROGRAM_INFO(pluginParam);
                    std::string retString = pPlugin->entry((void*)pCtx, pluginParam);
                    //release version
                    #if(PROGRAM_NOTICE_LEVEL == 5)
			 PROGRAM_NOTICE(retString);
		    #endif
                    PROGRAM_DEBUG(retString);

                } else {
                    PROGRAM_INFO("Unable to get plugin");
                }
            }
        }
    }
    
    if(pCtx) {
        delete pCtx;
        pCtx = NULL;
    }
    if(pPulginMgr)delete pPulginMgr;
    std::cout << "exit program" << std::endl;
    return 0;
}
