/********************************************************************************
 * endian.h
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
 * 2003-04-18	berg	endian.h splitted to endian.h and endian.c
 *
 * 2003-02-16	jones	the #ifndef inclusion guard only protected the #defines
 *						where it really needs to protect the whole file. Just
 *						moved the #endif to the end of the file.
 * 2003-02-09	jones	improved IsBigEndian function
 *						changes in rotate4b
 * 2003-02-01	berg	rotate2b / rotate4b added
 * 2002-12-12	berg	first implementation
 *******************************************************************************/

#ifndef _ANUBISNET_ENDIAN_
#define _ANUBISNET_ENDIAN_

/*
 * I N C L U D E S
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifdef __unix__   
   #include <sys/types.h>
   #ifndef __ANUBISNET_TYPES__
   #define __ANUBISNET_TYPES__
     typedef u_int16_t uint16_t;
     typedef u_int32_t uint32_t;
   #endif
/*
 * Windows does not know UINT16 types, therefore we have to make an improvement
 * for 32 Bit systems. unsigned short is only verified to work properly on 32 Bit
 * systems.
 */
#elif _WIN32
	#include <windows.h>
    #ifndef __ANUBISNET_TYPES__
    #define __ANUBISNET_TYPES__
      typedef UINT32 u_int32_t; 
      typedef unsigned short u_int16_t;
    #endif
#else
   #include <sys/types.h>
#endif

/*
 * D E F I N I T I O N S
 */
typedef int _bool;
#define _true 0x01
#define _false 0x00

/* 
 * F U N C T I O N S 
 */
_bool IsBigEndian ();
short rotate2b ( short var);
unsigned int rotate4b ( unsigned int var );

#endif
