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
 
#ifdef __linux__
#include <sys/wait.h>
#endif

#include "helper.h"

/**
 * Executes a command and returns the output as a string.
 *
 * @param cmd The command to be executed.
 * @param dummy An optional dummy parameter.
 * @return The output of the command as a string.
 */
std::string Helper::runCmd(const std::string cmd, int dummy) {
    int fd[2];
    const int buf_size = 1024;
    long int num_bytes = 0;
    pid_t pid;
    char buf[buf_size];
    std::string result = "";

    if (pipe(fd) == -1) {
        std::cout << "couldn't initiate command run" << std::endl;
        return result;
    }

    if ((pid = fork()) == -1) {
        std::cout << "couldn't run command" << std::endl;
        return result;
    }

    if (pid == 0) { // child process
        close(fd[0]); // close reading end in child process
        dup2(fd[1], 1); // redirect stdout
        dup2(fd[1], 2); // redirect stderr
        close(fd[1]);
        char *CMD[] = {(char *)"/bin/sh", (char *)"-c", (char *)cmd.c_str(), NULL};
        execve(CMD[0], CMD, NULL);
        std::cout << "failure in executing command" << std::endl;
        exit(EXIT_FAILURE);
    }
    close(fd[1]); // close writing end in parent process
    //memset(buf, '\0', buf_size);
    while ((num_bytes = read(fd[0], buf, buf_size)) > 0) {
        //std::cout << "***" << buf << std::endl;
        result.append(buf, num_bytes);
    }
    //result.push_back('\0');

    close(fd[0]);
    if (wait(NULL) == -1) {
        std::cerr << "error in executing child process" << std::endl;
    }
    (void)dummy;
    return result;
}


/**
 * Trims the specified characters from the end of the given string.
 * 
 * @param strTotrim The string to be trimmed.
 * @param trimChars The characters to be trimmed from the end of the string.
 */
void Helper::trimEnd( std::string& strTotrim, std::string trimChars)
{
    const std::string whiteSpaces( " \f\n\r\t\v" ); 
    if (trimChars.empty())
        trimChars =  whiteSpaces;
    std::string::size_type pos = strTotrim.find_last_not_of( trimChars );
    
    strTotrim.erase( pos + 1 );       
}


/**
 * @brief Trims leading characters from a string.
 * 
 * @param strTotrim The string to trim.
 * @param trimChars The characters to trim from the beginning of the string. 
 */
void Helper::trimHead( std::string& strTotrim, std::string trimChars)
{   
    const std::string whiteSpaces( " \f\n\r\t\v" ); 
    if (trimChars.empty())
        trimChars =  whiteSpaces;
    std::string::size_type pos = strTotrim.find_first_not_of( trimChars );
    
    strTotrim.erase( 0, pos );
}


/**
 * Trims the specified string by removing leading and trailing characters.
 * 
 * @param strTotrim The string to be trimmed.
 * @param trimChars The characters to be trimmed. If empty, whitespace characters will be used.
 */
void Helper::trim( std::string& strTotrim, std::string trimChars)
{
    const std::string whiteSpaces( " \f\n\r\t\v" ); 
    if (trimChars.empty()){   
        trimChars =  whiteSpaces;
    }
    Helper::trimEnd( strTotrim, trimChars );
    Helper::trimHead( strTotrim, trimChars );
    
    //std::cout << "*****trimed result: " << strTotrim << std::endl; 
    
} 

/**
 * @brief Erases all occurrences of a specified substring from a given string.
 * 
 * @param strOrigin The original string from which to remove the specified substring.
 * @param strToErase The substring to be removed from the original string.
 * @return The modified string with all occurrences of `strToErase` removed.
 */
std::string Helper::erase_all( std::string& strOrigin, std::string strToErase)
{
    if (!strOrigin.empty()) {
        std::regex pattern(strToErase); 
        std::string result = std::regex_replace(strOrigin, pattern, "");
        
        //std::cout << "*****erase_all result: " << result << std::endl;  
        return result; 
    }
    return strOrigin; 
}

/**
 * Converts a given string to lowercase and returns a copy of the converted string.
 *
 * @param str The string to be converted to lowercase.
 * @return A copy of the input string with all characters converted to lowercase.
 */
std::string Helper::to_lower_copy(std::string str){
    if (!str.empty()){
        std::string s = str; 
        transform(s.begin(), s.end(), s.begin(), ::tolower); 
        return s;
    } 
    return str; 
}
