#!/bin/bash
#purpose: script to install build environment setup for lean tool  [framework and plugins]

#RED='\033[0;31m'
#NC='\033[0m'
#sed -i -e 's/\r$//' *.sh
#chmod  +x  *.sh
 
#function print_error() {
#    echo -e "${RED}*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************"
#    echo -e "*************** WARNING: Error in Framework Build Process ****************"
#    echo -e "*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************${NC}"
#    exit 1
#}


echo "executing setup_ubuntu script ..."

#BASEDIR=$(dirname $0)
BASEDIR="$( cd "$( dirname "$0" )" && pwd )"
echo "BASEDIR = $BASEDIR"

#cmake
sudo apt-get install -y cmake build-essential

#build framework
#sudo apt-get install -y libcap-dev libssl-dev

#build plugins
sudo apt-get install -y nlohmann-json3-dev

#bundle as zip
sudo apt-get install -y zip tar

##build json-schema validator
SCHEMABLD_DIR="$BASEDIR/json-schema-validator/build"

if [ ! -d "$SCHEMABLD_DIR" ]; then
    git clone https://github.com/pboettch/json-schema-validator
    cd $BASEDIR/json-schema-validator
    echo "set up .."
    #append flag for the bad value error when builing plugin
    line='set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")'
    cmakefile="CMakeLists.txt"
    temp=$(mktemp)
    echo "$line" > "$temp"
    #copy from content into the temp file
    cat "$cmakefile" >> "$temp"
    mv "$temp" "$cmakefile"
    rm "$temp"
else
    echo "skip downloading json validator as folder found $SCHEMABLD_DIR. assuming sync is already done."
    cd $BASEDIR/json-schema-validator
fi

mkdir -p $BASEDIR/json-schema-validator/build
cd $BASEDIR/json-schema-validator/build
echo "building"
cmake ..
make
sudo make install

cd $BASEDIR



