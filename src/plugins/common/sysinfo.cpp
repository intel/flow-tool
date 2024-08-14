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

#include "sysinfo.h"
 
#include <filesystem>
#include <fstream>    
#include <nlohmann/json.hpp>
#include "helper.h"

/*
 * readSnapfile: Read the data from the platform snapshot file
 * @return: bool
 */
bool CSysInfo::readSnapfile(sysinfoData& osInfo)
{
    nlohmann::json snapInst;

    std::filesystem::path currentToolPath = std::filesystem::current_path();

    //verify the platform snap shot was created
    std::filesystem::path PlatSnapFile = currentToolPath / "platform-snapshot.json";

  try {
         if (!std::filesystem::exists(PlatSnapFile)) {
           return false;
        }
        std::ifstream fileHandler;
        fileHandler.open(PlatSnapFile);
        
	    if (fileHandler.is_open()) {
	        fileHandler >> snapInst;
	        if(snapInst.empty() ) {
	           fileHandler.close();
              return false;
          } else
          {
               nlohmann::json platform_info = snapInst["platform info"]; 
               pltinfo.kversion = platform_info["kernel"]["version"];
               std::string name = platform_info["os"]["name"]; 
               available = platform_info["storage"]["available"]; 
               pltName = platform_info["hardware"]["platform"]; 

               osVersion = platform_info["os"]["version"]; 
               osVersion = removeSpace(osVersion);
               removeSpace(name);
               pltinfo.distro = Helper::to_lower_copy(name);

            }
	        fileHandler.close();
        } 
   }  catch (const std::exception& e) {
	         std::cout << "precheck:file read failed "   << std::endl;
           return false;
     }
     
     return true;
}

/*
 * removeSpace: parse the extra space from the snapshot strings
 * @param: input string
 * @return: string
 */
std::string CSysInfo::removeSpace(std::string& input)
{
    size_t index = input.find(' ');

    if (index != std::string::npos) {
        input.erase(index ); 
    }
    //version return xx.xx.x
    size_t pos = input.find('.', input.find('.') + 1);
    std::string version = input.substr(0, pos);
    return version;
}

/*
 * extractString: parse the snapshot strings
 * @param: input string
 * @return: string
 */
std::string CSysInfo::extractString(std::string& input)
{
    size_t index = input.find('.');

    if (index != std::string::npos) {
        size_t second = input.find('.', index + 1);
        if (second != std::string::npos) {
           input.erase(second ); 
        }
    }

    return input;
}

/*
 * getAvailableStorage: return the available storage
 * @param: total space
 * @return: bool
 */
bool CSysInfo::getAvailableStorage(std::string& diskspace)
 {

   unsigned long long spaceRequired = 6ULL * 1024 * 1024 * 1024; // 6GB 
   
   try {    
	if (!available.empty()) {
           std::istringstream strstream(available);
           double counter;
           char unitType;
           unsigned long long value = 1;
	     
          strstream >> counter;
          strstream.get(unitType);

          switch (unitType) {
            case 'K':
              value = 1024;
             break;
            case 'M':
              value = 1024 * 1024;
            break;
            case 'G':
              value = 1024 * 1024 * 1024;
            break;
           case 'T':
              value = 1024ULL * 1024 * 1024 * 1024;
            break;
           default:
            
            break;
          }
            //static for both due to conversion errors
           unsigned long long spaceAvl = static_cast<unsigned long long>(counter * static_cast<double>(value));
       
           return spaceAvl < spaceRequired;
       }


    }
    catch (const std::exception& e) {
	      std::cout << "precheck:storage failed "   << std::endl;
           return false;
     }
   return false; 
 }
 
 /*
 * getKernelVersion: read the kernel version
 * @param: version
 * @return: bool
 */
 bool CSysInfo::getKernelVersion(std::string version)
 {

   try {
        int majorPlt, minorPlt = 0;
        int majorVer, minorVer =0;
        char delimit;
       
       //parse before the delimit
        std::stringstream ss(pltinfo.kversion);
        ss >> majorPlt >> delimit >> minorPlt;

        //get the value after the delimit
        std::stringstream ssVer(version);
        ssVer >> majorVer>> delimit >> minorVer ;

         if(majorPlt < majorVer) {
            return false;
         }
         if(minorPlt >= minorVer ) {
             return true;
          }

      } 
    catch (const std::exception& e) {
	        std::cout << "precheck:getKernelVersion failed "   << std::endl;
           return false;
   }
   return false; 
 } 
 
 /*
 * getDistroInfo: read the distro from the manifest
 * @param: string, vector, string
 * @return: bool
 */ 
bool CSysInfo::getDistroInfo(std::string distroName,std::vector<std::string> osdistroversions, std::string pkgset )
{

    bool result = false;
    sysinfoData osInfo;

  try {
        std::string osversion="";

       if(pltinfo.distro != distroName) {
         // std::cout << "distro name mismatched " << distroName << std::endl;
          return false; 
       }  

    //verify the distro versions
      for (const std::string& versions : osdistroversions)
      {
        if(osVersion == versions) 
        {
          result = true;
   
          break;
        }
      }
  } catch (const std::exception& e) {
       std::cout << "precheck:getOSversion failed "   << std::endl;
       return false;
    }
       
   return result;
   
}

 /*
 * getPltInfo: read the plaform supported from the manifest
 * @param: string
 * @return: bool
 */ 
bool CSysInfo::getPltInfo(std::vector<std::string> platversions)
{
   bool result = false;

   for (const std::string& platforms : platversions)
   {  
     if(platforms == pltName)
      {
          result = true;
          break;
      }
   }
 
  return result;
}

 /*
 * getSysInfo: read and verify the info from the manifest file
 * @param: struct
 * @return: bool
 */ 
bool CSysInfo::getSysInfo(const sysinfoData& sysdata ) {

  CSysInfo systemInfos;
  std::string diskspace;

  sysinfoData osInfo;

    try {
        systemInfos.readSnapfile(osInfo);
        if(sysdata.pkgsets == "gimp") { 
          
          bool value = systemInfos.getKernelVersion(sysdata.kversion );

          if(!value )
          {
            std::cout << "kernel mismatched# " << sysdata.kversion << std::endl;
            return false;
          }

          if(systemInfos.getAvailableStorage(diskspace))
          {
            std::cout <<"insufficient space :avalable is:" << diskspace <<"G" <<std::endl; 
            return false;
          }
          
          if(!systemInfos.getPltInfo(sysdata.platform))
          {
             std::cout <<"platform unsupported:"  << systemInfos.pltName << std::endl; 
             return false;
          }

       }

       if(!systemInfos.getDistroInfo(sysdata.distro,sysdata.distroVersion, sysdata.pkgsets ))
       {
         std::cout <<"os version mismatched:"  <<std::endl; 
         return false;
       }
   } catch (const std::exception& e) {
        std::cout << "precheck: failed "   << std::endl;
        return false;
    }

    return true;
}

 
