#!/bin/bash

QMAKE_APP=qmake
MAKE_APP=make

export BASE_DIR=$PWD
export WRAP_DIR=${BASE_DIR}/aqreports
export JAVACLS_DIR=${BASE_DIR}/javaclasses
export JAVA4CPP_DIR=${BASE_DIR}/java4cpp
export JAVA4CPP_CONF=${JAVA4CPP_DIR}/java4cpp.conf

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

$QMAKE_APP -project -t lib INCLUDEPATH*="${JAVA_HOME}/include ${JAVA_HOME}/include/linux ${WRAP_DIR}/include" \
                           TARGET=aqreports \
                           DLLDESTDIR=${BASE_DIR}/lib \
                           CONFIG-="qt" \
                           CONFIG+="warn_off dll exceptions debug stl plugin" \
                           DEFINES="" \
                           OBJECTS_DIR=".obj" \
                           LIBS+="-L${BASE_DIR}/win32 -ljvm"
                                                       
$QMAKE_APP
$MAKE_APP
${STRIP} --strip-unneeded ${BASE_DIR}/lib/*.dll
cd ${BASE_DIR}

