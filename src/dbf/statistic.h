/***********************************************************************************
 * statistic.h
 * inherits the statistic functions for dBASE files
 * Author: Bjoern Berg, September 2002
 * Email: clergyman@gmx.de
 * dbf Reader and Converter for dBase III, IV, 5.0 
 *
 * see statistic.c for history and details
 ************************************************************************************/

#ifndef _DBF_STATS_
#define _DBF_STATS_
#include "tables.h"
#include "congraph.h"
#include "endian.h"

char *get_db_version (int version);
void dbf_file_info (const struct DB_HEADER *db);
void dbf_field_stat (const struct DB_FIELD *header, int header_length);

#endif
