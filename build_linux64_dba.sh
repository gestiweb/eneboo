#!/bin/bash

./build.sh -prefix "$(pwd)/eneboo-build-linux64-dba/" -platform "linux-g++-64" -dbadmin -flfcgi && ./clean_build.sh "eneboo-build-linux64-dba"
