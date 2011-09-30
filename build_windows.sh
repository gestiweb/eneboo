#!/bin/bash
export CROSS='i586-mingw32msvc-'

./build.sh -prefix 'C:\abanq' -platform 'win32-g++-cross'

./dist_windows.sh

