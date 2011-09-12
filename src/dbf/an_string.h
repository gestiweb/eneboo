/***********************************************************************************
 * an_string.h
 ***********************************************************************************
 * anubisnet string work-on library, advanced commands to manipulate strings
 * Author: Bjoern Berg, June 2002
 * Email: clergyman@gmx.de
 *
 * Implemented for: dbf Reader and Converter for dBase 3
 * Version 0.2
 *
 * History:
 * - Version 0.2 - April 2003
 * - Version 0.1.1 - September 2002
 *   pasted into extra header file to use it in other programmes
 * - Version 0.1 - June 2002
 *	 first implementation in dbf.c
 ************************************************************************************/

#ifndef _ANUBISNET_STRING_
#define _ANUBISNET_STRING_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char * trim_spaces (unsigned char *dest, const unsigned char *src, size_t srclen);
int countumlauts (const char *src);
void convuml (char *dest, const char *src);

#endif

