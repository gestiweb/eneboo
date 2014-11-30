#!/bin/bash
export CROSS='x86_64-w64-mingw32-'

./build.sh -prefix 'C:\eneboo' -platform 'win32-g++-cross' -win64 -quick && ./clean_build.sh "eneboo-build-win64-quick"

