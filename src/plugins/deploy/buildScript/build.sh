#!/bin/bash
#Purpose: builds deploy plugin, self-test binary
#Version:
#Parameters
#     "--clean" for clean build
#     "--build_dir=<dir_path>" for custom build

RED='\033[0;31m'
NC='\033[0m'

 
function print_error() {
    echo -e "${RED}*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************"
    echo -e "*************** WARNING: Error in Build Process ****************"
    echo -e "*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************${NC}"
    exit 1
}

########################## build main project - BEGIN ##############################################

BUILD_DIR="build"

CPPMS_RELPATH="../../CppMicroServices-3.4.0/$BUILD_DIR"
CPPMS_ABSPATH=$(readlink -e $CPPMS_RELPATH)
export CppMicroServices_DIR=$CPPMS_ABSPATH
    
#rm -fr ../build && 
if [ -d "$BUILD_DIR" ]
then
    echo $BUILD_DIR "directory exists"
else
    echo "creating $BUILD_DIR directory..."
    mkdir $BUILD_DIR
fi

cd $BUILD_DIR && cmake -G"Unix Makefiles" ../.. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON && make -j $(nproc) || print_error
########################## build main project - END ##############################################


    
