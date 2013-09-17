#!/bin/bash

echo "basedir    = ${BASE_DIR}" > $JAVA4CPP_CONF
echo "classfile  = ${JAVA4CPP_DIR}/javaclasses.txt" >> $JAVA4CPP_CONF
echo "targetpath = ${WRAP_DIR}" >> $JAVA4CPP_CONF
echo "verbose    = true" >> $JAVA4CPP_CONF

. ${JAVA4CPP_DIR}/jvm-list.sh
. ${JAVA4CPP_DIR}/java-wrappers.sh

export JAVA_JARPATH=${JAVA_JARPATH}:${JAVACLS_DIR}:${JAVA4CPP_DIR}/jar
require_java_runtime
find_jars ${JAVACLS_DIR}/*.jar
find_jars ${JAVA4CPP_DIR}/jar/*.jar

rm -fRv ${WRAP_DIR}
mkdir -p ${WRAP_DIR}

cp -fv ${JAVA4CPP_DIR}/src/c/* ${WRAP_DIR}

run_java org.kapott.wrappergen.c.Main $JAVA4CPP_CONF
