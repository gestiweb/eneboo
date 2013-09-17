#!/bin/bash

find . -name "*~" -exec rm -fR \{\} \;
rm -fR ./lib*
rm -fR aqreports
rm -fR java4cpp/java4cpp.conf
