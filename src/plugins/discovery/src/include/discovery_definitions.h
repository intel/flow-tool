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

/**Common definitions*/
#define MSG_INVALID_REQUEST "Invalid Request"
#define MSG_API_NOT_SUPPORTED "REST API not supported"
#define MSG_FAILURE "failure"
#define MSG_SUCCESS "success"
#define MSG_STATUS "status"
#define MSG_PROGRESS "progress"
#define MSG_DESC "desc"
#define MSG_DATA "data"
#define MSG_TOOMANY_REQUEST "Too Many Request"
#define MSG_MISSING_INFO "Invalid or missing info"
#define MSG_ENUMERATE "This API is replaced by API enumerate"
#define MSG_NOT_IMPLEMENTED "service not Implemented"
#define MSG_INTERNAL_ERROR "internal error"
#define MSG_MISSING_CLIENTINFO "Invalid request or request missing client info"

//todo: move to a different file later
#define COLLECT_STATUS "collect"
//#define OUTPUT_FILE "/usr/share/kit/json/output/platform-snapshot.json"
//#define CONFIG_FILE "/usr/share/kit/json/snapshot_config.json"
#define OUTPUT_FILE "platform-snapshot.json"
#define CONFIG_FILE "snapshot_config.json"
