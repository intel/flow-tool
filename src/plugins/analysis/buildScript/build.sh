#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'

 
function print_error() {
    echo -e "${RED}*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************"
    echo -e "*************** WARNING: Error in Build Process ****************"
    echo -e "*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************${NC}"
    exit 1
}

echo "Building analysis plugin package"
# generate build files
cmake -S .. -B build -DCMAKE_BUILD_TYPE=Release

# build
cmake --build build || print_error


