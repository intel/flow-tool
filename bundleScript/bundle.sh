#!/bin/bash

BASEDIR="$( cd "$( dirname "$0" )" && pwd )"
echo $BASEDIR

sed -i -e 's/\r$//' $BASEDIR/bundle-zip_tar.sh
source $BASEDIR/bundle-zip_tar.sh
