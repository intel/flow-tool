#!/bin/bash
# purpose: script to clean temp files

rm -fr ../buildScript/out
rm -fr ../buildScript/tool.zip
rm -fr ../buildScript/tool.tar

rm -fr ../bundleScript/bundle

rm -fr json-schema-validator

rm -fr ../src/framework/buildScript/build
rm -fr ../src/plugins/analysis/buildScript/build
rm -fr ../src/plugins/deploy/buildScript/build
rm -fr ../src/plugins/discovery/buildScript/build
