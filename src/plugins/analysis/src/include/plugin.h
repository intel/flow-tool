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

/**
 * Plugin Class registed with plugin manager
 */
#include "analysis.h"
#include "pluginInterface.h"


class CPlugin : public CPluginInterface {
private:
    CAnalysis* pInstance;
    
public:
    CPlugin();
    ~CPlugin();

    int init(void* pCxt);
    void deinit(void* pCxt);

    std::string getInfo(void* pCxt);
    std::string entry(void* pCxt, std::string sReq);
    void cancel(void* pCxt, std::string sReq);
    
};

