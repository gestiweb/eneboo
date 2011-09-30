#!/bin/bash
export CROSS='i586-mingw32msvc-'

./build.sh -prefix 'C:\eneboo' -platform 'win32-g++-cross'

./dist_windows.sh

