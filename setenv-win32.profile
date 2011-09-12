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
  
export QTDIR=$PWD/src/qt
export PATH=$QTDIR/bin:$PATH
export QMAKESPEC=win32-g++-cross

