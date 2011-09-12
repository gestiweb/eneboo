!x11:mac {
   macx-g++:QMAKE_LFLAGS_PREBIND    = -seg1addr 0xB2000000
   macx:LIBS += -framework Carbon -framework QuickTime
   PRECOMPILED_HEADER = kernel/qt_pch.h
   *-mwerks:INCLUDEPATH += compat
   mac9 {
     LIBS       += "MSL C++.PPC.Lib" "MSL SIOUX.Carbon.Lib" "CarbonLib" \
                   "MSL RuntimePPC.Lib" "MSL C.Carbon.Lib" 
     #INCLUDEPATH += "MacOS 9:CarbonLib_1.2_SDK:Carbon Support:Universal Interfaces:CIncludes" 
   }
}
