#!/bin/bash
export CROSS='powerpc-apple-darwin8-'
export PATH=$PATH:/opt/mac/bin

./build.sh -prefix "$(pwd)/eneboo-build-macx" -platform "macx-g++-cross" -quick
