#!/bin/bash
export CROSS='i586-mingw32msvc-'

./build.sh -prefix 'C:\eneboo' -platform 'win32-g++-cross' -dbadmin -flfcgi -fixxp && ./clean_build.sh "eneboo-build-win32-dba"
