#!/bin/bash

NEB_PORT=1818

if [ "$1" != "" ]
then
  NEB_PORT=$1
fi

xvfb-run -a --server-args="+bs -screen 0 1440x990x16" $PWD/nebula_v2 $PWD/abanq $NEB_PORT

