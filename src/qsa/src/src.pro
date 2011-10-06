TEMPLATE	= subdirs
SUBDIRS		= qsa 
PRJ_MODULE      = QSA
QMAKE_CC              = @echo $$PRJ_MODULE ::   [CC___] $< && $$QMAKE_CC
QMAKE_CXX             = @echo $$PRJ_MODULE ::   [C++__] $< && $$QMAKE_CXX
shared:SUBDIRS += plugin
CONFIG += ordered
