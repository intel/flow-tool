#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'
#sed -i -e 's/\r$//' *.sh
 
function print_error() {
    echo -e "${RED}*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************"
    echo -e "*************** WARNING: Error in Framework Build Process ****************"
    echo -e "*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************${NC}"
    exit 1
}

# generate build files
cmake -S .. -B build -DCMAKE_BUILD_TYPE=Release

# build
cmake --build build || print_error

# package
# cd build && cpack -G DEB && cd ..
