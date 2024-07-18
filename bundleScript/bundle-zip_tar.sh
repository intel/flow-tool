#!/bin/sh
# Script to generate zip/tar deployment package

set -e

RED='\033[0;31m'
NC='\033[0m'

print_error() {
    echo -e "${RED}*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************"
    echo "*************** WARNNING: Error in Packaging Process ****************"
    echo -e "*************** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ****************${NC}"
    exit 1
}

trap '[ $? -eq 0 ] && exit 0 || print_error' EXIT

#BASEDIR=$(dirname $0)
BASEDIR="$( cd "$( dirname "$0" )" && pwd )"
echo $BASEDIR

echo "Prepare..."

BASE="flow-tool"
MAJOR="1"
MINOR="03"
ARCH="x86_64"
BUILD_DATE=$(date +'%y%m%d-%H%M')
VERSION=$MAJOR.$MINOR.$BUILD_DATE
#<name>-<Version>-<Revision>-<Architecture>.deb
FILENAME="$BASE-$VERSION-$ARCH"

if [ $(uname -r | sed -n 's/.*\( *Microsoft *\).*/\1/ip') ];
then
        #WSL VM
        SOURCEFOLDER=/home/$USER/$FILENAME
else
	#native linux 
        SOURCEFOLDER=$FILENAME
fi
echo "DEBUG: Target folder $SOURCEFOLDER"

#echo "create new directory"
rm -fr $SOURCEFOLDER
mkdir -p $SOURCEFOLDER

#tool path
BUILD_DIR=$BASEDIR/../buildScript/out
if [ -d "$BUILD_DIR" ]
then
    #echo $BUILD_DIR "directory exists"
    echo .
else
    echo "Error: $BUILD_DIR directoy not found..."
    print_error
    exit
fi

#copy tool
cp -r $BUILD_DIR/* $SOURCEFOLDER/

#zip/tar folder
mkdir -p $BASEDIR/bundle
tar -czvf $BASEDIR/bundle/$FILENAME.tar.gz $SOURCEFOLDER
sha512sum $BASEDIR/bundle/$FILENAME.tar.gz > $BASEDIR/bundle/$FILENAME.tar.gz.sha512sum.txt

zip -vr $BASEDIR/bundle/$FILENAME.zip $SOURCEFOLDER/
sha512sum $BASEDIR/bundle/$FILENAME.zip > $BASEDIR/bundle/$FILENAME.zip.sha512sum.txt

#cleanup
rm -fr $SOURCEFOLDER

echo "done"
