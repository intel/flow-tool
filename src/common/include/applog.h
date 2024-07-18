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

#include <string>
#include <stdio.h>

//todo: change CAppLog
class applog {
public:
    /*! destructor */
    applog(const applog& obj) = delete;
    
    /*! Function to get class instance*/
    static applog* getInstance() {
        if (instance == NULL) {
            instance = new applog();
        }
        return(instance);
    }
   
    //! Public function
    /*! todo: ??? */
    void Log(std::string logmessage, int logType);
    
    //! Public function
    /*! todo: ??? */
    void setLogInfo(bool bdebug = false); 
    
    enum _log_type {
        error = 1,
        warning = 2,
        info = 3,
    };
        
private:
    /*! private constructor */
    applog() { _blogInfo = false; }

    /*! boolean todo ??? */
    bool _blogInfo;
    
    /*! single instance */
    static applog* instance;
};
    



