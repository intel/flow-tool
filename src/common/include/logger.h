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
#include <map>
#include <sstream>
//#include <stacktrace>

#define PROGRAM_DEBUG_LEVEL 7
#define PROGRAM_INFO_LEVEL 6
#define PROGRAM_NOTICE_LEVEL 5
#define PROGRAM_WARNING_LEVEL 4
#define PROGRAM_ERROR_LEVEL 3
#define PROGRAM_CRITICAL_LEVEL 2
#define PROGRAM_ALERT_LEVEL 1
#define PROGRAM_EMERGENCY_LEVEL 0

#define PROGRAM_DEBUG(...) PROGRAM_LOG(PROGRAM_DEBUG_LEVEL, __VA_ARGS__)
#define PROGRAM_INFO(...) PROGRAM_LOG(PROGRAM_INFO_LEVEL, __VA_ARGS__)
#define PROGRAM_NOTICE(...) PROGRAM_LOG(PROGRAM_NOTICE_LEVEL, __VA_ARGS__)
#define PROGRAM_WARNNING(...) PROGRAM_LOG(PROGRAM_WARNING_LEVEL, __VA_ARGS__)
#define PROGRAM_ERROR(...) PROGRAM_LOG(PROGRAM_ERROR_LEVEL, __VA_ARGS__)
#define PROGRAM_CRITICAL(...) PROGRAM_LOG(PROGRAM_CRITICAL_LEVEL, __VA_ARGS__)
#define PROGRAM_ALERT(...) PROGRAM_LOG(PROGRAM_ALERT_LEVEL, __VA_ARGS__)
#define PROGRAM_EMERGENCY(...) PROGRAM_LOG(PROGRAM_EMERGENCY_LEVEL, __VA_ARGS__)

#define PROGRAM_LOG(n, ...) {std::ostringstream ss; ss << "[" MICRO_SERVICE_NAME "] "; logToString(ss, __VA_ARGS__); m_LogHandler(n, ss.str());}

/*#ifdef DEBUG_PROGRAM_ENABLED
#define PROGRAM_DEBUG(...) PROGRAM_LOG(PROGRAM_DEBUG_LEVEL, __VA_ARGS__)
#else
#define PROGRAM_DEBUG(...) do {} while(0);
#endif*/;

typedef void (*loggerFuncPtr)(int level, std::string msg);

template <typename... T>
inline void logToString(std::ostringstream &ss, T... args) {
  (ss << ... << args);
}


/* Reuse the same directives used in interface file*/
#ifdef PROGRAM_LOG
#undef PROGRAM_LOG
#endif
#define PROGRAM_LOG(n, ...) {std::ostringstream ss; ss  << "[Framework] "; logToString(ss, __VA_ARGS__); CLogger::getInstance().log(n, ss.str());}

static std::map<int, std::string> PROGRAM_LOG_PREFIX {
    {PROGRAM_DEBUG_LEVEL, "DEBUG"},
    {PROGRAM_INFO_LEVEL, "INFO"},
    {PROGRAM_NOTICE_LEVEL, "NOTICE"},
    {PROGRAM_WARNING_LEVEL, "WARNING"},
    {PROGRAM_ERROR_LEVEL, "ERROR"},
    {PROGRAM_CRITICAL_LEVEL, "CRITICAL"},
    {PROGRAM_ALERT_LEVEL, "ALERT"},
    {PROGRAM_EMERGENCY_LEVEL, "EMERGENCY"}
};

class CLogger {
public:
    static CLogger& getInstance() {
        static CLogger instance;
        return instance;
    }

    void log(int level, std::string msg) {
        if (level <= logLevel) {
            if (level < PROGRAM_NOTICE_LEVEL) {
                std::cerr << PROGRAM_LOG_PREFIX[level] << msg << std::endl;
            } else {
                std::cout << PROGRAM_LOG_PREFIX[level] << msg << std::endl;
            }
        }
        /* print stacktrace for serverity less than PROGRAM_ERROR_LEVEL */
        if (level < PROGRAM_CRITICAL_LEVEL) {
//#ifdef __linux__
  //          std::cout << std::stacktrace::current() << std::endl();
//#endif
        }
    }

    void setLogLevel(int level) {
        logLevel = level;
    }

    int getLogLevel() {
        return logLevel;
    }

private:
    int logLevel = PROGRAM_WARNING_LEVEL;
    
    CLogger() {}
    ~CLogger() {}
    
    CLogger(CLogger const&) = delete;
    
    void operator=(CLogger const&) = delete;
};
