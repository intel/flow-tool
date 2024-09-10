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

/** Implementation of Config Class */

#include "Config.h"
#include <iostream>

/** Sets default values */
Config::Config() {
}

Config::~Config() {
}


/** loads default json config data */
json Config::LoadDefaulConfig() {
    
json j = {
     
    {"os", json::array({  
        json{
            {"name", "name"},
            {"command", "cat /etc/os-release |grep PRETTY_NAME= |cut -f2 -d="}
        },
        json{
            {"name","version"},       
            {"command", "cat /etc/os-release |grep VERSION= |cut -f2 -d="}  
        }
    })},

    {"kernel", json::array({  
        json{
            {"name","version"},       
            {"command", "uname -a |awk '{print $3}'"}  
        }
    })},
    
    {"storage", json::array({  
        json{
            {"name","available"},       
            {"command", "df -h --output=avail --total | awk 'END {print $1}'"}  
        }
    })}, 
    
    {"software", json::array({  
        json{
            {"name","dpkglist"},
            {"command", "dpkg -l | grep ^ii | awk '{print $2 \" \" $3}'"}
        }, 
        json{
            {"name","snaplist"},
            {"command", "snap list |grep \"^[^Name]\" | awk '{print $1 \" \" $2}'"}
        },
        json{        
            {"name","appimage"},
            {"command", "find / -type f -name ''*.AppImage'' 2>/dev/null"}
        }
    })},
    
    {"localInstall", {          
            {"name", "/opt/intel/openvino"}
    
        }
    },
       
    {"firmware", json::array({  
        json{     
            {"name","PD"},
            {"command", ""}
        },
        json{
            {"name", "timer"},
            {"command", ""}      
        }
    })},      
       
    {"audio", json::array({  
        json{
            {"name","PD"},
            {"command", ""}
        },
        json{
            {"name", "timer"},
            {"command", ""}      
        }
    })},   
    
    {"network", json::array({  
        json{    
            {"name","ethernet"},
            {"command", ""}
        },
        json{
            {"name", "wifi"},
            {"command", ""}      
        }
    })},    
    
    {"ifwi", json::array({  
        json{    
            {"name","ifwi"},
            {"command", ""}
        },
        json{
            {"name", "timer"},
            {"command", ""}      
        }
    })},    
                 
    {"hardware", json::array({  
        json{    
            {"name","platform"},
            {"command", "grep -oP \"model name\\K.*\" /proc/cpuinfo | head -1"}
        },
        json{
            {"name", ""},
            {"command", ""}      
        }
    })}              
 };
 
 return j;     
     
}
