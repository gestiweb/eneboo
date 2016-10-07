#!/bin/bash
export CROSS='i586-mingw32msvc-'

./build.sh -prefix 'C:\eneboo' -platform 'win32-g++-cross' -quick -console && ./clean_build.sh "eneboo-build-win32-quick"
