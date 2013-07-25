TEMPLATE	= subdirs
#SUBDIRS		= linguist 
!win32 {
  SUBDIRS       += lupdate
  #SUBDIRS	+= lrelease \
  #		         lupdate \
  #		         qm2ts
}
CONFIG += ordered
REQUIRES=full-config #nocrosscompiler
