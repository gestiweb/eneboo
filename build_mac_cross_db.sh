#!/bin/bash
export CROSS='i686-apple-darwin8-'
export PATH=$PATH:/opt/mac/bin

./build.sh -prefix "$(pwd)/eneboo-build-macx" -platform "macx-g++-cross" -debugger -dbadmin
