
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

#include "metricsBase.h"

    
//! A private class
/*!
A private inner class that implements a system service;
see ICapabilityProvider for details of the service.
Provide system software info 
*/    
class SoftwareInfo : metricsBase {
    public:
          //! Public variable
        /*! OS type*/    
        std::string _os;
        //! Public variable
        /*! release # */    
        std::string release;
        //! Public variable
        /*! OS version */                
        std::string version;

        //! Public function
        /*! get the details of the software */    
        int get_details(std::string& staticValues);
        //! Public function
        /*! get a json string of software details */                
        std::string tojson(bool authcall = false );
        //! Public function
        /*! get a json string of static software details */            
        std::string tojson_staticValue(bool authcall = false );
};

