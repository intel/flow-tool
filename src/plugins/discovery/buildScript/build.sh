#!/bin/bash
#Purpose: builds discovery plugin
#Version:
#Parameters

RED='\033[0;31m'
NC='\033[0m'

function print_error() {
    echo -e "${RED}*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************"
    echo -e "*************** WARNNING: Error in Build Process ****************"
    echo -e "*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************${NC}"
    exit 1
}

BUILD_DIR="build"
#rm -fr ../build &&
mkdir -p $BUILD_DIR
#mkdir "$BUILD_DIR/config"
cd $BUILD_DIR && cmake -G"Unix Makefiles" ../.. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON && make -j $(nproc) || print_error

