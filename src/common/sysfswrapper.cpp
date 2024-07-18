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

#include "sysfswrapper.h"

std::mutex read_file_mutex;


/**
 * readfrompath : reads a particular value from a sysfs file
 * @param: std::string needs to be the exact file path 
 * @param: std::string& the read value to be a string value 
   @return: int error status 0 success, 1 failed to open 
*/
std::string sysfswrapper::readfrompath(std::string sysfspath, int& rescode ) {
read_file_mutex.lock(); 

    rescode = 0;
    std::string result; 

    std::ifstream file(sysfspath.c_str());
    std::string context;
    //todo: handle expection if file pointer is null. path is invalid
    while (getline(file, context, '\n')) {
        result.append(context); //after file close, buffer and stream are all gone
    }
    file.close();
read_file_mutex.unlock(); 
    return result;
}


/**
 * enumeratefolder: enuerate a folder and return a std::vector containing all file names
 * @param: std::string folder path 
 * @param: std::vector<std::string>&  a string vector contains all file names in the directory
   @return: int error status 0 success, 1 failed to open 
*/
int sysfswrapper::filesoffolder(std::string s_Dir, std::vector<std::string>& v_matchingFiles){
    int rescode = 0;

    if (s_Dir.empty()){
        rescode = -1;
        return rescode; 
    }

    read_file_mutex.lock();
    
    int namesize = 256;
    std::vector<directory_entry> vFiles;
    path p(s_Dir);
    if(is_directory(p)) {
        copy(directory_iterator(p), directory_iterator(), back_inserter(vFiles));
        for ( std::vector<directory_entry>::const_iterator it = vFiles.begin(); it != vFiles.end();  ++ it ) {
            string file_name = (*it).path().string();

            int srclen = (int)sizeof(file_name) + 1;
            if (namesize < srclen) {
                std::cout << "error!! filename length is over the allocated size: " << file_name.c_str() <<'\n';
                continue; 
            }
            v_matchingFiles.push_back(file_name);
            
        }
    } else {
        //log the error, for now use cout, will change to universal logging
        cout << "passed in value not a directory" << std::endl; 
    }
    read_file_mutex.unlock();

    return rescode; 
}


/**
 * enumeratefolder: enuerate a folder and return a std::vector containing all file which name contains the passed in value
 * @param: std::string folder path 
 * @param: std::string the file name to search 
 * @param: std::vector<std::string>&  a string vector contains all file names in the directory
   @return: int error status 0 success, 1 error
*/
int sysfswrapper::matchingfilesoffolder(std::string s_Dir, std::string s_filename, std::vector<std::string>& v_matchingFiles){
    int rescode = 0;

    if (s_Dir.empty() || s_filename.empty()){
        rescode = -1;
        return rescode; 
    }

    read_file_mutex.lock();
    
    int namesize = 256;
    std::vector<directory_entry> vFiles;
    path p(s_Dir);
    if(is_directory(p)) {
        copy(directory_iterator(p), directory_iterator(), back_inserter(vFiles));
        for ( std::vector<directory_entry>::const_iterator it = vFiles.begin(); it != vFiles.end();  ++ it ) {
            string file_name = (*it).path().string();
            bool bFound = false; 
            std::size_t pos = std::string::npos; 
            if (std::string::npos != (pos = s_filename.find("*"))){
                //ignore if the * is at the beginning or the end
                if (pos == 0 || pos == s_filename.size()-1){
                    Helper::erase_all(s_filename, "*");
                    if (file_name.find(s_filename) != std::string::npos) 
                        bFound = true;                     
                } else {
                    std::string s1 = s_filename.substr(0, pos); 
                    std::string s2 = s_filename.substr(pos+1);
              
                    if (!s1.empty() && !s2.empty()){
                        std::size_t n_s1 = file_name.find(s1);
                        std::size_t n_s2 = file_name.find(s2);
                        if (n_s1 != std::string::npos && n_s2 != std::string::npos && n_s2 >= (n_s1+ s1.size())) 
                            bFound = true;  
                    }               
                }
            } else {
                if (file_name.find(s_filename) != std::string::npos) 
                    bFound = true; 
            }

            if (bFound)            
            {
                int srclen = (int)sizeof(file_name) + 1;
                if (namesize < srclen) {
                    std::cout << "error!! filename length is over the allocated size: " << file_name.c_str() <<'\n';
                    continue; 
                }
                v_matchingFiles.push_back(file_name);
            }
        }
    } else {
        //log the error, for now use cout, will change to universal logging
        cout << "passed in value not a directory" << std::endl; 
    }
    read_file_mutex.unlock();
    return rescode; 
}





















