#!/bin/bash
DIR="$( cd -P "$( dirname "$0" )" && pwd )"
cd "$DIR"
VER="2.4.5.1"

REBUILD_QT=auto
OPT_PREFIX=""
OPT_QMAKESPEC=""
OPT_DEBUG=yes
OPT_SQLLOG=no
OPT_FLFCGI=no

OPT_HOARD=no
# Hoard se desactiva por defecto. Eneboo funciona principalmente con un 
# único hilo y hoard es un "memory allocator" orientado a mejorar la 
# velocidad de malloc/free para multihilo, en sistemas con más de un 
# núcleo.

OPT_QWT=yes
OPT_DIGIDOC=yes
OPT_WIN64=no
OPT_QWS=no
OPT_MULTICORE=yes
OPT_AQ_DEBUG=no
OPT_QUICK_CLIENT=no
OPT_MAKE_SILENT=yes
OPT_DEBUGGER=no
OPT_NEBULA_BUILD=no
OPT_FIXXP=no

QT_CHECK=" -DQT_CHECK_NULL"
QT_DEBUG=""
QT_DEBUG_OPT="-release"
QSADIR=qsa
QT_CONFIG_VERBOSE=""
if [ -e ".svn" -a "$BUILD_NUMBER" == "" ]; then
  BUILD_NUMBER="$(svnversion . -n)"
fi

if [ -e ".git" -a "$BUILD_NUMBER" == "" ]; then
  APPEND="$(git diff --quiet -- src/flbase || echo '-dev')"
  BUILD_NUMBER="$(git describe --tags)$APPEND"
fi

if [ "$BUILD_NUMBER" == "" ]; then
  SRC_VERSION="${DIR##*/eneboo-}"
  if [ "$SRC_VERSION" != "$DIR" -a "$SRC_VERSION" != "" ]; then
    BUILD_NUMBER=$SRC_VERSION
  fi
fi

if [ "$BUILD_NUMBER" == "" ]; then
  BUILD_NUMBER="unknown-$(date +%Y-%m-%d)"
fi


for a in "$@"; do
  case "$a" in
    -nebula)
      OPT_NEBULA_BUILD=yes
    ;;
    -verbose)
      OPT_MAKE_SILENT=no
    ;;
    -silent)
      OPT_MAKE_SILENT=yes
    ;;
    -static)
      QT_DEBUG="$QT_DEBUG -static"
    ;;
    -quick)
      OPT_QUICK_CLIENT=yes  
    ;;
    -dbadmin)
      OPT_QUICK_CLIENT=no
      OPT_DEBUGGER=yes
    ;;
    -single)
      OPT_MULTICORE=no
    ;;
    -rebuild-qt)
      REBUILD_QT=yes
    ;;
    -no-debugger)
      OPT_DEBUGGER=no
    ;;
    -debug)
      OPT_DEBUG=yes
    ;;
    -no-debug)
      OPT_DEBUG=no
      BUILD_NUMBER="$BUILD_NUMBER-nodbg"
    ;;
    -debugger)
      OPT_DEBUGGER=yes
      BUILD_NUMBER="$BUILD_NUMBER-debugger"
    ;;
    -aqdebug)
      OPT_AQ_DEBUG=yes
    ;;
    -no-check)
     QT_CHECK="-DQT_NO_CHECK "
    ;;
    -check)     
     QT_CHECK="-DQT_CHECK_STATE -DQT_CHECK_RANGE -DQT_CHECK_NULL -DQT_CHECK_MATH "
    ;;
    -thread)     
     QT_DEBUG="$QT_DEBUG -DQT_THREAD_SUPPORT "
    ;;
    -qtdebug)
      QT_DEBUG_OPT="-debug"
      BUILD_NUMBER="$BUILD_NUMBER-qtdebug"
    ;;
    -sqllog)
      OPT_SQLLOG=yes
    ;;
    -hoard)
      OPT_HOARD=yes
    ;;
    -no-hoard)
      OPT_HOARD=no
    ;;
    -qwt)
      OPT_QWT=yes
    ;;
    -digidoc)
      OPT_DIGIDOC=yes
    ;;
    -no-digidoc)
      OPT_DIGIDOC=no
    ;;
    -win64)
      OPT_WIN64=yes
    ;;
    -flfcgi)
      OPT_FLFCGI=yes    
    ;;
    -fixxp)
      OPT_FIXXP=yes
    ;;
    -qws)
      OPT_QWS=yes
      OPT_QMAKESPEC="qws/linux-generic-g++"
      export QMAKESPEC=$OPT_QMAKESPEC
    ;;
    -prefix|-platform)
      VAR=`echo $a | sed "s,^-\(.*\),\1,"`
      shift
      VAL="yes"
    ;;
    *)
    if [ "$VAL" == "yes" ];then
      VAL=$a
    fi
    ;;
  esac
  case "$VAR" in
    prefix)
    if  [ $VAL != "yes" ];then
      OPT_PREFIX=$VAL
    fi
    ;;
    platform)
    if  [ $VAL != "yes" ];then
      OPT_QMAKESPEC=$VAL
      export QMAKESPEC=$OPT_QMAKESPEC
    fi
    ;;
  esac
done
QT_DEBUG="$QT_DEBUG $QT_CHECK"
CMD_MAKE="make"
if [ "$OPT_MAKE_SILENT" == "yes" ]; then
  CMD_MAKE="$CMD_MAKE -s "
else
  QT_CONFIG_VERBOSE="-v"
fi


MAKE_INSTALL=""
QT_DEBUG="$QT_DEBUG $QT_DEBUG_OPT"

BUILD_MACX="no"
if [ "$OPT_QMAKESPEC" == "macx-g++" ]; then
  BUILD_MACX="yes"
  OPT_MULTICORE="no"
  OPT_DIGIDOC="no"  
fi
if [ "$OPT_QMAKESPEC" == "macx-g++-cross" ]; then
  BUILD_MACX="yes"
fi
if [ "$OPT_QUICK_CLIENT" == "yes" ]; then
  QT_DEBUG="$QT_DEBUG -DFL_QUICK_CLIENT"
  BUILD_NUMBER="$BUILD_NUMBER-quick"
else
  BUILD_NUMBER="$BUILD_NUMBER-dba"
fi
if [ "$OPT_NEBULA_BUILD" == "yes" ]; then
  QT_DEBUG="$QT_DEBUG -DAQ_NEBULA_BUILD"
  BUILD_NUMBER="$BUILD_NUMBER-nebula"
fi

if [ "$OPT_MULTICORE" == "yes" ]; then
  PROCESSORS=$(expr  $(cat /proc/cpuinfo | grep processor | tail -n 1 | sed "s/.*:\(.*\)/\1/") + 1)
  # CMD_MAKE="make -k -j $PROCESSORS -s "
  CMD_MAKE="$CMD_MAKE -k -j $PROCESSORS "
fi
  
unlink src/libmysql 2>/dev/null
if [ "$BUILD_MACX" == "no" ]; then
  if [ "$OPT_QWS" = "no" ]; then
    QT_DEBUG="$QT_DEBUG -DQT_NO_COMPAT"
  fi
  ln -s libmysql_std src/libmysql
else
  ln -s libmysql_macosx src/libmysql
fi

if [ "$OPT_QMAKESPEC" == "" ]; then
  case `uname -m` in
  amd64 | x86_64)
    OPT_QMAKESPEC="linux-g++-64"
    export QMAKESPEC=$OPT_QMAKESPEC
  ;;
  *)
    OPT_QMAKESPEC="linux-g++"
    export QMAKESPEC=$OPT_QMAKESPEC
  ;;
  esac
fi

VERSION="$VER (Build $BUILD_NUMBER)"
BUILD_KEY="$VER-Build"

echo -e "\nUtilidad de compilación e instalación de Eneboo $VERSION ( - ESTABLE - )"
echo -e "(C) 2003-2013 InfoSiAL, S.L. http://infosial.com - http://abanq.org\n"
echo -e "(C) 2012 Gestiweb Integración de Soluciones Web S.L.  http://www.gestiweb.com \n"

if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" -o "$OPT_QMAKESPEC" == "macx-g++-cross" ];then
  export CC=${CROSS}gcc
  export CXX=${CROSS}g++
  export LD=${CROSS}ld
  export AR=${CROSS}ar
  export AS=${CROSS}as
  export NM=${CROSS}nm
  export STRIP=${CROSS}strip
  export RANLIB=${CROSS}ranlib
  export DLLTOOL=${CROSS}dlltool
  export OBJDUMP=${CROSS}objdump
  export RESCOMP=${CROSS}windres
  export WINDRES=${CROSS}windres
if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" ];then

  OPT_PREFIX="$PWD/src/qt"
  BUILD_KEY="$VER-Build-mingw32-4.2"
else
  OPT_PREFIX="$PWD/src/qt"
  BUILD_KEY="$VER-$CROSS"
fi
else
MAKE_INSTALL="install"
fi

if [ "$OPT_PREFIX" == "" ]
then
  echo -e "AVISO : No se ha especificado directorio de instalación"
  echo -e "Uso :  $0 directorio_de_instalacion\n"
  DIRINST=$PWD/eneboo-build
  echo -e "Utilizando por defecto el directorio $DIRINST\n"
else
  DIRINST=$OPT_PREFIX
fi

mkdir -p $DIRINST

if [ ! -w $DIRINST ]
then
  echo -e "ERROR : Actualmente no tienes permisos de escritura en el directorio de instalación ($DIRINST)."
  echo -e "Solución : Cambia los permisos o ejecuta este script como un usuario que tenga permisos de escritura en ese directorio.\n"
  exit 1
fi

BASEDIR=$PWD
PREFIX=$DIRINST

echo -e "Directorio de instalación : $PREFIX\n"

echo -e "El comando MAKE es: $CMD_MAKE\n"

echo -e "Estableciendo configuración...\n"

cd $BASEDIR/src/libdigidoc/openssl/crypto
rm -f opensslconf.h
if [ "$OPT_QMAKESPEC" == "linux-g++-64" ]; then
  ln -s opensslconf.h.64 opensslconf.h
else
  ln -s opensslconf.h.32 opensslconf.h
fi
cd $BASEDIR

rm -f $HOME/.qmake.cache

export QTDIR=$BASEDIR/src/qt
mkdir $QTDIR/bin 2>/dev/null
if [ ! -f $QTDIR/include/qglobal.h ]
then
  cd $QTDIR
  $CMD_MAKE -f Makefile.cvs
  cd $BASEDIR
fi

version=$(cat $QTDIR/include/qglobal.h | grep "QT_VERSION_STR" | sed "s/.*\"\(.*\)\"/\1/g")
echo -e "Versión de Qt... $version\n"
echo -e "Compilando qmake y moc...\n"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QTDIR/lib:../lib
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$QTDIR/lib:../lib
cd $QTDIR

if [ "$OPT_HOARD" = "yes" ]
then
  echo "CONFIG *= enable_hoard" >> tools/designer/app/hoard.pri
fi
if [ "$REBUILD_QT" = "auto" ]
then
  if $CMD_MAKE qmake-install 2>/dev/null
  then
    REBUILD_QT=no
  else
    REBUILD_QT=yes
  fi
fi

export ORIGIN=\\\$\$ORIGIN

if [ "$REBUILD_QT" = "yes" ]
then
  $CMD_MAKE confclean 2>/dev/null
  if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" ];then
    cd $BASEDIR
    TMP_QTDIR=$(echo $QTDIR | sed "s/\//\\\\\\\\\\\\\\//g")
    cat qconfig/qconfig.h.in | sed "s/@BKEY@/$BUILD_KEY/" > qconfig/qconfig.h
    cp -fv qconfig/qconfig.h src/qt/include
    cp -fv qconfig/qmodules.h src/qt/include
    cd $QTDIR
  if [ "$OPT_WIN64" = "yes" ]
  then
    cp -fv mkspecs/win32-g++-cross/qmake.conf-w64 mkspecs/win32-g++-cross/qmake.conf
    cp -fv mkspecs/win32-g++-cross/qtcrtentrypoint.cpp-w64 mkspecs/win32-g++-cross/qtcrtentrypoint.cpp
  else
    cp -fv mkspecs/win32-g++-cross/qmake.conf-w32 mkspecs/win32-g++-cross/qmake.conf
    cp -fv mkspecs/win32-g++-cross/qtcrtentrypoint.cpp-w32 mkspecs/win32-g++-cross/qtcrtentrypoint.cpp
  fi
 
  ./configure --win32 $QT_CONFIG_VERBOSE -prefix $PREFIX -L$PREFIX/lib $QT_DEBUG -thread -stl -no-pch -no-exceptions -platform linux-g++ \
              -xplatform win32-g++-cross -buildkey $BUILD_KEY -disable-opengl -no-cups -no-nas-sound \
              -no-nis -qt-libjpeg -qt-gif -qt-libmng -qt-libpng -qt-imgfmt-png -qt-imgfmt-jpeg -qt-imgfmt-mng
else
    cp -vf Makefile.qt Makefile
    if [ "$BUILD_MACX" == "yes" ]; then
      mkdir -p $DIRINST/lib
      if [ "$OPT_QMAKESPEC" == "macx-g++" ]; then
        ./configure $QT_CONFIG_VERBOSE -platform $OPT_QMAKESPEC $QT_DEBUG -prefix $PREFIX -thread -stl -no-pch -no-exceptions \
                    -buildkey $BUILD_KEY -disable-opengl -no-cups -no-ipv6 -no-nas-sound -no-nis -qt-libjpeg \
                    -qt-gif -qt-libmng -qt-libpng -qt-imgfmt-png -qt-imgfmt-jpeg -qt-imgfmt-mng || exit 1
      else
        ./configure $QT_CONFIG_VERBOSE -platform linux-g++ -xplatform $OPT_QMAKESPEC $QT_DEBUG -prefix $PREFIX -thread -stl -no-pch \
                    -no-exceptions -buildkey $BUILD_KEY -disable-opengl -no-cups -no-ipv6 -no-nas-sound -no-nis -qt-libjpeg \
                    -qt-gif -qt-libmng -qt-libpng -qt-imgfmt-png -qt-imgfmt-jpeg -qt-imgfmt-mng || exit 1
      fi
    else
      # Configure informa de que no se encuentra MySQL o PostgreSQL, pero si se agrega:
      # -I/usr/include/mysql/ -I/usr/include/postgresql/
      # , se incluyen, pero luego aparece un error para libpg: LOCALEDIR undefined.
      # rpath: -R $ORIGIN/../lib , sirve para que en Linux busque las librerías (también) de forma relativa al ejecutable del programa.
      # ... pero el símbolo dólar ($) da problemas tanto en bash como en make. En bash hay que escaparlo, y en make hay que duplicar el símbolo dolar.
      # ... hacer uso del flag "-continue" puede desembocar en que no se aplique el -rpath.
    if [ "$OPT_QWS" = "no" ]; then      
      ./configure $QT_CONFIG_VERBOSE --x11 -platform $OPT_QMAKESPEC -prefix $PREFIX -R'$$(ORIGIN)/../lib' -R'$$(ORIGIN)/../../lib' -L$PREFIX/lib $QT_DEBUG -thread -stl -L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu  \
                  -no-pch -no-exceptions -xinerama -buildkey $BUILD_KEY -disable-opengl -no-cups \
                  -no-nas-sound -no-nis -qt-libjpeg -qt-gif -qt-libmng -qt-libpng -qt-imgfmt-png -qt-imgfmt-jpeg -qt-imgfmt-mng || exit 1
    else
      ./configure $QT_CONFIG_VERBOSE -depths 8,16,32 -qvfb -qt-gfx-vnc -platform $OPT_QMAKESPEC -prefix $PREFIX -R'$$(ORIGIN)/../lib' -R'$$(ORIGIN)/../../lib' -L$PREFIX/lib $QT_DEBUG -thread -stl -L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu  \
            -no-pch -no-exceptions -buildkey $BUILD_KEY -disable-opengl -no-cups \
            -no-nas-sound -no-nis -qt-libjpeg -qt-gif -qt-libmng -qt-libpng -qt-imgfmt-png -qt-imgfmt-jpeg -qt-imgfmt-mng || exit 1
    fi
    fi
  fi
fi
$CMD_MAKE qmake-install || exit 1
$CMD_MAKE moc-install || exit 1

cd $BASEDIR

export PATH=$PREFIX/bin:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PREFIX/lib
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$PREFIX/lib

echo -e "\nComprobando qmake...\n"
$QTDIR/bin/qmake -v > /dev/null
if [ $? = 154 ]
then
  echo -e "OK : qmake encontrado\n"
else
  echo -e "ERROR : No se encuentra qmake, esta utilidad se proporciona con las Qt."
  echo -e "        Comprueba que se encuentra en la ruta de búsqueda (variable $PATH).\n"
  exit 1
fi

cd $BASEDIR

mkdir -p $PREFIX/share/eneboo/images
mkdir -p $PREFIX/share/eneboo/forms
mkdir -p $PREFIX/share/eneboo/tables
mkdir -p $PREFIX/share/eneboo/translations
mkdir -p $PREFIX/share/eneboo/scripts
mkdir -p $PREFIX/share/eneboo/queries
mkdir -p $PREFIX/share/eneboo/reports
mkdir -p $PREFIX/share/eneboo/tmp
mkdir -p $PREFIX/share/eneboo/doc
mkdir -p $PREFIX/share/eneboo/packages
mkdir -p $PREFIX/lib
mkdir -p $PREFIX/bin

if [ "$AQ_CIN" == "" ]; then
  AQ_CIN="C = C"
fi

if [ "$AQ_PACK_VER" == "" ]; then
  AQ_PACK_VER="(qstrlen(V) > 0 && qstrcmp(AQPACKAGER_VERSION, V) == 0)"
fi


echo -e "\n\nRecopilando datos de la compilación \n"

if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" -o "$OPT_QMAKESPEC" == "macx-g++-cross" ];then
DATOS_CROSS="Versión CROSS-GCC: $CC ( $(${CROSS}gcc -v 2> temp && cat temp | grep 'gcc ver' | cut -f3 -d ' ') ) \n"
fi
DATOS_COMPILACION="\n Versión eneboo: $VERSION \n  Mkspecs: $OPT_QMAKESPEC \n $DATOS_CROSS Make: $CMD_MAKE \n Versión GCC: $(gcc -v 2> temp && cat temp | grep 'gcc ver' | cut -f3 -d ' ') \n\n Opciones de Compilación : \n QWT: $OPT_QWT \n DIGIDOC: $OPT_DIGIDOC \n MULTICORE: $OPT_MULTICORE \n HOARD: $OPT_HOARD \n REBUILD_QT: $REBUILD_QT \n QT_DEBUG_OPT: $QT_DEBUG_OPT" 


cat > AQConfig.h <<EOF
// ** $(date +%d%m%Y):$PREFIX -> AQConfig.h
// ** AQConfig.h  Generated by $0
// ** WARNING!    All changes made in this file will be lost!

#ifndef AQCONFIG_H_
#define AQCONFIG_H_

#include "qplatformdefs.h"
#include "AQGlobal.h"

#define AQ_DIRAPP                   AQConfig::aqDirApp
#define AQ_PREFIX                   AQ_DIRAPP
#define AQ_QTDIR                    AQ_DIRAPP
#define AQ_KEYBASE                  AQConfig::aqKeyBase
#define AQ_DATA                     AQConfig::aqData
#define AQ_LIB                      AQConfig::aqLib
#define AQ_BIN                      AQConfig::aqBin
#define AQ_USRHOME                  AQConfig::aqUsrHome
#define AQ_VERSION                  "$VERSION"
#define AQ_CIN(C)                   $AQ_CIN
#define AQPACKAGER_VERSION_CHECK(V) $AQ_PACK_VER
#define ENB_DATOS_COMP              "$DATOS_COMPILACION"

class QApplication;
class FLApplication;

class AQ_EXPORT AQConfig
{
public:
  static QString aqDirApp;
  static QString aqKeyBase;
  static QString aqData;
  static QString aqLib;
  static QString aqBin;
  static QString aqUsrHome;
        
private:
  static void init(QApplication *);
  friend class FLApplication;
};

#endif /*AQCONFIG_H_*/
EOF

cat > AQConfig.cpp <<EOF
// ** $(date +%d%m%Y):$PREFIX -> AQConfig.cpp
// ** AQConfig.cpp  Generated by $0
// ** WARNING!    All changes made in this file will be lost!

#include <qapplication.h>
#include <qdir.h>

#include "AQConfig.h"

AQ_EXPORT QString AQConfig::aqDirApp;
AQ_EXPORT QString AQConfig::aqKeyBase;
AQ_EXPORT QString AQConfig::aqData;
AQ_EXPORT QString AQConfig::aqLib;
AQ_EXPORT QString AQConfig::aqBin;
AQ_EXPORT QString AQConfig::aqUsrHome;

void AQConfig::init(QApplication *aqApp)
{
#if defined(Q_OS_MACX)
  aqDirApp = QDir::cleanDirPath(aqApp->applicationDirPath() + QString::fromLatin1("/../../../.."));
#else
  aqDirApp = QDir::cleanDirPath(aqApp->applicationDirPath() + QString::fromLatin1("/.."));
#endif
  aqKeyBase = QString::fromLatin1("Eneboo/");
  aqData = aqDirApp + QString::fromLatin1("/share/eneboo");
  aqLib = aqDirApp + QString::fromLatin1("/lib");
  aqBin = aqDirApp + QString::fromLatin1("/bin");
  aqUsrHome = QDir::cleanDirPath(QDir::home().absPath());
}
EOF

echo "include(./includes.pri)" > settings.pro
echo "PREFIX = $PREFIX" >> settings.pro
echo "ROOT = $BASEDIR" >> settings.pro
echo "INCLUDEPATH *= $PREFIX/include" >> settings.pro
echo "INCLUDEPATH *= $BASEDIR/src/qt/src/tmp" >> settings.pro
echo "CONFIG += warn_off" >> settings.pro
echo "AQSSLVERSION = 0.9.8" >> settings.pro
echo "DEFINES *= AQSTRSSLVERSION=\\\"098\\\"" >> settings.pro
#echo "DEFINES *= AQ_NO_PRINT_FUN" >> settings.pro
#echo "DEFINES *= AQ_NO_DEBUG_FUN" >> settings.pro

if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" ];then
  if [ "$OPT_DEBUG" == "yes" ];then
    echo "OBJECTS_DIR = .o/debug-shared-mt/" >> settings.pro
    echo "MOC_DIR = .moc/debug-shared-mt/" >> settings.pro
  else
    echo "OBJECTS_DIR = .o/release-shared-mt/" >> settings.pro
    echo "MOC_DIR = .moc/release-shared-mt/" >> settings.pro
  fi
fi

if [ "$CROSS" == "i686-apple-darwin8-" -o "$CROSS" == "powerpc-apple-darwin8-" ];then
    echo "DEFINES *= SQLITE_WITHOUT_ZONEMALLOC" >> settings.pro
fi

if [ "$OPT_DEBUG" = "yes" ]
then
  echo "CONFIG *= debug" >> settings.pro
  echo "DEFINES *= FL_DEBUG" >> settings.pro
fi

if [ "$OPT_DEBUGGER" = "yes" ]
then
  if [ "$OPT_DEBUG" = "no" ]
  then
    echo "DEFINES *= FL_DEBUG" >> settings.pro
  fi
  echo "DEFINES *= QSDEBUGGER QSDEBUGGER_VISUAL AQ_DEBUGGER FL_DEBUGGER" >> settings.pro  
fi

if [ "$OPT_AQ_DEBUG" = "yes" ]
then
  echo "DEFINES *= AQ_DEBUG" >> settings.pro
fi

if [ "$OPT_SQLLOG" = "yes" ]
then
  echo "DEFINES *= FL_SQL_LOG" >> settings.pro

fi

if [ "$OPT_QWT" = "yes" ]
then
  echo "CONFIG *= enable_qwt" >> settings.pro
  echo "DEFINES *= FL_QWT" >> settings.pro
fi

if [ "$OPT_DIGIDOC" = "yes" ]
then
  echo "CONFIG *= enable_digidoc" >> settings.pro
  echo "DEFINES *= FL_DIGIDOC" >> settings.pro
fi

if [ "$OPT_WIN64" = "yes" ]
then
  echo "CONFIG *= enable_win64" >> settings.pro
  echo "DEFINES *= AQ_WIN64" >> settings.pro
fi

if [ "$OPT_FIXXP" = "yes" ]
then
 echo "DEFINES *= ENEBOO_FIXXP" >> settings.pro
fi

if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" ];then
  # pthread
  cd $BASEDIR/src/pthreads
  $QTDIR/bin/qmake pthreads.pro
  $CMD_MAKE || exit 1
  mkdir -p $BASEDIR/src/qt/src/tmp/
  cp -fv pthread.h $BASEDIR/src/qt/src/tmp/
  cp -fv semaphore.h $BASEDIR/src/qt/src/tmp/
  cp -fv sched.h $BASEDIR/src/qt/src/tmp/
  cd $BASEDIR
  # flfcgi para windows
if [ ! -f /opt/mac/cross/flfastcgi_includes/fastcgi.h -a "$OPT_FLFCGI" = "yes" ]; then
    OPT_FLFCGI=no
else
cp -fv /opt/mac/cross/flfastcgi_includes/* $BASEDIR/src/qt/include
cp -fv /opt/mac/cross/spawn-fcgi.exe $BASEDIR/src/qt/bin/spawn-fcgi.exe
if [ "$OPT_WIN64" = "yes" ];then
cp -fv /opt/mac/cross/libfcgi_64.dll $BASEDIR/src/qt/lib/libfcgi.dll
cp -fv /opt/mac/cross/libfcgi_64.dll $BASEDIR/src/qt/bin/libfcgi.dll
else
cp -fv /opt/mac/cross/libfcgi_32.dll $BASEDIR/src/qt/lib/libfcgi.dll
cp -fv /opt/mac/cross/libfcgi_32.dll $BASEDIR/src/qt/bin/libfcgi.dll
fi
fi
fi

if [ "$OPT_FLFCGI" = "yes" ]
then
  echo "CONFIG *= enable_flfcgi" >> settings.pro
fi

if [ "$OPT_HOARD" = "yes" ]
then
  echo "CONFIG *= enable_hoard" >> settings.pro
  echo "DEFINES *= FL_HOARD" >> settings.pro
  echo -e "\nCompilando Hoard...\n"
  cd $BASEDIR/src/hoard
  $QTDIR/bin/qmake hoard.pro
  $CMD_MAKE || exit 1
  cd $BASEDIR
fi


echo -e "\nCompilando Qt ($QTDIR) ...\n"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QTDIR/lib:$PREFIX/lib
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$QTDIR/lib:$PREFIX/lib

if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" ];then
  export QTDIR=$BASEDIR/src/qt
  cd $QTDIR/src
  $QTDIR/bin/qmake -spec $QTDIR/mkspecs/win32-g++-cross -o Makefile.main qtmain.pro
  $CMD_MAKE -f Makefile.main || exit 1
fi
if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" ];then
  cd $QTDIR/tools/designer/uic
  $CMD_MAKE || exit 1
fi

if  [ "$OPT_QMAKESPEC" == "macx-g++-cross" ];then
 case `uname -m` in
  amd64 | x86_64)
    ln -s /opt/mac/cross/uic_64 $PWD/src/qt/bin/uic   
  ;;
  *)
    ln -s /opt/mac/cross/uic_32 $PWD/src/qt/bin/uic
  ;;
  esac
ln -s /opt/mac/SDKs/MacOSX10.4u.sdk/usr/lib/libiconv.dylib $PWD/src/qt/lib/libiconv.dylib
if  [ "$CROSS" == "i686-apple-darwin8-" ];then
ln -s /opt/mac/cross/libz_i686.dylib $PWD/src/qt/lib/libz.dylib
else
ln -s /opt/mac/cross/libz.1.2.7_ppc.dylib $PWD/src/qt/lib/libz.dylib
fi
fi

cd $QTDIR
$CMD_MAKE $MAKE_INSTALL || exit 1

export QTDIR=$PREFIX

echo "Compilando QSA..."
cd $BASEDIR/src/$QSADIR
cp -fv ../qt/.qmake.cache .qmake.cache
cp -fv ../qt/.qmake.cache src/$QSADIR/
cp -fv ../qt/.qmake.cache src/plugin/
if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" -o "$OPT_QMAKESPEC" == "macx-g++-cross" ];then
  cd configure2
  export QTDIR=/usr/share/qt3
  /usr/bin/qmake-qt3 -nocache -spec linux-g++ configure2.pro
  $CMD_MAKE || exit 1
  export QTDIR=$PREFIX
  cd $BASEDIR/src/qt
  for license in LICENSE.*
  do
    mv "$license" "old_license${license#LICENSE}"
  done
  rm -fr LICENSE.*
  touch LICENSE
  # svn up 2> /dev/null
  cd $BASEDIR/src/$QSADIR
  rm -fr $BASEDIR/src/qt/LICENSE
  for license in $BASEDIR/src/qt/old_license.*
  do
    mv "$license" "LICENSE${license#$BASEDIR/src/qt/old_license}"
  done
    ./configure2/configure2
  $QTDIR/bin/qmake CONFIG+="shared"
else
./configure
fi
$CMD_MAKE || exit 1
$CMD_MAKE $MAKE_INSTALL || exit 1

# ----> para que sirve tanto make?
#$CMD_MAKE
#$CMD_MAKE $MAKE_INSTALL
#$CMD_MAKE
#$CMD_MAKE $MAKE_INSTALL
# <----------------------

cd $BASEDIR
echo ""
echo -e "Creando Makefiles con qmake...\n"

cp -f src/qt/.qmake.cache .qmake.cache
cp -f src/qt/.qmake.cache src/advance/
cp -f src/qt/.qmake.cache src/qwt/
cp -f src/qt/.qmake.cache src/qwt/designer
cp -f src/qt/.qmake.cache src/barcode/
cp -f src/qt/.qmake.cache src/flbase/
cp -f src/qt/.qmake.cache src/fllite/
cp -f src/qt/.qmake.cache src/flbase/
cp -f src/qt/.qmake.cache src/kugar/
cp -f src/qt/.qmake.cache src/sqlite/
cp -f src/qt/.qmake.cache src/libpq/
cp -f src/qt/.qmake.cache src/dbf/
cp -f src/qt/.qmake.cache src/plugins/designer/flfielddb/
cp -f src/qt/.qmake.cache src/plugins/designer/fltabledb/
cp -f src/qt/.qmake.cache src/plugins/sqldrivers/sqlite/
cp -f src/qt/.qmake.cache src/plugins/sqldrivers/psql/
cp -f src/qt/.qmake.cache src/plugins/styles/bluecurve/
echo $QTDIR/bin/qmake user.pro
$QTDIR/bin/qmake user.pro
#echo $QTDIR/bin/qmake src/src.pro
#$QTDIR/bin/qmake src.pro

echo -e " * * * Compilando Eneboo ...\n"
cd src/flbase
$QTDIR/bin/qmake flbase.pro
if  [ "$OPT_QMAKESPEC" == "win32-g++-cross" ];then
  $CMD_MAKE mocables || exit 1
else
  $CMD_MAKE uicables || exit 1
fi

cd $BASEDIR

# A veces (en win32) no compila correctamente qwt, porque compila antes el designer plugin de qwt que la propia librería.
# , por eso, esperamos que el make falle y lo re-ejecutamos, para que complete.
$CMD_MAKE || { $CMD_MAKE || exit 1; }
$CMD_MAKE $MAKE_INSTALL
make -s $MAKE_INSTALL

cd $BASEDIR/src/libdigidoc/openssl/crypto
rm -f opensslconf.h
ln -s opensslconf.h.32 opensslconf.h
cd $BASEDIR

if  [ "$BUILD_MACX" == "yes" ];then
	echo -e "\nConfigurando packete app ...\n"
	CMD_INST_NAME_TOOL=${CROSS}install_name_tool
  
	${CROSS}install_name_tool -change libqsa.1.dylib @executable_path/../../../../lib/libqsa.1.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libqt-mt.3.dylib @executable_path/../../../../lib/libqt-mt.3.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libqwt.5.dylib @executable_path/../../../../lib/libqwt.5.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libflbase.2.dylib @executable_path/../../../../lib/libflbase.2.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libadvance.0.dylib @executable_path/../../../../lib/libadvance.0.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libflmail.1.dylib @executable_path/../../../../lib/libflmail.1.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libhoard.3.dylib @executable_path/../../../../lib/libhoard.3.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libcrypto.0.dylib @executable_path/../../../../lib/libcrypto.0.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change liblibdigidoc.2.dylib @executable_path/../../../../lib/liblibdigidoc.2.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libssl.0.dylib @executable_path/../../../../lib/libssl.0.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libxml2.2.dylib @executable_path/../../../../lib/libxml2.2.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libxslt.1.dylib @executable_path/../../../../lib/libxslt.1.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libqt-mt.3.dylib @executable_path/../../../../lib/libqt-mt.3.dylib $PREFIX/bin/designer.app/Contents/MacOS/designer
	${CROSS}install_name_tool -change libqt-mt.3.dylib @executable_path/../../../../lib/libqt-mt.3.dylib $PREFIX/bin/linguist.app/Contents/MacOS/linguist
	${CROSS}install_name_tool -change libqt-mt.3.dylib @executable_path/../../../../lib/libqt-mt.3.dylib $PREFIX/bin/lupdate
	${CROSS}install_name_tool -change libexslt.0.dylib @executable_path/../../../../lib/libexslt.0.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libxsltproc.1.dylib @executable_path/../../../../lib/libxsltproc.1.dylib $PREFIX/bin/Eneboo.app/Contents/MacOS/Eneboo
	${CROSS}install_name_tool -change libqt-mt.3.dylib @executable_path/../../../../lib/libqt-mt.3.dylib $PREFIX/bin/kudesigner.app/Contents/MacOS/kudesigner

	${CROSS}install_name_tool -change libqt-mt.3.dylib @executable_path/../../../../lib/libqt-mt.3.dylib $PREFIX/bin/teddy.app/Contents/MacOS/teddy

        mkdir -p $PREFIX/bin/designer.app/Contents/plugins/designer
        cp -f $PREFIX/plugins/designer/*.dylib $PREFIX/bin/designer.app/Contents/plugins/designer 2> /dev/null
	
	for i in $(find $PREFIX -type f -name "*.dylib" -print)
	do
		${CROSS}install_name_tool -change libqsa.1.dylib @executable_path/../../../../lib/libqsa.1.dylib $i
		${CROSS}install_name_tool -change libqt-mt.3.dylib @executable_path/../../../../lib/libqt-mt.3.dylib $i
		${CROSS}install_name_tool -change libqwt.5.dylib @executable_path/../../../../lib/libqwt.5.dylib $i
		${CROSS}install_name_tool -change libflbase.2.dylib @executable_path/../../../../lib/libflbase.2.dylib $i
		${CROSS}install_name_tool -change libadvance.0.dylib @executable_path/../../../../lib/libadvance.0.dylib $i
		${CROSS}install_name_tool -change libkdefxx.1.dylib @executable_path/../../../../lib/libkdefxx.1.dylib $i
		${CROSS}install_name_tool -change libpq.4.dylib @executable_path/../../../../lib/libpq.4.dylib $i
		${CROSS}install_name_tool -change libsqlite.2.dylib @executable_path/../../../../lib/libsqlite.2.dylib $i
		${CROSS}install_name_tool -change libmysqlclient.15.dylib @executable_path/../../../../lib/libmysqlclient.15.dylib $i
		${CROSS}install_name_tool -change libflmail.1.dylib @executable_path/../../../../lib/libflmail.1.dylib $i
		${CROSS}install_name_tool -change libcrypto.0.dylib @executable_path/../../../../lib/libcrypto.0.dylib $i
		${CROSS}install_name_tool -change liblibdigidoc.2.dylib @executable_path/../../../../lib/liblibdigidoc.2.dylib $i
	  ${CROSS}install_name_tool -change libssl.0.dylib @executable_path/../../../../lib/libssl.0.dylib $i
	  ${CROSS}install_name_tool -change libxml2.2.dylib @executable_path/../../../../lib/libxml2.2.dylib $i
	  ${CROSS}install_name_tool -change libxslt.1.dylib @executable_path/../../../../lib/libxslt.1.dylib $i
	  ${CROSS}install_name_tool -change libexslt.0.dylib @executable_path/../../../../lib/libexslt.0.dylib $i
	  ${CROSS}install_name_tool -change libxsltproc.1.dylib @executable_path/../../../../lib/libxsltproc.1.dylib $i
	done
fi
if [ "$OPT_DEBUG" = "no" -a "$OPT_QMAKESPEC" == "win32-g++-cross" ]; then
  ${CROSS}strip --strip-unneeded $PREFIX/bin/* 2> /dev/null
  ${CROSS}strip --strip-unneeded $PREFIX/lib/* 2> /dev/null
  ${CROSS}strip --strip-unneeded $PREFIX/plugins/designer/* 2> /dev/null
  ${CROSS}strip --strip-unneeded $PREFIX/plugins/sqldrivers/* 2> /dev/null
  ${CROSS}strip --strip-unneeded $PREFIX/plugins/styles/* 2> /dev/null
fi
if [ "$OPT_DEBUG" = "no" -a "$OPT_QMAKESPEC" != "win32-g++-cross" -a "$BUILD_MACX" == "no" ]
then
  strip --strip-unneeded $PREFIX/bin/* 2> /dev/null
  strip --strip-unneeded $PREFIX/lib/* 2> /dev/null
  strip --strip-unneeded $PREFIX/plugins/designer/* 2> /dev/null
  strip --strip-unneeded $PREFIX/plugins/sqldrivers/* 2> /dev/null
  strip --strip-unneeded $PREFIX/plugins/styles/* 2> /dev/null
  cd src/translations
  ./update.sh 2> /dev/null
  cd ../..
fi

echo -e "\nTerminando compilación...\n"
cp -f ./src/fllite/images/icon*.png $PREFIX/share/eneboo/images 2> /dev/null
cp -f ./src/forms/*.ui $PREFIX/share/eneboo/forms 2> /dev/null
cp -f ./src/tables/*.mtd $PREFIX/share/eneboo/tables 2> /dev/null
cp -f ./src/translations/*.ts $PREFIX/share/eneboo/translations 2> /dev/null
#cp -f ./src/scripts/*.qs $PREFIX/share/eneboo/scripts 2> /dev/null
rm $PREFIX/share/eneboo/scripts/* 2> /dev/null
cp -f ./src/scripts/*.qs $PREFIX/share/eneboo/scripts 2> /dev/null

if [ "$OPT_QWS" != "no" ]; then
  cp -fr ./src/qt/lib/fonts $PREFIX/lib/ 2> /dev/null
fi

cp -f ./src/docs/*.html $PREFIX/share/eneboo/doc 2> /dev/null
cp -f ./src/*.xml $PREFIX/share/eneboo 2> /dev/null
cp -f ./src/*.xpm $PREFIX/share/eneboo 2> /dev/null
# cp -f ./packages/*.eneboo $PREFIX/share/eneboo/packages 2> /dev/null

echo -e "\n * Eneboo $VERSION - Compilación terminada. * \n"

