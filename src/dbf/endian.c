/********************************************************************************
 * endian.c
 ********************************************************************************
 * Routines for Little Endian and Big Endian Systems
 * Library version
 *
 * Version 0.4, 2003-09-08
 * Author: Björn Berg, clergyman@gmx.de
 *
 * History:
 * 2003-09-08	berg	changes in u_int types, adapted to different system
 *						standards
 * 2003-05-09   jones	changes for AIX in IsBigEndian()
 * 2003-04-18	berg	endian.h splitted to endian.h and endian.c
 *						implemented fix by Uwe Steinmann
 * 2003-02-16	jones	the #ifndef inclusion guard only protected the #defines
 *						where it really needs to protect the whole file. Just
 *						moved the #endif to the end of the file.
 * 2003-02-09	jones	improved IsBigEndian function
 *						changes in rotate4b
 * 2003-02-01	berg	rotate2b / rotate4b added
 * 2002-12-12	berg	first implementation
 *******************************************************************************/

#include "endian.h"

/*******************************************************************
 * Routine to test if System uses Big Endian or Little Endian
 *******************************************************************/
/*_bool IsBigEndian() {
 int i = 1;
 int c = *((char*)&i);
 return c == 0;
}*/
_bool IsBigEndian() {
#if defined(__aix__)
 return _true;
#else
 int i = 1;
 int c = *((char*)&i);
 return c == 0;
#endif
}

/******************************************************************
 * Uwe Steinman reported a Bug related to the Big Endian problem
 * on his Linux PPC system and has sent in a fixed version of
 * rotate2b, because the old one has also shifted the leading sign
 ******************************************************************/
/*int rotate2b(short var) {
	if(IsBigEndian() == _true) {
        short tmp = var;
        var = (tmp >> 8) | ( var << 8);
	}
	return var;
}*/

short rotate2b(short var) {
  typedef union {
    short int i;
    char b[2];
  } int_um;

  if( IsBigEndian() == _true ) {
    int_um *ptr;
    char hilf;
    ptr = (int_um *) &var;
    hilf = ptr->b[0];
    ptr->b[0] = ptr->b[1];
    ptr->b[1] = hilf;
  }
  return var;
}

/*******************************************************************
 * Changes byte construction if dbf is used on another platform
 * than Little Endian. dBASE databases are written in Little Endian
 * format.
 *******************************************************************/
unsigned int
rotate4b(u_int32_t var) {
	u_int32_t old, tmp;
	if(IsBigEndian() == _true) {
		tmp = old = var;
		// change Byte 4 with Byte 1
		tmp>>=24;
		var<<=24;
		var^=tmp;
		// change Byte 3 with Byte 2
		tmp=old;
		tmp<<=8;
		old>>=8;
		tmp &= 0x00FF0000;
		old &= 0x0000FF00;
		tmp |= old;
		var |= tmp;
	}
	return var;
}
