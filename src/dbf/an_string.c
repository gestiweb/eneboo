/***********************************************************************************
 * an_string.c
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

#include <assert.h>
#include "an_string.h"

/* * * * COUNTUMLAUTS
 * we use this function to get the number of umlauts in
 * the string and decide if we have to convert or not */
int
countumlauts (const char *src)
{
	int count;
	count = 0;
	for(; *src; src++) {
		printf("%c\n", *src);
    	switch(*src) {
      	case '\341':
			printf("ß gefunden\n");
        	count++;
        	break;
      	case '\204':
        	count++;
        	break;
      	case '\216':
        	count++;
        	break;
      	case '\224':
			printf("ü gefunden\n");
        	count++;
        	break;
    	}
	}
	return count;

}


/* * * * CONVUML
 * converts german special chars into standard chars */
void
convuml(char *dest, const char *src)
{
	while(*src != '\0') {
    switch(*src) {
      case 'ß':
        *dest++ = 's';
        *dest++ = 's';
        break;
      case 'ä':
        *dest++ = 'a';
        *dest++ = 'e';
        break;
      case 'ö':
        *dest++ = 'o';
        *dest++ = 'e';
        break;
      case 'ü':
        *dest++ = 'u';
        *dest++ = 'e';
        break;
      default:
        *dest++ = *src;
    }
    src++;
  }
  *dest = '\0';
}
