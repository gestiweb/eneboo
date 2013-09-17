/***********************************************************************************
 * dbf.h
 ***********************************************************************************
 * Author: Bjoern Berg, June 2002
 * Email: clergyman@gmx.de
 * dbf Reader and Converter for dBASE files
 * Version 0.6
 ***********************************************************************************/

#ifndef __DBF_CORE__
#define __DBF_CORE__

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* 
 * special anubisnet and dbf includes 
 */
#include "codepages.h"
#include "iodbf.h"
#include "an_string.h"
#include "statistic.h"
#include "endian.h"
#include "csv.h"
#include "sql.h"

#define FoxBase 0x02			/* Code for FoxBase */
#define FoxBasePlus 0x03		/* Code for FoxBasePlus, same as for dBase III */
#define dBase3 0x03				/* Code for dBase III */
#define dBase3WM 0x83			/* Code for dBase III with memo fields */
#define dBase4 0x04				/* Code for dBase IV */
#define dBase4WM 0x8B			/* Code for dBase IV with memo fields */
#define dBase4SQL 0x8E			/* Code for dBase IV with SQL table */
#define dBase5 0x05				/* Code for dBase 5.0 */
#define FoxPro2WM 0xF5			/* Code for FoxPro 2.0 (or earlier) with memo fields */					
#define VisualFoxPro 0x30		/* Code for Visual FoxPro without memo fields */

/* 
 * V A R I A B L E S 
 */

extern unsigned int verbosity;
extern unsigned int dbversion;

typedef int	(*headerMethod)(FILE *output, const struct DB_FIELD * header,
    int header_length,
    const char *filename, const char *export_filename);

typedef int	(*lineMethod)(FILE *output, const struct DB_FIELD * header,
    const unsigned char *value, int header_length,
    const char *filename, const char *export_filename);

#endif
