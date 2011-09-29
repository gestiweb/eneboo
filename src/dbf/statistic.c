/***********************************************************************************
 * statistic.c
 * inherits the statistic functions for dBASE files
 * Author: Bjoern Berg, September 2002
 * Email: clergyman@gmx.de
 * dbf Reader and Converter for dBase III, IV, 5.0
 *
 *
 * History:
 * 2003-11-05	berg	get_db_version() and verbosity checks for header values in
 *						dbf_file_info
 ************************************************************************************/

#include "statistic.h"
#include "dbf.h"


char *get_db_version (int version) {
	char *name;
	
	switch (version) {
		case 0x02:
			// without memo fields
			name = "FoxBase";
			break;
		case 0x03:
			// without memo fields
			name = "FoxBase+/dBASE III+";
			break;
		case 0x04:
			// without memo fields
			name = "dBASE IV";
			break;	
		case 0x05:
			// without memo fields
			name = "dBASE 5.0";
			break;	
		case 0x83:
			name = "FoxBase+/dBASE III+";
			break;		
		case 0x8B:
			name = "dBASE IV";
			break;			
		case 0x30:
			// without memo fields
			name = "Visual FoxPro";
			break;	
		case 0xF5:
			// with memo fields
			name = "FoxPro 2.0";
			break;		
	}
	
	return name;				
}


/* output for header statistic */
void
dbf_file_info (const struct DB_HEADER *db)
{
	int version, memo;

	version	= db->version;
	memo = (db->version  & 128)==128 ? 1 : 0;	
	printf("\n-- File statistics\n");
	printf("dBase version.........: \t %s (%s)\n",
			get_db_version(version), memo?"with memo":"without memo");
	printf("Date of last update...: \t %d-%02d-%02d\n", 
			1900 + db->last_update[0], db->last_update[1], db->last_update[2]);
	printf("Number of records.....: \t %d (%08xd)\n", 
			rotate4b(db->records), rotate4b(db->records));
	printf("Length of header......: \t %d (%04xd)\n", 
			rotate2b(db->header_length), rotate2b(db->header_length));
	printf("Record length.........: \t %d (%04xd)\n", 
			rotate2b(db->record_length), rotate2b(db->record_length));
	printf("Columns in file.......: \t %d \n", 
			(rotate2b(db->header_length)/32)-1);
	printf("Rows in file..........: \t %d\n\n", 
			rotate4b(db->records));
}

/* output for field statistic */
#define linelength	73

void
dbf_field_stat (const struct DB_FIELD *header, int header_length)
{
	const struct DB_FIELD *dbf;
	int cross[] = {1,17,25,41,57,73};

	drawline(linelength, cross, (sizeof(cross)/sizeof(int)));
	printf("| field name\t| type\t| field adress\t| length\t| field dec.\t|\n");
	drawline(linelength, cross, sizeof(cross)/sizeof(int));
	for (dbf = header + 1; --header_length; dbf++)
		printf("|%13.11s\t| %3c\t| %8x\t| %3d\t\t| %3d\t\t|\n",
			   dbf->field_name, dbf->field_type, rotate4b(dbf->field_adress), dbf->field_length, dbf->field_decimals);
	drawline(linelength, cross, sizeof(cross)/sizeof(int));
}
