QMAKE_MKDIR           = @echo $$PRJ_MODULE ::   [MKDIR] $< && $$QMAKE_MKDIR
QMAKE_COPY            = @echo $$PRJ_MODULE ::   [CP___] $< && $$QMAKE_COPY
QMAKE_CC              = @echo $$PRJ_MODULE ::   [CC___] $< && $$QMAKE_CC
QMAKE_CXX             = @echo $$PRJ_MODULE ::   [C++__] $< && $$QMAKE_CXX
QMAKE_UIC             = @echo $$PRJ_MODULE ::   [UIC__] $< && $$QMAKE_UIC
QMAKE_AR              = @echo $$PRJ_MODULE ::   [AR___] $< && $$QMAKE_AR
QMAKE_LINK            = @echo $$PRJ_MODULE ::   [LK___] $@ && $$QMAKE_LINK
QMAKE_LINK_SHLIB      = @echo $$PRJ_MODULE ::   [LK___] $@ && $$QMAKE_LINK_SHLIB
QMAKE_INSTALL_FILE    = @echo $$PRJ_MODULE ::   [INS__] $@ && $$QMAKE_INSTALL_FILE
QMAKE_INSTALL_PROGRAM = @echo $$PRJ_MODULE ::   [INS__] $@ && $$QMAKE_INSTALL_PROGRAM

QMAKE_SEPARATE_DEBUG_INFO         = @echo $$PRJ_MODULE ::   [DBG__] $@ && $$QMAKE_SEPARATE_DEBUG_INFO
QMAKE_INSTALL_SEPARATE_DEBUG_INFO = @echo $$PRJ_MODULE ::   [DBG__] $@ && $$QMAKE_INSTALL_SEPARATE_DEBUG_INFO

QMAKE_MOVE            = @echo $$PRJ_MODULE ::   [MV___] $@ && $$QMAKE_MOVE
QMAKE_DEL_FILE        = @echo $$PRJ_MODULE ::   [RM___] $@ && $$QMAKE_DEL_FILE 
QMAKE_DEL_DIR         = @echo $$PRJ_MODULE ::   [RMDIR] $@ && $$QMAKE_DEL_DIR
QMAKE_STRIP           = @echo $$PRJ_MODULE ::   [STRIP] $@ && $$QMAKE_STRIP
QMAKE_CHK_DIR_EXISTS  = @echo $$PRJ_MODULE ::   [CHK__] $@ && $$QMAKE_CHK_DIR_EXISTS

