QMAKE_DIR             = @echo $$AQMODULE ::   [MKDIR] $< && $$QMAKE_MKDIR
QMAKE_COPY            = @echo $$AQMODULE ::   [CP___] $< && $$QMAKE_COPY
QMAKE_CC              = @echo $$AQMODULE ::   [CC___] $< && $$QMAKE_CC
QMAKE_CXX             = @echo $$AQMODULE ::   [C++__] $< && $$QMAKE_CXX
QMAKE_MOC             = @echo $$AQMODULE ::   [MOC__] $< && $$QMAKE_MOC
QMAKE_UIC             = @echo $$AQMODULE ::   [UIC__] $< && $$QMAKE_UIC
QMAKE_AR              = @echo $$AQMODULE ::   [AR___] $< && $$QMAKE_AR
QMAKE_LINK            = @echo $$AQMODULE ::   [LK___] $@ && $$QMAKE_LINK
QMAKE_LINK_SHLIB      = @echo $$AQMODULE ::   [LK___] $@ && $$QMAKE_LINK_SHLIB
QMAKE_INSTALL_FILE    = @echo $$AQMODULE ::   [INS__] $@ && $$QMAKE_INSTALL_FILE
QMAKE_INSTALL_PROGRAM = @echo $$AQMODULE ::   [INS__] $@ && $$QMAKE_INSTALL_PROGRAM 

QMAKE_SEPARATE_DEBUG_INFO         = @echo $$AQMODULE ::   [DBG__] $@ && $$QMAKE_SEPARATE_DEBUG_INFO
QMAKE_INSTALL_SEPARATE_DEBUG_INFO = @echo $$AQMODULE ::   [DBG__] $@ && $$QMAKE_INSTALL_SEPARATE_DEBUG_INFO

QMAKE_MOVE            = @echo $$AQMODULE ::   [MV___] $@ && $$QMAKE_MOVE
QMAKE_DEL_FILE        = @echo $$AQMODULE ::   [RM___] $@ && $$QMAKE_DEL_FILE 
QMAKE_DEL_DIR         = @echo $$AQMODULE ::   [RMDIR] $@ && $$QMAKE_DEL_DIR
QMAKE_STRIP           = @echo $$AQMODULE ::   [STRIP] $@ && $$QMAKE_STRIP
QMAKE_CHK_DIR_EXISTS  = @echo $$AQMODULE ::   [CHK__] $@ && $$QMAKE_CHK_DIR_EXISTS

headers.path = $$ABANQBLDDIR/include/$$TARGET
headers.files = $$HEADERS
headers.CONFIG += no_all
INSTALLS += headers

make_dir.commands = $$QMAKE_MKDIR $$ABANQBLDDIR/include/$$TARGET
make_dir.target = $$ABANQBLDDIR/include/$$TARGET

install_headers.output = $$ABANQBLDDIR/include/$$TARGET/${QMAKE_FILE_BASE}.h
install_headers.input = INSTALL_HEADERS
install_headers.commands = $$QMAKE_COPY ${QMAKE_FILE_NAME} $$make_dir.target
install_headers.CONFIG = no_link

QMAKE_EXTRA_TARGETS += make_dir
QMAKE_EXTRA_COMPILERS += install_headers

INSTALL_HEADERS = $$HEADERS $$GENERATED_HEADERS

PRE_TARGETDEPS += $$make_dir.target $$UIDIR $$GENERATED_HEADERS $$INST_HDRS $$INST_GEN_HDRS
