/***************************************************************************
                       ps.cpp  -  description
                           -------------------
  begin                : Mie Jun 18 2003
  copyright            : (C) 2003 by InfoSiAL, S.L.
  email                : mail@infosial.com
***************************************************************************/ 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * ps.c -- printing the "partial" bar encoding
 *
 * Copyright (c) 1999 Alessandro Rubini (rubini@gnu.org)
 * Copyright (c) 1999 Prosa Srl. (prosa@prosa.it)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "barcode.h"

#define SHRINK_AMOUNT 0.15			/* shrink the bars to account for ink spreading */


/*
 * How do the "partial" and "textinfo" strings work?
 *
 * The first char in "partial" tells how much extra space to add to the
 * left of the bars. For EAN-13, it is used to leave space to print the
 * first digit, other codes may have '0' for no-extra-space-needed.
 *
 * The next characters are alternating bars and spaces, as multiples
 * of the base dimension which is 1 unless the code is
 * rescaled. Rescaling is calculated as the ratio from the requested
 * width and the calculated width.  Digits represent bar/space
 * dimensions. Lower-case letters represent those bars that should
 * extend lower than the others: 'a' is equivalent to '1', 'b' is '2' and
 * so on.
 *
 * The "textinfo" string is made up of fields "%lf:%lf:%c" separated by
 * blank space. The first integer is the x position of the character,
 * the second is the font size (before rescaling) and the char item is
 * the charcter to be printed.
 *
 * Both the "partial" and "textinfo" strings may include "-" or "+" as
 * special characters (in "textinfo" the char should be a standalone
 * word).  They state where the text should be printed: below the bars
 * ("-", default) or above the bars. This is used, for example, to
 * print the add-5 and add-2 codes to the right of UPC or EAN codes
 * (the add-5 extension is mostly used in ISBN codes.
 */


int Barcode_ps_print( struct Barcode_Item *bc, QString * f ) {
	int i, j, k, barlen, printable = 1;
	double f1, f2, fsav = 0;
	int mode = '-';							/* text below bars */
	double scalef = 1, xpos, x0, y0, yr;
	char *ptr;
	char c;

	if ( !bc->partial || !bc->textinfo ) {
		bc->error = EINVAL;
		return -1;
	}


	/*
	 * Maybe this first part can be made common to several printing back-ends,
	 * we'll see how that works when other ouput engines are added
	 */

	/* First, calculate barlen */
	barlen = bc->partial[ 0 ] - '0';
	for ( ptr = bc->partial + 1; *ptr; ptr++ )
		if ( isdigit( *ptr ) )
			barlen += ( *ptr - '0' );
		else if ( islower( *ptr ) )
			barlen += ( *ptr - 'a' + 1 );

	/* The scale factor depends on bar length */
	if ( !bc->scalef ) {
		if ( !bc->width )
			bc->width = barlen;	/* default */
		scalef = bc->scalef = ( double ) bc->width / ( double ) barlen;
	}

	/* The width defaults to "just enough" */
	if ( !bc->width )
		bc->width = barlen * scalef + 1;

	/* But it can be too small, in this case enlarge and center the area */
	if ( bc->width < barlen * scalef ) {
		int wid = barlen * scalef + 1;
		bc->xoff -= ( wid - bc->width ) / 2;
		bc->width = wid;
		/* Can't extend too far on the left */
		if ( bc->xoff < 0 ) {
			bc->width += -bc->xoff;
			bc->xoff = 0;
		}
	}

	/* The height defaults to 80 points (rescaled) */
	if ( !bc->height )
		bc->height = 80 * scalef;

#if 0
	/* If too small (5 + text), enlarge and center */
	i = 5 + 10 * ( ( bc->flags & BARCODE_NO_ASCII ) == 0 );
	if ( bc->height < i * scalef ) {
		int hei = i * scalef;
		bc->yoff -= ( hei - bc->height ) / 2;
		bc->height = hei;
		if ( bc->yoff < 0 ) {
			bc->height += -bc->yoff;
			bc->yoff = 0;
		}
	}
#else
	/* If too small (5 + text), reduce the scale factor and center */
	i = 5 + 10 * ( ( bc->flags & BARCODE_NO_ASCII ) == 0 );
	if ( bc->height < i * scalef ) {
		double scaleg = ( ( double ) bc->height ) / i;
		int wid = bc->width * scaleg / scalef;
		bc->xoff += ( bc->width - wid ) / 2;
		bc->width = wid;
		scalef = scaleg;
	}
#endif

	/*
	 * Ok, then deal with actual ps (eps) output
	 */

	if ( !( bc->flags & BARCODE_OUT_NOHEADERS ) ) {	/* spit a header first */
		if ( bc->flags & BARCODE_OUT_EPS )
			( *f ) += "%%!PS-Adobe-2.0 EPSF-1.2\n";
		else
			( *f ) += "%%!PS-Adobe-2.0\n";
		( *f ) += "%%%%Creator: libbarcode\n";
		if ( bc->flags & BARCODE_OUT_EPS ) {
			( *f ) += QString( "%%%%BoundingBox: %1 %2 %3 %4\n" )
			          .arg( bc->xoff )
			          .arg( bc->yoff )
			          .arg( bc->xoff + bc->width + 2 * bc->margin )
			          .arg( bc->yoff + bc->height + 2 * bc->margin );
		}
		( *f ) += "%%%%EndComments\n";
		if ( bc->flags & BARCODE_OUT_PS ) {
			( *f ) += "%%%%EndProlog\n\n";
			( *f ) += "%%%%Page: 1 1\n\n";
		}
	}

	/* Print some informative comments */
	for ( i = 0; bc->ascii[ i ]; i++ )
		if ( bc->ascii[ i ] < ' ' )
			printable = 0;

	( *f ) += QString( "%% Printing barcode for \"%1\", scaled %2" )
	          .arg( printable ? bc->ascii : "<unprintable string>" )
	          .arg( scalef, 2, 'f', 2 );
	if ( bc->encoding )
		( *f ) += QString( ", encoded using \"%1\"" ).arg( bc->encoding );
	( *f ) += "\n";
	( *f ) += "%% The space/bar succession is represented "
	          "by the following widths (space first):\n" "%% ";
	for ( i = 0; i < strlen( bc->partial ); i++ ) {
		char c = bc->partial[ i ];
		if ( isdigit( c ) )
			( *f ) += QChar( c );
		if ( islower( c ) )
			( *f ) += QChar( c - 'a' + '1' );
		if ( isupper( c ) )
			( *f ) += QChar( c - 'A' + '1' );
	}
	/* open array for "forall" */
	( *f ) += "\n[\n%%  height  xpos   ypos  width"
	          "       height  xpos   ypos  width\n";

	xpos = bc->margin + ( bc->partial[ 0 ] - '0' ) * scalef;
	for ( ptr = bc->partial + 1, i = 1; *ptr; ptr++, i++ ) {
		/* special cases: '+' and '-' */
		if ( *ptr == '+' || *ptr == '-' ) {
			mode = *ptr;				/* don't count it */
			i++;
			continue;
		}
		/* j is the width of this bar/space */
		if ( isdigit( *ptr ) )
			j = *ptr - '0';
		else
			j = *ptr - 'a' + 1;
		if ( i % 2 ) {						/* bar */
			x0 = bc->xoff + xpos + ( j * scalef ) / 2;
			y0 = bc->yoff + bc->margin;
			yr = bc->height;
			if ( !( bc->flags & BARCODE_NO_ASCII ) ) {	/* leave space for text */
				if ( mode == '-' ) {
					/* text below bars: 10 points or five points */
					y0 += ( isdigit( *ptr ) ? 10 : 5 ) * scalef;
					yr -= ( isdigit( *ptr ) ? 10 : 5 ) * scalef;
				} else {				/* '+' */
					/* text above bars: 10 or 0 from bottom, and 10 from top */
					y0 += ( isdigit( *ptr ) ? 10 : 0 ) * scalef;
					yr -= ( isdigit( *ptr ) ? 20 : 10 ) * scalef;
				}
			}
			/* Define an array and then use "forall" (Hans Schou) */
			( *f ) += QString( "   [%1 %2 %3 %4]%5" )
			          .arg( yr, 5, 'f', 2 )
			          .arg( x0, 6, 'f', 2 )
			          .arg( y0, 6, 'f', 2 )
			          .arg( ( j * scalef ) - SHRINK_AMOUNT, 5, 'f', 2 )
			          .arg( i % 4 == 1 ? "   " : "\n" );
		}
		xpos += j * scalef;
	}
	( *f ) += "\n]\t{ {} forall setlinewidth moveto 0 exch rlineto stroke} "
	          "bind forall\n";

	/* Then, the text */

	mode = '-';									/* reinstantiate default */
	if ( !( bc->flags & BARCODE_NO_ASCII ) ) {
		( *f ) += "[\n%%   char    xpos   ypos fontsize\n";
		k = 0;									/* k is the "previous font size" */
		for ( ptr = bc->textinfo; ptr; ptr = strchr( ptr, ' ' ) ) {
			while ( *ptr == ' ' )
				ptr++;
			if ( !*ptr )
				break;
			if ( *ptr == '+' || *ptr == '-' ) {
				mode = *ptr;
				continue;
			}
			if ( sscanf( ptr, "%lf:%lf:%c", &f1, &f2, &c ) != 3 ) {
				fprintf( stderr, "barcode: impossible data: %s\n", ptr );
				continue;
			}

			( *f ) += "    [(";
			/* Both the backslash and the two parens are special */
			if ( c == '\\' || c == ')' || c == '(' )
				( *f ) += QString( "\\%1) " ).arg( QChar( c ) );
			else
				( *f ) += QString( "%1)  " ).arg( QChar( c ) );
			( *f ) += QString( "%1 %2 %3]\n" )
			          .arg( bc->xoff + f1 * scalef + bc->margin, 6, 'f', 2 )
			          .arg( mode ==
			                '-' ? ( double ) bc->yoff +
			                bc->margin : ( double ) bc->yoff + bc->margin +
			                bc->height - 8 * scalef, 6, 'f', 2 )
			          .arg( fsav == f2 ? 0.0 : f2 * scalef, 5, 'f', 2 );
			fsav = f2;
		}
		( *f ) += "]   { {} forall dup 0.00 ne {\n\t"
		          "/Helvetica findfont exch scalefont setfont\n"
		          "    } {pop} ifelse\n" "    moveto show} bind forall\n";


	}

	( *f ) += QString( "%% End barcode for \"%1\"\n\n" )
	          .arg( printable ? bc->ascii : "<unprintable string>" );

	if ( !( bc->flags & BARCODE_OUT_NOHEADERS ) ) {
		if ( bc->flags & BARCODE_OUT_PS ) {
			( *f ) += "showpage\n";
			( *f ) += "%%%%Trailer\n\n";
		}
	}
	return 0;
}
