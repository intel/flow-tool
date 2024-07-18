#!/bin/bash
# purpose: script to setup build environment for lean tool  [framework and plugins] on RHEL/Fedora

#only non root users can use sudo
USE_SUDO=0

if [ "$EUID" -ne 0 ]; then 
	echo "not running as root"
	USE_SUDO=1
else
	echo "running as root"
fi

cat /etc/redhat-release

if [ $USE_SUDO -eq 1 ]; then

	echo "sudo dnf install"
	
	#sudo yum install gcc gcc-c++ kernel-devel make
	#sudo dnf group install "Development Tools"
	
	sudo dnf install -y clang cmake 
	#sudo dnf install llvm-toolset

	#sudo dnf install libcap-devel openssl-devel nlohmann-json3-devel
	sudo dnf install -y libcap-devel openssl-devel
	sudo dnf install -y json-devel

	#bundle
	sudo dnf install -y zip tar

	#rpm build
	sudo dnf install -y rpmdevtools rpmlint
else
	echo "dnf install as root"
	#yum install gcc gcc-c++ kernel-devel make
	#dnf group install "Development Tools"

	dnf install -y clang cmake 
	#dnf install llvm-toolset

	#dnf install libcap-devel openssl-devel nlohmann-json3-devel
	dnf install -y libcap-devel openssl-devel
	dnf install -y json-devel

	#bundle
	dnf install -y zip tar

	#rpm build
	dnf install -y rpmdevtools rpmlint
fi

##build json-schema validator for analysis plugin
SCHEMABLD_DIR="json-schema-validator"

if [ ! -d "$SCHEMABLD_DIR" ]; then
    git clone https://github.com/pboettch/json-schema-validator
    cd json-schema-validator
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
    cd json-schema-validator
fi

mkdir -p build
cd build
echo "building"
cmake ..
make

if [ $USE_SUDO -eq 1 ]; then
	sudo make install
else
	make install
fi

