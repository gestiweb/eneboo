#!/bin/bash

./build.sh -prefix "$(pwd)/eneboo-build-linux32-quick/" -platform "linux-g++-32" -flfcgi -quick && ./clean_build.sh "eneboo-build-linux32-quick"
