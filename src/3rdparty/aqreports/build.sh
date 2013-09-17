#!/bin/bash

OS_ID=$(uname)
QMAKE_APP=qmake
MAKE_APP=make

export BASE_DIR=$PWD
export WRAP_DIR=${BASE_DIR}/aqreports
export JAVACLS_DIR=${BASE_DIR}/javaclasses
export JAVA4CPP_DIR=${BASE_DIR}/java4cpp
export JAVA4CPP_CONF=${JAVA4CPP_DIR}/java4cpp.conf

if [ "$OS_ID" == "Darwin" ]
then
  export JAVA_HOME="/System/Library/Frameworks/JavaVM.framework/Home"
  if [ "$QTDIR" == "" ]
  then
    export QTDIR="${HOME}/abanq-macosx/lite-instalacion"
  fi
  export QMAKESPEC="macx-g++"
  QMAKE_APP="${QTDIR}/bin/qmake"
  
  if [ -x /usr/bin/ant ]
  then
    cd $JAVA4CPP_DIR
    /usr/bin/ant clean
    /usr/bin/ant
    cd ..
  fi
fi

. ${JAVA4CPP_DIR}/jvm-list.sh
. ${JAVA4CPP_DIR}/java-wrappers.sh

${JAVA4CPP_DIR}/gen-wrapper.sh

require_java_runtime

mkdir -p ${WRAP_DIR}
mkdir -p ${WRAP_DIR}/include
mkdir -p ${WRAP_DIR}/src

cd ${WRAP_DIR}/include
ln -s ${BASE_DIR}/*.h
ln -s ${BASE_DIR}/src/*.h
cd ${WRAP_DIR}/src
ln -s ${BASE_DIR}/*.cpp
ln -s ${BASE_DIR}/src/*.cpp

cd ${JAVACLS_DIR}
JAR_DEPS=""
for i in $(ls -1 *.jar)
do
	if [ "$JAR_DEPS" != "" ]; then
		JAR_DEPS="$JAR_DEPS:"
	fi
	JAR_DEPS="$JAR_DEPS$i" 
done

cat > ${WRAP_DIR}/include/aq_jar_deps.h <<EOF
#define JAR_DEPS "$JAR_DEPS"
EOF
 
cd ${WRAP_DIR}

mv -fv *.h include
mv -fv *.cpp src

####
# para rodear un bug en java4cpp que permite palabras clave (p.e. export)
# en los espacios de nombres
cat include/net_sf_jasperreports_engine_export_JRPrintServiceExporter.h | \
sed  "s/namespace export/namespace export_/" > tmp.h
mv -f tmp.h include/net_sf_jasperreports_engine_export_JRPrintServiceExporter.h
cat src/net_sf_jasperreports_engine_export_JRPrintServiceExporter.cpp | \
sed  "s/namespace export/namespace export_/" > tmp.cpp
mv -f tmp.cpp src/net_sf_jasperreports_engine_export_JRPrintServiceExporter.cpp

cat include/net_sf_jasperreports_engine_export_JRPrintServiceExporterParameter.h | \
sed  "s/namespace export/namespace export_/" > tmp.h
mv -f tmp.h include/net_sf_jasperreports_engine_export_JRPrintServiceExporterParameter.h
cat src/net_sf_jasperreports_engine_export_JRPrintServiceExporterParameter.cpp | \
sed  "s/namespace export/namespace export_/" > tmp.cpp
mv -f tmp.cpp src/net_sf_jasperreports_engine_export_JRPrintServiceExporterParameter.cpp

cat include/net_sf_jasperreports_engine_export_JRPrintServiceExporterParameter.h | \
sed  "s/::export::/::export_::/" > tmp.h
mv -f tmp.h include/net_sf_jasperreports_engine_export_JRPrintServiceExporterParameter.h
cat src/net_sf_jasperreports_engine_export_JRPrintServiceExporterParameter.cpp | \
sed  "s/::export::/::export_::/" > tmp.cpp
mv -f tmp.cpp src/net_sf_jasperreports_engine_export_JRPrintServiceExporterParameter.cpp
####

if [ "$OS_ID" == "Darwin" ]
then
$QMAKE_APP -project -t lib INCLUDEPATH*="${JAVA_HOME}/include ${WRAP_DIR}/include" \
                           TARGET=aqreports \
                           DESTDIR=${BASE_DIR}/lib \
                           CONFIG+="warn_off exceptions stl plugin" \
                           CONFIG-="qt debug thread" \
                           OBJECTS_DIR=.obj \
                           DEFINES+="AQ_OS_MACX" \
                           LIBS+="-framework JavaVM"
else
$QMAKE_APP -project -t lib INCLUDEPATH*="${JAVA_HOME}/include ${JAVA_HOME}/include/linux ${WRAP_DIR}/include" \
                           TARGET=aqreports \
                           DESTDIR=${BASE_DIR}/lib \
                           CONFIG+="warn_off exceptions stl plugin" \
                           CONFIG-="qt debug thread" \
                           OBJECTS_DIR=.obj \
                           LIBS+="-L${JAVA_HOME}/jre/lib/amd64/server -L${JAVA_HOME}/jre/lib/i386/server -ljvm"
fi

$QMAKE_APP -nocache
make
if [ "$OS_ID" != "Darwin" ]
then
 strip --strip-unneeded ${BASE_DIR}/lib/*
fi
cd ${BASE_DIR}

