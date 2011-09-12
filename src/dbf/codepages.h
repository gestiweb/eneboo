/**********************************************************************
 *	CODEPAGES.H
 * 	Header file with codepage definitions for dbf
 *  using ISO-8559-1 definitions (Latin 1) to encode
 *  Author: Bjoern Berg, September 2002
 *  Email: clergyman@gmx.de
 *  dbf Reader and converter for dBase
 *  Version 0.3
 *
 *  History:
 *  - Version 0.3 - 2003-04-20
 *	  splitted to codepages.h and codepages.c
 *  - Version 0.2 - 2003-01-30
 *	  included patch by Christian Vogel:
 *	  changes all occurences of "char" to "unsigned char"
 *    This avoids many warnings about "case statement out of range"
 *  - Version 0.1 - 14.09.2002
 *	  first implementation, using iso-definitions
 ********************************************************************/

#ifndef _DBF_CODEPAGES_
#define _DBF_CODEPAGES_

#include <string.h>

void cp850andASCIIconvert(unsigned char *src);

#endif
