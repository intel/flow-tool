#!/bin/bash
#purpose: script to build entier Tool [includes framework and all plugins]
#sed -i -e 's/\r$//' *.sh

set -e

RED='\033[0;31m'
NC='\033[0m'

print_error() {
    echo "${RED}*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************"
    echo "*************** WARNNING: Error in Packaging Process ****************"
    echo "*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************${NC}"
    exit 1
}
trap '[ $? -eq 0 ] && exit 0 || print_error' EXIT

echo "Preparing ..."

#BASEDIR=$(dirname $0)
BASEDIR="$( cd "$( dirname "$0" )" && pwd )"
echo $BASEDIR

TARGETFOLDER_PATH="$BASEDIR/out"
mkdir -p $TARGETFOLDER_PATH  

#add license

#add user guide
#cp $BASEDIR/../docs/userGuide.txt $TARGETFOLDER_PATH/

echo "setting up development environment ..."
SETUPSCRIPT=$BASEDIR/../devSetup/
pushd $SETUPSCRIPT #fix otherwise it messes with basedir in other script.
sed -i -e 's/\r$//' *sh
chmod +x *.sh
#check distro
if test -f /etc/lsb-release; then
	echo "ubuntu file found!"
	source setup_ubuntu.sh || print_error
fi
if test -f /etc/redhat-release; then
	echo "redhat file found!"
	source setup_rhel.sh || print_error
fi
popd

#devSetup/setup* script not changing the build dir.
#BASEDIR="$( cd "$( dirname "$0" )" && pwd )"
#cd $BASEDIR

echo "building src ..."

ARRAY_BUILDSCRIPT_PATHS=("$BASEDIR/../src/framework/buildScript" "$BASEDIR/../src/plugins/deploy/buildScript" "$BASEDIR/../src/plugins/discovery/buildScript" "$BASEDIR/../src/plugins/analysis/buildScript")

for BUILDSCRIPT_PATH in "${ARRAY_BUILDSCRIPT_PATHS[@]}"; do
    echo "building: BUILDSCRIPT_PATH : $BUILDSCRIPT_PATH"
    chmod +x $BUILDSCRIPT_PATH/build.sh
    pushd $BUILDSCRIPT_PATH
	sed -i -e 's/\r$//' *sh
	chmod +x *.sh
    #source $BUILDSCRIPT_PATH/build.sh || print_error
    source build.sh || print_error
    
    echo "copy binaries to target folder ..."
    BUILDOUT_PATH=$BUILDSCRIPT_PATH/build
    
    #framework: copy tool binary
    if [[ $BUILDSCRIPT_PATH == *"framework"* ]]; then
        cp -r $BUILDOUT_PATH/lean* $TARGETFOLDER_PATH
        chmod +x $TARGETFOLDER_PATH/lean*
    fi

    #plugin: copy lib
    if [[ $BUILDSCRIPT_PATH == *"plugins"* ]]; then
        if [ -d "$BUILDOUT_PATH/lib" ]; then
            mkdir -p $TARGETFOLDER_PATH/lib
            
            find "$BUILDOUT_PATH/lib/" -type f -not -name "*libs*" -exec cp -t "$TARGETFOLDER_PATH/lib" {} +

      
        fi
        if [ -d "$BUILDOUT_PATH/config" ]; then
            mkdir -p $TARGETFOLDER_PATH/config
            cp -r $BUILDOUT_PATH/config/* $TARGETFOLDER_PATH/config
        fi
    fi
    
    popd
done

#BASEDIR="$( cd "$( dirname "$0" )" && pwd )"
#echo "BASEDIR=$BASEDIR"

echo "bundle target folder ..."
cd $TARGETFOLDER_PATH
zip -r $BASEDIR/tool.zip ./*

#cleanup
#cd ..
#rm -fr $TARGETFOLDER_PATH

echo "done"
