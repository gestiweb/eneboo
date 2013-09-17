#!/bin/sh

# where does your java-sdk live?
# if you only have a jre installed, use the variable
# JRE_HOME below to define its location
export JAVA_HOME="$JAVA_HOME"

# where is your jre installed? 
export JRE_HOME="$JAVA_HOME/jre"

# specify the directory where the wrapper library is stored in.
export JAVA_WRAPPER_LIB_DIR="./java_wrapper"

# specify the classpath to the original java classes you
# use in your application
export CLASSPATH="./javalib.jar"

# command and arguments to call your application
export APPLICATION="./demo.bin"
export ARGS="$@"

# end of configuration section ----------------------------------------------

env LD_LIBRARY_PATH="$JRE_HOME/lib/i386/client:$JRE_HOME/lib/i386:$JAVA_WRAPPER_LIB_DIR" \
  "$APPLICATION" \
  $ARGS \
  -Djava.class.path="$CLASSPATH"
