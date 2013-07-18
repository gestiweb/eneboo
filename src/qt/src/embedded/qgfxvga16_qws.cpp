/****************************************************************************
**
** Implementation of QGfxVga16 (graphics context) class for VGA cards
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free Qt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "qapplication.h"

#ifndef QT_NO_QWS_VGA16

#include "qregexp.h"
#include "qpen.h"
#include "qpaintdevicemetrics.h"
#include "qgfxraster_qws.h"
#include "qgfxvga16_qws.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <math.h>

#ifndef QT_NO_QWS_MULTIPROCESS
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#if !defined(Q_OS_MACX) && !defined(Q_OS_FREEBSD) && !defined (QT_NO_QWS_LINUXFB)
#include <linux/fb.h>
#endif


// Profiling stuff
// Turn it off
// #include "qprofile_qws.h"
#define BEGIN_PROFILING
#define PROFILE_BLOCK(x)


static unsigned char *screen_double_buffer;
static unsigned char *vga_register_values;
static unsigned char *fb_line_ptrs[480]; //### probably want these in shared memory
static unsigned char *db_line_ptrs[480];
// ### Might want to put in shared memory
static unsigned char closestMatchLUT[16][16][16]; // this takes 4096 bytes

static bool closestMatchLUTinitialised = FALSE;
static bool set_back_buffer = TRUE;
static bool force_set_pixel = FALSE;


#ifndef QT_NO_QWS_CURSOR

class QVga16Cursor : public QScreenCursor
{
public:
    QVga16Cursor();
    virtual void set( const QImage &image, int hotx, int hoty );
    virtual void move( int x, int y );
    void drawCursor(QRect &r);
    virtual bool supportsAlphaCursor() { return FALSE; }
    static bool enabled() { return TRUE; }
private:
    void restoreUnder();
};


#define SCREEN_DOUBLE_BUFFER_SIZE   (640*480/2)
#define VGA_REGISTER_VALUES_SIZE    (512)
#define FB_LINE_PTS_SIZE	    (480*4)
#define DB_LINE_PTS_SIZE	    (480*4)
#define CLOSEST_MATCH_LUT_SIZE	    (16*16*16)
#define TOTAL_SHARED_MEMORY_SIZE    (SCREEN_DOUBLE_BUFFER_SIZE + \
				     VGA_REGISTER_VALUES_SIZE + \
				     FB_LINE_PTRS_SIZE + \
				     DB_LINE_PTRS_SIZE + \
				     CLOSEST_MATCH_LUT_SIZE)

/*

// This is will be added later to clean up shared memory/ malloc stuff

class Vga16ScreenGlobals
{
    public:
	Vga16ScreenGlobals()
	{
	    screen_double_buffer = NULL;
	    vga_register_values = NULL;
	    fb_line_ptrs = NULL;
	    db_line_ptrs = NULL;

	}

	~Vga16ScreenGlobals()
	{
	}

    public:
	unsigned char *screen_double_buffer;
	unsigned char *vga_register_values;
	unsigned char *fb_line_ptrs[480];
	unsigned char *db_line_ptrs[480];
	unsigned char closestMatchLUT[16][16][16]; // this takes 4096 bytes
};


CVga16ScreenGlobals  *gVga16ScreenGlobals;

*/


static QVga16Cursor *qt_vga16cursor = NULL;


inline QVga16Cursor::QVga16Cursor()
{
    qt_vga16cursor = this;
}

#endif // QT_NO_QWS_CURSOR


#ifndef QT_NO_QWS_CURSOR

static QRect	Vga16DrawArea;

# define VGA16_GFX_START(r)	if(is_screen_gfx && qt_sw_cursor) \
				    Vga16DrawArea = r, beginDraw();
# define VGA16_GFX_END		if(is_screen_gfx && qt_sw_cursor) { \
				    if (qt_vga16cursor) \
	    				qt_vga16cursor->drawCursor(Vga16DrawArea); \
				    endDraw(); \
				}

#else //QT_NO_QWS_CURSOR

# define VGA16_GFX_START(r)	if(is_screen_gfx) \
				    beginDraw();
# define VGA16_GFX_END		if(is_screen_gfx) \
				    endDraw();

#endif //QT_NO_QWS_CURSOR



static inline unsigned int closestMatch(int r, int g, int b)
{
    QRgb *clut = qt_screen->clut();
    int clutcols = qt_screen->numCols();

    if ( r>255 || g>255 || b>255 || r<0 || g<0 || b<0 )
	abort();

    unsigned int distance, closest_distance = ~0x00;
    int pos = 0;

    for (int loopc = 0; loopc < clutcols; loopc++)
    {
	QRgb a = clut[loopc];
	int dr = qRed(a)   - r;
	int dg = qGreen(a) - g;
	int db = qBlue(a)  - b;
	distance = dr*dr + dg*dg + db*db;
	if (distance < closest_distance) {
	    closest_distance = distance;
	    pos = loopc;
	}
    }

    return pos;
}


static void initialiseColorLookupTable(void)
{
    for (int _r = 0; _r < 256; _r += 16)
	for (int _g = 0; _g < 256; _g += 16)
	    for (int _b = 0; _b < 256; _b += 16)
		closestMatchLUT[_r >> 4][_g >> 4][_b >> 4] = closestMatch(_r, _g, _b);
    closestMatchLUTinitialised = TRUE;
}


// extern unsigned int closestMatchUsingTable(int r,int g,int b);


static unsigned int closestMatchUsingTable(int r,int g,int b)
{
    if (closestMatchLUTinitialised == FALSE) {
	initialiseColorLookupTable();
    }
    return closestMatchLUT[(r >> 4) & 0x0F][(g >> 4) & 0x0F][(b >> 4) & 0x0F];
}


class VGA16_CriticalSection    // Mutex for serialising VGA16 access
{
    public:

	VGA16_CriticalSection()
	{
#ifndef QT_NO_QWS_MULTIPROCESS
	    QWSDisplay::grab(TRUE);
#endif
	}

	~VGA16_CriticalSection()
	{
#ifndef QT_NO_QWS_MULTIPROCESS
	    QWSDisplay::ungrab();
#endif
	}
};


// This is better than branching as the CPU pipeline can
// probably just do both shifts simultaneously and combine the result
// in the same time it takes it to just work out if y is negative or not
#define MyFastShift(x, y) \
    (((x) << (y)) | ((x) >> -(y)))


//
// The idea of this macro is to generically make functions that will
// output to the VGA ports only if the cached value of what the port
// is set to is different.
//
#define MAKE_VGA16_FUNC(funcname, reg, port) \
    static inline void set_##funcname##_vga16(unsigned char value, bool force = TRUE) \
    { \
	if ((1) || (force) || (vga_register_values[((port - 0x3C0) * 16) + reg] != value)) { \
	outw((value << 8) | reg, port); \
	vga_register_values[((port - 0x3C0) * 16) + reg] = value; \
    } \
}


// make some VGA16 helper functions
MAKE_VGA16_FUNC(write_planes,	0x02, 0x3C4)
MAKE_VGA16_FUNC(memory_mode,	0x04, 0x3C4)
MAKE_VGA16_FUNC(set_reset,	0x00, 0x3CE)
MAKE_VGA16_FUNC(enable_set_reset, 0x01, 0x3CE)
MAKE_VGA16_FUNC(rotate,		0x03, 0x3CE)
MAKE_VGA16_FUNC(read_map,	0x04, 0x3CE)
MAKE_VGA16_FUNC(mode,		0x05, 0x3CE)
MAKE_VGA16_FUNC(bit_mask,	0x08, 0x3CE)






// private QPolygonScanner
class QGfxVga16 : public QGfxRasterBase , private QPolygonScanner
{

    public:

	QGfxVga16(unsigned char *,int w,int h);
	virtual ~QGfxVga16();

        void useBrush() {
            pixel=gfx_screen->alloc(cbrush.color().red(),
                                    cbrush.color().green(),
                                    cbrush.color().blue());
        }
        void usePen() {
            pixel=gfx_screen->alloc(cpen.color().red(),
                                    cpen.color().green(),
                                    cpen.color().blue());
        }

	virtual void drawPoint( int,int );
	virtual void drawPoints( const QPointArray &,int,int );
	virtual void drawLine( int,int,int,int );
	// virtual void drawRect( int,int,int,int );
	virtual void fillRect( int,int,int,int );
	virtual void drawPolyline( const QPointArray &,int,int );
	virtual void drawPolygon( const QPointArray &,bool,int,int );
	virtual void blt( int,int,int,int,int,int );
	virtual void scroll( int,int,int,int,int,int );
#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS)
	virtual void stretchBlt( int,int,int,int,int,int );
#endif
	virtual void tiledBlt( int,int,int,int );
	virtual int bitDepth() { return 4; }
	virtual void setSource(const QImage *);
	virtual void setSource(const QPaintDevice *);


	unsigned char getPixel( int,int );
	virtual void  setPixel( int,int,unsigned char );

    protected:

	virtual void drawThickLine( int,int,int,int );

    private:


	void drawrect_4(unsigned int x1, unsigned int y1,
    	    unsigned int x2, unsigned int y2, unsigned char col);
	unsigned int get_pixel_4_32(unsigned int x, unsigned int y);
	void himageline_4(unsigned int y, unsigned int x1, unsigned int x2, unsigned char *srcdata, bool reverse = FALSE);

	GFX_INLINE unsigned int get_value_32(int sdepth, unsigned char **srcdata, bool reverse = FALSE);
	GFX_INLINE void calcPacking(void * m,int x1,int x2, int & frontadd,int & backadd,int & count);
	GFX_INLINE void hline( int x1,int x2,int y);
	GFX_INLINE void hAlphaLineUnclipped( int x1,int x2,unsigned int y,unsigned char *srcdata,unsigned char *alphas);
	void processSpans( int n, QPoint* point, int* width );
//	void paintCursor(const QImage& image, int hotx, int hoty, QPoint cursorPos);
	void buildSourceClut(QRgb * cols,int numcols);

};


QGfxVga16::QGfxVga16(unsigned char * a,int b,int c) : QGfxRasterBase(a,b,c)
{
    BEGIN_PROFILING

//    setLineStep((depth*width+7)/8);
    Q_UNUSED(a);
    Q_UNUSED(b);
    Q_UNUSED(c);
}


QGfxVga16::~QGfxVga16()
{
    BEGIN_PROFILING
}


void QGfxVga16::setPixel( int x, int y, unsigned char col )
{
    BEGIN_PROFILING

    // don't bother setting the pixel if it's already the right colour
    if ((!force_set_pixel) && (getPixel(x, y) == col)) {
   	return;
    }

    // this gives a value of 4 or 0 if x is odd or even. Handy for shifting bits accordingly
    col &= 0x0F;

    if (set_back_buffer) {
	unsigned char shift = (x & 1) << 2;
	unsigned char *db_ptr = db_line_ptrs[y] + (x >> 1);
	*db_ptr = (*db_ptr & (0xF0 >> shift)) | (col << shift);
    }

    VGA16_CriticalSection cs;

    unsigned char *fb_ptr = fb_line_ptrs[y] + (x >> 3);
    set_enable_set_reset_vga16(0x0F, 0);
    set_rotate_vga16(0x00, 0);
    set_mode_vga16(0x00, 0);
    set_write_planes_vga16(0x0F, 0);
    set_set_reset_vga16(col, 0);
    set_bit_mask_vga16(0x80 >> (x & 0x07), 0);
    *fb_ptr |= 0xFE;
}


unsigned char QGfxVga16::getPixel( int x, int y )
{
    BEGIN_PROFILING
    return (*(db_line_ptrs[y] + (x >> 1)) >> ((x & 1) << 2)) & 0x0F;
}


unsigned int QGfxVga16::get_pixel_4_32(unsigned int x, unsigned int y)
{
    BEGIN_PROFILING
    unsigned char val = getPixel(x, y);
    return clut[val];
}


void vga16_exposeDoubleBufferedRegion(int x1, int y1, int x2, int y2)
{
    BEGIN_PROFILING

    // Doing large areas a plane at a time looks ugly because
    // you may see breifly the screen while it is partially red or
    // not all planes filled in if this operation doesn't complete
    // with in a retrace. Waiting for vertical retrace would really
    // slow down Qt so the solution is to do it in sections that
    // won't make this effect obvious, hence the following recursive
    // calls below.

    if ((y2 - y1) > 15) {
	unsigned int height = y2 - y1 + 1;
	unsigned int mid_line = y1 + (height / 2);
	vga16_exposeDoubleBufferedRegion(x1,y1,x2,mid_line);
	vga16_exposeDoubleBufferedRegion(x1,mid_line+1,x2,y2);
	return;
    }

    y1 = (y1 > 479) ? 479 : (y1 < 0) ? 0 : y1;
    y2 = (y2 > 479) ? 479 : (y2 < 0) ? 0 : y2;
    x1 = (x1 > 639) ? 639 : (x1 < 0) ? 0 : x1;
    x2 = (x2 > 639) ? 639 : (x2 < 0) ? 0 : x2;

    int dst_y = y1, h = y2 - y1 + 1;
    int new_x1 = x1 & (~0x07);
    int new_x2 = (x2 & (~0x07)) + 7;

    // alignment thingo
    new_x1 = new_x1 & ~31;

    new_x1 = (new_x1 <   0) ?   0 : new_x1;
    new_x2 = (new_x2 > 639) ? 639 : new_x2;

    if (new_x1 <= new_x2)
    {
	VGA16_CriticalSection cs;

	set_enable_set_reset_vga16(0x00, 0); // disable it
	set_rotate_vga16(0x00, 0);
	set_mode_vga16(0x00, 0);
	set_bit_mask_vga16(0xFF, 0);

	for (unsigned int plane = 0; plane < 4; plane++ ) {

	    set_write_planes_vga16(1 << plane, 0);

	    // Inlining this for each plane might help performance heaps
	    for (int j = 0; j < h; j++) {

		unsigned char *dst_ptr1 = fb_line_ptrs[dst_y + j] + (new_x1 >> 3);
		unsigned int *dst_ptr = (unsigned int *)dst_ptr1;
		unsigned int *src_row = (unsigned int *)(db_line_ptrs[dst_y + j] + (new_x1 >> 1));
		unsigned int src_pixels;

		for (int x = new_x1; x <= new_x2; x += 32) {

		    // Optimised to 71 operations to do 32 pixels
		    // This code doesn't look very readable though
		    // It just moves bits around rearranging them
		    // as required, going from pixels in 4 bit packed
		    // mode to pixels in planar mode (4 1-bit planes).

		    unsigned int src_dword4 = src_row[0] >> plane;
		    unsigned int src_dword3 = src_row[1] >> plane;
		    unsigned int src_dword2 = src_row[2] >> plane;
		    unsigned int src_dword1 = src_row[3] >> plane;

		    unsigned int tmp_dword1 = ((src_dword1) << 16)
					    | ((src_dword3 & 0x00001111));
		    unsigned int tmp_dword2 = ((src_dword2) << 16)
					    | ((src_dword4 & 0x00001111));

		    unsigned int tmp_dword3 = ((src_dword1 & 0x11110000))
					    | ((src_dword3) >> 16);
		    unsigned int tmp_dword4 = ((src_dword2 & 0x11110000))
					    | ((src_dword4) >> 16);

		    unsigned int tmp_dword5 = ((tmp_dword1 & 0x00110011) << 8)
					    | ((tmp_dword2 & 0x00110011));
		    unsigned int tmp_dword6 = ((tmp_dword1 & 0x11001100))
					    | ((tmp_dword2 & 0x11001100) >> 8);

		    unsigned int tmp_dword7 = ((tmp_dword3 & 0x00110011) << 8)
					    | ((tmp_dword4 & 0x00110011));
		    unsigned int tmp_dword8 = ((tmp_dword3 & 0x11001100))
					    | ((tmp_dword4 & 0x11001100) >> 8);

		    unsigned int tmp_dword9  = ((tmp_dword5 & 0x01010101) << 4)
					     | ((tmp_dword7 & 0x01010101));
		    unsigned int tmp_dword10 = ((tmp_dword5 & 0x10101010))
					     | ((tmp_dword7 & 0x10101010) >> 4);

		    unsigned int tmp_dword11 = ((tmp_dword6 & 0x01010101) << 4)
					     | ((tmp_dword8 & 0x01010101));
		    unsigned int tmp_dword12 = ((tmp_dword6 & 0x10101010))
					     | ((tmp_dword8 & 0x10101010) >> 4);

		    src_pixels  = (tmp_dword9  << 3)
			       	| (tmp_dword10 << 2)
			        | (tmp_dword11 << 1)
				| (tmp_dword12 << 0);

		    src_row += 4;
		    *dst_ptr++ = src_pixels;
		}
	    }
	}
    }
}


static void vga16_bltScrToScr(int dst_x, int dst_y, int src_x, int src_y, int w, int h)
{
    BEGIN_PROFILING

    // Plan of attack for this special case -
    // We copy the pixels from the double buffer in src to the dst
    // we then copy the double buffer to the screen at the 8 pixel
    // boundraies on either side of the changed region to the screen
    // (PS some mouse boundry checks may need to take this into account)
    // This approach is better than others in that the 8 pixel
    // boundry problem has to be handled some how and this fixes it in
    // the most economical way for this case
    if (!w || !h) return;

    src_y = (src_y > 479) ? 479 : (src_y < 0) ? 0 : src_y;
    h = (src_y + h > 480) ? 480 - src_y : h;
    src_x = (src_x > 639) ? 639 : (src_x < 0) ? 0 : src_x;
    w = (src_x + w > 640) ? 640 - src_x : w;

    int i_1st = 0, i_inc = 1;
    int j_1st = 0, j_inc = 320;

    // Have to handle overlap situations
    if (src_y < dst_y)
	j_1st = h - 1, j_inc = -320;
    else if ((src_y == dst_y) && (src_x < dst_x))
	i_1st = w - 1, i_inc = -1;

    // there are two cases to handle, the first is where the src and dst
    // both are aligned in the same nibbles so a memmove can be used,
    // the other case is where shifts will be needed, probably copying
    // 32bits and shifting that 4bits might be an efficent way to go
    // Some experimentation may be needed with how the double_buffer stores
    // the pixels to find the best approach
    unsigned char *src_line_ptr = db_line_ptrs[j_1st+src_y];
    unsigned char *dst_line_ptr = db_line_ptrs[j_1st+dst_y];

    if (i_inc == 1) {

	for (int jcx = 0; jcx < h; jcx++) {

	    int i = 0;
	    unsigned char shift1 = (src_x & 1) << 2;
	    unsigned char shift2 = (dst_x & 1) << 2;

	    // we start by aligning the src data to the 1st byte
	    if (shift1) {
		unsigned char src_pixel = *(src_line_ptr+((src_x+i)>>1));
		unsigned char dst_pixel = *(dst_line_ptr+((dst_x+i)>>1));
		src_pixel = (src_pixel >> 4) & 0x0F;
		src_pixel <<= shift2;
		dst_pixel &= (0xF0 >> shift2);
		*(dst_line_ptr+((dst_x+i)>>1)) = src_pixel | dst_pixel;
		shift1 = 4 - shift1;
		shift2 = 4 - shift2;
		i++;
	    }

	    // we know shift1 will be now 0, therefore if shift2 is 0 too
	    // then the src and dst are aligned nicely such that we can
	    // do a memcpy. Else if shift2 is 4 then we have to fix
	    // up the pixels to make them aligned before we copy them.
	    if (shift2) {

		// this is the slow case where we can't just memcpy

		unsigned char *src_ptr = src_line_ptr + ((src_x + i) >> 1);
		unsigned char *dst_ptr = dst_line_ptr + ((dst_x + i) >> 1);

		unsigned int *idst_ptr = (unsigned int *)dst_ptr;
		unsigned int *isrc_ptr = (unsigned int *)src_ptr;
		unsigned char last_src_pixel = *dst_ptr & 0x0F;

		// loop to do 8 pixels at a time
		for (; i < w - 9; i += 8) {
		    // this is nicely optimised to read in a dword,
		    // then it aligns 7 of the pixels to the destination
		    // dword, and adds in the 8th pixel from the last
		    // iteration, and saves the 8th pixel for the next
		    // iteration.
		    unsigned int src_pixel = *isrc_ptr++;
		    *idst_ptr++ = (src_pixel << 4) | last_src_pixel;
		    last_src_pixel = src_pixel >> 28;
		}

		src_ptr = src_line_ptr + ((src_x + i) >> 1);
		dst_ptr = dst_line_ptr + ((dst_x + i) >> 1);

		// finishing off loop that does 2 pixels at a time
		for (; i < w - 1; i += 2) {
		    unsigned char src_pixel = *src_ptr++;
		    *dst_ptr++ = (src_pixel << 4) | last_src_pixel;
		    last_src_pixel = src_pixel >> 4;
		}

		// write last pixels out
		if (i < w) {
    		    *dst_ptr = (*src_ptr << 4) | last_src_pixel;
		} else {
    		    *dst_ptr = (*dst_ptr & 0xF0) | last_src_pixel;
		}

	    } else {

		// Fast case where we quickly move the pixels around with a memcpy

		unsigned char *src_ptr = src_line_ptr + ((src_x + i) >> 1);
		unsigned char *dst_ptr = dst_line_ptr + ((dst_x + i) >> 1);
		unsigned int memcpy_width = (w - i) >> 1;

		//for (; i < w - 1; i += 2)
		//    *dst_ptr++ = *src_ptr++;

		memcpy(dst_ptr, src_ptr, memcpy_width);

		if ((i += (memcpy_width << 1)) < w) {
		    dst_ptr += memcpy_width;
		    src_ptr += memcpy_width;
		    *dst_ptr = (*src_ptr & 0x0F) | (*dst_ptr & 0xF0);
		}

	    }

	    src_line_ptr += j_inc;
	    dst_line_ptr += j_inc;
	}

    } else {

	// this section is the reverse case (src_y == dst_y && src_x < dst_x)
	// horizontal scrolling uses this so it may need more optimization

	for (int jcx = 0; jcx < h; jcx++) {
	    unsigned char shift1 = ((src_x + i_1st) & 1) << 2;
	    unsigned char shift2 = ((dst_x + i_1st) & 1) << 2;
	    for (int icx = 0, i = i_1st; icx < w; icx++, i--) {
		unsigned char src_pixel = *(src_line_ptr+((src_x+i)>>1));
		unsigned char dst_pixel = *(dst_line_ptr+((dst_x+i)>>1));
		src_pixel = (src_pixel >> shift1) & 0x0F;
		src_pixel <<= shift2;
		dst_pixel &= (0xF0 >> shift2);
		*(dst_line_ptr+((dst_x+i)>>1)) = src_pixel | dst_pixel;
		shift1 = 4 - shift1;
		shift2 = 4 - shift2;
	    }
	    src_line_ptr += 320;
	    dst_line_ptr += 320;
	}

    }

#if 0

    // Original unoptimized version that handles all cases
    // This is a more readable form of approximately the above 100 lines
    // of code. It may be helpful in understanding it.
    for (int jcx = 0; jcx < h; jcx++) {
	unsigned char shift1 = ((src_x + i_1st) & 1) << 2;
	unsigned char shift2 = ((dst_x + i_1st) & 1) << 2;
	for (int icx = 0, i = i_1st; icx < w; icx++, i += i_inc) {
	    unsigned char src_pixel = *(src_line_ptr+((src_x+i)>>1));
	    unsigned char dst_pixel = *(dst_line_ptr+((dst_x+i)>>1));
	    src_pixel = (src_pixel >> shift1) & 0x0F;
	    src_pixel <<= shift2;
	    dst_pixel &= (0xF0 >> shift2);
	    *(dst_line_ptr+((dst_x+i)>>1)) = src_pixel | dst_pixel;
	    shift1 = 4 - shift1;
	    shift2 = 4 - shift2;
	}
	src_line_ptr += j_inc;
	dst_line_ptr += j_inc;
    }

#endif

    vga16_exposeDoubleBufferedRegion(dst_x, dst_y, dst_x + w, dst_y + h);

}


static void vga16_blt16BitBufToScr(int dst_x, int dst_y, unsigned short *srcdata, int w, int h)
{
    BEGIN_PROFILING

    unsigned int r,g,b, x = dst_x, i = w;

    while ( i-- ) {
	unsigned int hold = *srcdata++;
	r = ((hold & 0x0000f800) >> 11) << 3;
	g = ((hold & 0x000007e0) >>  5) << 2;
	b = ((hold & 0x0000001f) >>  0) << 3;
	unsigned char col = closestMatchUsingTable(r,g,b);
	*(db_line_ptrs[dst_y] + (x >> 1)) &= 0xF0 >> ((x & 1) << 2);
	*(db_line_ptrs[dst_y] + (x >> 1)) |= (col&0x0F) << ((x & 1) << 2);
	x++;
    }

    vga16_exposeDoubleBufferedRegion(dst_x, dst_y, dst_x + w - 1, dst_y + h - 1);
}


static void vga16_blt8BitBufToScr(int dst_x, int dst_y, unsigned char *src_data, int w, int h)
{
    BEGIN_PROFILING

    unsigned int x = dst_x, i = w;

    while ( i-- ) {
	unsigned char col = *src_data++;
	*(db_line_ptrs[dst_y] + (x >> 1)) &= 0xF0 >> ((x & 1) << 2);
	*(db_line_ptrs[dst_y] + (x >> 1)) |= (col&0x0F) << ((x & 1) << 2);
	x++;
    }

    vga16_exposeDoubleBufferedRegion(dst_x, dst_y, dst_x + w - 1, dst_y + h - 1);
}


//
// This is fairly well optimised
//
void QGfxVga16::drawrect_4(unsigned int x1, unsigned int y1,
	unsigned int x2, unsigned int y2, unsigned char col)
{
    BEGIN_PROFILING

    // unsigned char shift;
    unsigned int i, j;
    unsigned char *db_line_ptr = db_line_ptrs[y1];
    unsigned char *fb_line_ptr = fb_line_ptrs[y1];
    unsigned char *first_db_line_ptr = db_line_ptr;

    unsigned char middle_char = ((col & 0x0F) << 4) | (col & 0x0F);
    unsigned int tx1 = x1, tx2 = x2;

    if (x1 > x2)
	x1 = tx2, x2 = tx1;
    tx1 = x1, tx2 = x2;


    if (tx1 & 1) {
    	db_line_ptr = first_db_line_ptr + (tx1 >> 1);
	for (j = y1; j <= y2; j++, db_line_ptr += 320)
	    *db_line_ptr = (col << 4) | (*db_line_ptr & 0x0F);
	tx1++;
    }

    tx2++; // I didn't originally make it inclusive

    if (tx2 & 1) {
    	db_line_ptr = first_db_line_ptr + (tx2 >> 1);
	for (j = y1; j <= y2; j++, db_line_ptr += 320)
	    *db_line_ptr = (col & 0x0F) | (*db_line_ptr & 0xF0);
	tx2--;
    }

    if (tx1 < tx2) {
	unsigned int width = (tx2 - tx1) >> 1;
	db_line_ptr = first_db_line_ptr + (tx1 >> 1);
	for (j = y1; j <= y2; j++, db_line_ptr += 320) {
	    memset(db_line_ptr, middle_char, width);
	}
    }

    VGA16_CriticalSection cs;

    set_enable_set_reset_vga16(0x0F, 0);
    set_rotate_vga16(0x00, 0);
    set_mode_vga16(0x00, 0);
    set_write_planes_vga16(0x0F, 0);
    set_set_reset_vga16(col & 0x0F, 0);

    // this is a special case for narrow vertical strips that will
    // be in the same byte and need a special mask.
    if ((x1 >> 3) == (x2 >> 3)) {
	unsigned char *fb_ptr = fb_line_ptr + (x1 >> 3);
	unsigned int mask1 = 0xFF >> (x1 & 7);
	unsigned int mask2 = 0xFF << (7 - (x2 & 7));
	set_bit_mask_vga16(mask1 & mask2, 0);
    	for (i = y1; i <= y2; i++, fb_ptr += 80)
    	    *fb_ptr |= 0xFE;
	return;
    }

    unsigned char *fb_ptr = fb_line_ptr + (x1 >> 3);
    set_bit_mask_vga16(0xFF >> (x1 & 7), 0);
    for (i = y1; i <= y2; i++, fb_ptr += 80)
        *fb_ptr |= 0xFE;
    x1 += 8 - (x1 & 7);

    fb_ptr = fb_line_ptr + (x2 >> 3);
    set_bit_mask_vga16(0xFF << (7 - (x2 & 7)), 0);
    for (i = y1; i <= y2; i++, fb_ptr += 80)
        *fb_ptr |= 0xFE;
    x2 -= x2 & 7;

    set_bit_mask_vga16(0xFF, 0);
    if (x1 < x2) {
	unsigned int width = (x2 - x1) >> 3;
	fb_ptr = fb_line_ptr + (x1 >> 3);
        for (i = y1; i <= y2; i++, fb_ptr += 80) {
	    memset(fb_ptr, 0, width);
	}
    }
}


void QGfxVga16::himageline_4(unsigned int y, unsigned int x1, unsigned int x2, unsigned char *srcdata, bool reverse)
{
    BEGIN_PROFILING

    unsigned char SavedRow[640];
    unsigned int x = (!reverse) ? x1 : x2;
    unsigned int inc = (!reverse) ? 1 : -1;
    unsigned char gv = srccol;
    int w = x2 - x1 + 1;

    w = (w > 640) ? 640 : w;


    if ((srcdepth==4) && (!ismasking))
    {
	exit(0); // Should this really ever get called???

    } else if ((srcdepth==16) && (!ismasking)) {

	exit(0); // see if this is really getting called!!!

	// If this hypothetically was needed it would like like this:
	vga16_blt16BitBufToScr(x1,y,(unsigned short*)srcdata,w,1);
	return;

    } else if (srcdepth==8) {

	if ( !ismasking ) {

	    if (src_normal_palette) {

		// reverse is not expected
		if (reverse) printf("reverse!\n"), exit(0);

		if (reverse)
		    // If this hypothetically was needed it would like like this:
		    vga16_blt8BitBufToScr(x1,y,srcdata-w+1,w,1);
		else
		    vga16_blt8BitBufToScr(x1,y,srcdata,w,1);
		return;

	    } else {
		while ( w-- ) {
		    SavedRow[x] = transclut[*srcdata];
		    srcdata++;
		    x += inc;
		}
	    }
	} else {

	    while ( w-- ) {
		if ( srctype==SourceImage ) {
		    if (src_normal_palette)
		        gv = *srcdata;
		    else
		        gv = transclut[*srcdata];
		    srcdata++;
		}
		bool masked = TRUE;
		GET_MASKED(reverse, w);
		SavedRow[x] = (!masked) ? gv : getPixel( x, y );
		x += inc;
	    }
	}

	vga16_blt8BitBufToScr(x1,y,SavedRow+x1,x2-x1+1,1);
	return;

    } else {

	if ( !ismasking ) {
	    while ( w-- ) {
		unsigned int val = get_value_32(srcdepth,&srcdata,reverse);
		unsigned int r, g, b;
		r = (val & 0x00FF0000) >> 16;
		g = (val & 0x0000FF00) >>  8;
		b = (val & 0x000000FF) >>  0;
		SavedRow[x] = closestMatchUsingTable(r,g,b);
		x += inc;
	    }
	} else {
	    while ( w-- ) {
		if ( srctype==SourceImage ) {
		    unsigned int val = get_value_32(srcdepth,&srcdata,reverse);
		    unsigned int r, g, b;
		    r = (val & 0x00FF0000) >> 16;
		    g = (val & 0x0000FF00) >>  8;
		    b = (val & 0x000000FF) >>  0;
		    gv = closestMatchUsingTable(r,g,b);
		}
		bool masked = TRUE;
		GET_MASKED(reverse, w);
		SavedRow[x] = (!masked) ? gv : getPixel( x, y );
		x += inc;
	    }
	}
    }

    vga16_blt8BitBufToScr(x1,y,SavedRow+x1,x2-x1+1,1);
}


// Convert between pixel values for different depths
// reverse can only be true if sdepth == depth
GFX_INLINE unsigned int QGfxVga16::get_value_32(
		       int sdepth, unsigned char **srcdata, bool reverse)
{
    BEGIN_PROFILING

    unsigned int ret;
    if(sdepth==32) {
	ret = *((unsigned int *)(*srcdata));
	if(reverse) {
	    (*srcdata)-=4;
	} else {
	    (*srcdata)+=4;
	}
    } else if(sdepth==16) {
	unsigned int r,g,b;
	unsigned short int hold=*((unsigned short int *)(*srcdata));
	r=(hold & 0xf800) >> 11;
	g=(hold & 0x07e0) >> 5;
	b=(hold & 0x001f);
	r=r << 3;
	g=g << 2;
	b=b << 3;
	ret = 0;
	unsigned char * tmp=(unsigned char *)&ret;
	*(tmp+2)=r;
	*(tmp+1)=g;
	*(tmp+0)=b;
	(*srcdata)+=2;

    // Still needed for alpha blending
    } else if(sdepth==8) {
	unsigned char val=*((*srcdata));
	if(src_normal_palette) {
	    ret = clut[val];
	    // ret = val;
	} else {
	    ret = srcclut[val];
	    // ret = transclut[val];
	}
	(*srcdata)++;

    } else if(sdepth==1) {
	if(monobitcount<8) {
	    monobitcount++;
	} else {
	    monobitcount=1;	// yes, 1 is correct
	    (*srcdata)++;
	    monobitval=*((*srcdata));
	}
	if(src_little_endian) {
	    ret=monobitval & 0x1;
	    monobitval=monobitval >> 1;
	} else {
	    ret=(monobitval & 0x80) >> 7;
	    monobitval=monobitval << 1;
	    monobitval=monobitval & 0xff;
	}
	ret=srcclut[ret];
    } else {
	qDebug("Odd source depth %d!",sdepth);
	ret=0;
    }

    return ret;
}


GFX_INLINE void QGfxVga16::calcPacking(
			  void * m,int x1,int x2,
			  int & frontadd,int & backadd,int & count)
{
    BEGIN_PROFILING

    Q_UNUSED(m);

    frontadd = x2-x1+1;
    backadd = 0;
    count = 0;
    if(frontadd<0)
    	frontadd=0;
    return;
}


void QGfxVga16::setSource(const QPaintDevice * p)
{
    BEGIN_PROFILING

    QPaintDeviceMetrics qpdm(p);
    srclinestep=((QPaintDevice *)p)->bytesPerLine();
    srcdepth=qpdm.depth();
    if(srcdepth==0)
	abort();
    srcbits=((QPaintDevice *)p)->scanLine(0);
    srctype=SourceImage;
    setAlphaType(IgnoreAlpha);
    if ( p->devType() == QInternal::Widget ) {
	QWidget * w=(QWidget *)p;
	srcwidth=w->width();
	srcheight=w->height();
	QPoint hold;
	hold=w->mapToGlobal(hold);
	setSourceWidgetOffset( hold.x(), hold.y() );
	if ( srcdepth == 1 ) {
	    buildSourceClut(0, 0);
	} else if(srcdepth <= 8) {
	    src_normal_palette=TRUE;
	}
    } else if ( p->devType() == QInternal::Pixmap ) {
	//still a bit ugly
	QPixmap *pix = (QPixmap*)p;
	setSourceWidgetOffset( 0, 0 );
	srcwidth=pix->width();
	srcheight=pix->height();
	if ( srcdepth == 1 ) {
	    buildSourceClut(0, 0);
	} else if(srcdepth <= 8) {
    	    src_normal_palette=TRUE;
	}
    } else {
	// This is a bit ugly #### I'll say!
	//### We will have to find another way to do this
	setSourceWidgetOffset( 0, 0 );
	buildSourceClut(0,0);
	printf("setting an 8bit something as source\n");
    }

    src_little_endian=TRUE;
}


void QGfxVga16::setSource(const QImage * i)
{
    BEGIN_PROFILING

    srctype=SourceImage;
    srclinestep=i->bytesPerLine();
    srcdepth=i->depth();
    if(srcdepth==0)
	abort();
    srcbits=i->scanLine(0);
    src_little_endian=(i->bitOrder()==QImage::LittleEndian);
    setSourceWidgetOffset( 0, 0 );
    srcwidth=i->width();
    srcheight=i->height();
    src_normal_palette=FALSE;
    if ( srcdepth == 1 )
	buildSourceClut(0, 0);
    else  if(srcdepth<=8)
	buildSourceClut(i->colorTable(),i->numColors());
}

void QGfxVga16::buildSourceClut(QRgb * cols,int numcols)
{
    BEGIN_PROFILING

    if (!cols) {
	useBrush();
	srcclut[0]=pixel;
	transclut[0]=pixel;
	usePen();
	srcclut[1]=pixel;
	transclut[1]=pixel;
	return;
    }

    int loopc;

    // Copy clut
    for(loopc=0;loopc<numcols;loopc++)
	srcclut[loopc] = cols[loopc];

    for(loopc=0;loopc<numcols;loopc++) {
	int r = qRed(srcclut[loopc]);
	int g = qGreen(srcclut[loopc]);
	int b = qBlue(srcclut[loopc]);
	transclut[loopc] = closestMatchUsingTable(r,g,b);
    }
}

void QGfxVga16::drawPoint( int x, int y )
{
    BEGIN_PROFILING

    if(cpen.style()==NoPen)
	return;
    x += xoffs;
    y += yoffs;
    if (inClip(x,y)) {
	usePen();
        VGA16_GFX_START(QRect(x,y,2,2))
	setPixel(x,y, pixel);
        VGA16_GFX_END
    }
}


void QGfxVga16::drawPoints( const QPointArray & pa, int index, int npoints )
{
    BEGIN_PROFILING

    if(cpen.style()==NoPen)
	return;
    usePen();
    QRect cr;
    bool in = FALSE;

    VGA16_GFX_START(clipbounds);
    while (npoints--) {
	int x = pa[index].x() + xoffs;
	int y = pa[index].y() + yoffs;
	if ( !cr.contains(x,y) ) {
	    in = inClip(x,y,&cr);
	}
	if ( in ) {
	    setPixel(x,y,pixel);
	}
	++index;
    }
    VGA16_GFX_END
}

void QGfxVga16::drawLine( int x1, int y1, int x2, int y2 )
{
    BEGIN_PROFILING

    if(cpen.style()==NoPen)
	return;

    if (cpen.width() > 1) {
	drawThickLine( x1, y1, x2, y2 );
	return;
    }

    usePen();
    x1+=xoffs;
    y1+=yoffs;
    x2+=xoffs;
    y2+=yoffs;

    if(x1>x2) {
	int x3;
	int y3;
	x3=x2;
	y3=y2;
	x2=x1;
	y2=y1;
	x1=x3;
	y1=y3;
    }

    int dx=x2-x1;
    int dy=y2-y1;

    VGA16_GFX_START(QRect(x1, y1 < y2 ? y1 : y2, dx+1, QABS(dy)+1))

#ifdef QWS_EXPERIMENTAL_FASTPATH
    // Fast path
    if (y1 == y2 && !dashedLines && ncliprect == 1) {
	if ( x1 > cliprect[0].right() || x2 < cliprect[0].left()
	     || y1 < cliprect[0].top() || y1 > cliprect[0].bottom() ) {
	    VGA16_GFX_END
	    return;
	}
	x1 = x1 > cliprect[0].left() ? x1 : cliprect[0].left();
	x2 = x2 > cliprect[0].right() ? cliprect[0].right() : x2;
	drawrect_4(x1, y1, x2, y1, pixel);
	VGA16_GFX_END
	return;
    }
#endif
    // Bresenham algorithm from Graphics Gems

    int ax=QABS(dx)*2;
    int ay=QABS(dy)*2;
    int sx=dx>0 ? 1 : -1;
    int sy=dy>0 ? 1 : -1;
    int x=x1;
    int y=y1;

    int d;

    QRect cr;
    bool inside = inClip(x,y,&cr);
    if(ax>ay && !dashedLines) {
	d=ay-(ax >> 1);
	int px=x;
	#define FLUSH_VGA16(nx) \
		if ( inside ) \
		    if ( sx < 1 ) \
			drawrect_4(nx, y, px, y, pixel); \
		    else \
			drawrect_4(px, y, nx, y, pixel); \
		px = nx+sx;
	for(;;) {
	    if(x==x2) {
		FLUSH_VGA16(x);
		VGA16_GFX_END
		return;
	    }
	    if(d>=0) {
		FLUSH_VGA16(x);
		y+=sy;
		d-=ax;
		if ( !cr.contains(x+sx,y) )
		    inside = inClip(x+sx,y, &cr);
	    } else if ( !cr.contains(x+sx,y) ) {
		FLUSH_VGA16(x);
		inside = inClip(x+sx,y, &cr);
	    }
	    x+=sx;
	    d+=ay;
	}
    } else if (ax > ay) {
	// cannot use hline for dashed lines
	int di = 0;
	int dc = dashedLines ? dashes[0] : 0;
	d=ay-(ax >> 1);
	for(;;) {
	    if ( !cr.contains(x,y))
		inside = inClip(x,y, &cr);
	    if ( inside && (di&0x01) == 0) {
		setPixel(x,y,pixel);
	    }
	    if(x==x2) {
		VGA16_GFX_END
		return;
	    }
	    if (dashedLines && --dc <= 0) {
		if (++di >= numDashes)
		    di = 0;
		dc = dashes[di];
	    }
	    if(d>=0) {
		y+=sy;
		d-=ax;
	    }
	    x+=sx;
	    d+=ay;
	}
    } else {
	int di = 0;
	int dc = dashedLines ? dashes[0] : 0;
	d=ax-(ay >> 1);
	for(;;) {
	    // y is dominant so we can't optimise with hline
	    if ( !cr.contains(x,y))
		inside = inClip(x,y, &cr);
	    if ( inside && (di&0x01) == 0)
		setPixel(x,y,pixel);
	    if(y==y2) {
		VGA16_GFX_END
		return;
	    }
	    if (dashedLines && --dc <= 0) {
		if (++di >= numDashes)
		    di = 0;
		dc = dashes[di];
	    }
	    if(d>=0) {
		x+=sx;
		d-=ay;
	    }
	    y+=sy;
	    d+=ax;
	}
    }
    VGA16_GFX_END
}


void QGfxVga16::drawThickLine( int x1, int y1, int x2, int y2 )
{
    BEGIN_PROFILING

    QPointArray pa(5);
    int w = cpen.width() - 1;
    double a = atan2( y2 - y1, x2 - x1 );
    int ix = (int)(cos(a) * w / 2);
    int iy = (int)(sin(a) * w / 2);

    // No cap.
    pa[0].setX( x1 + iy );
    pa[0].setY( y1 - ix );
    pa[1].setX( x2 + iy );
    pa[1].setY( y2 - ix );
    pa[2].setX( x2 - iy );
    pa[2].setY( y2 + ix );
    pa[3].setX( x1 - iy );
    pa[3].setY( y1 + ix );

    pa[4] = pa[0];

    usePen();

    VGA16_GFX_START(clipbounds)
    scan(pa, FALSE, 0, 5);
    QPen savePen = cpen;
    cpen = QPen( cpen.color() );
    drawPolyline(pa, 0, 5);
    cpen = savePen;
    VGA16_GFX_END
}

//screen coordinates, clipped, x1<=x2
GFX_INLINE void QGfxVga16::hline( int x1,int x2,int y)
{
    BEGIN_PROFILING

    QRect cr;
    bool plot=inClip(x1,y,&cr);
    int x=x1;
    for (;;) {
	int xr = cr.right();
	if ( xr >= x2 ) {
	    if (plot)
		drawrect_4(x,y,x2,y,pixel);
	    break;
	} else {
	    if (plot)
		drawrect_4(x,y,xr,y,pixel);
	    x=xr+1;
	    plot=inClip(x,y,&cr,plot);
	}
    }
}


GFX_INLINE void QGfxVga16::hAlphaLineUnclipped( int x1,int x2,
						    unsigned int y,
						    unsigned char * srcdata,
						    unsigned char * alphas)
{
    BEGIN_PROFILING

    // First read in the destination line
    unsigned char *avp = alphas;
    unsigned char *srcptr = srcdata;
    unsigned char srcval4 = 0;
    unsigned int srcval32 = 0;
    unsigned int av;
    unsigned char alphabuf4[640];


    // SourcePen
    if (srctype == SourceImage) {
	for (int loopc = 0, i = x1; i <= x2; loopc++, i++) {

	    srcval32 = get_value_32(srcdepth,&srcptr);

	    if (alphatype == InlineAlpha)
		av = srcval32 >> 24;
	    else if (alphatype == SolidAlpha)
		av = calpha;
	    else
		av = *(avp++);

	    if (av == 255) {
		unsigned int r, g, b;
    		r = (srcval32 & 0x00FF0000) >> 16;
    		g = (srcval32 & 0x0000FF00) >>  8;
    		b = (srcval32 & 0x000000FF) >>  0;
		alphabuf4[loopc] = closestMatchUsingTable(r,g,b);
	    } else if (av == 0) {
		alphabuf4[loopc] = getPixel(i, y);
	    } else {
		unsigned int screen_val = get_pixel_4_32(i, y);
		int r2 = (screen_val & 0xff0000) >> 16,
		    g2 = (screen_val & 0x00ff00) >>  8,
		    b2 = (screen_val & 0x0000ff) >>  0;
		int r = (srcval32 & 0x00FF0000) >> 16,
		    g = (srcval32 & 0x0000FF00) >>  8,
		    b = (srcval32 & 0x000000FF) >>  0;
		r = (((r - r2) * av) >> 8) + r2;
		g = (((g - g2) * av) >> 8) + g2;
		b = (((b - b2) * av) >> 8) + b2;
		alphabuf4[loopc] = closestMatchUsingTable(r,g,b);
	    }
	}
    } else {
	// srctype != SourceImage
	srcval32 = clut[srccol];
	srcval4 = srccol;

    	for (int loopc = 0, i = x1; i <= x2; loopc++, i++) {

	    if (alphatype == InlineAlpha)
		av = srcval32 >> 24;
	    else if (alphatype == SolidAlpha)
		av = calpha;
	    else
		av = *(avp++);

	    if (av == 255) {
		alphabuf4[loopc] = srcval4;
	    } else if (av == 0) {
		alphabuf4[loopc] = getPixel(i, y);
	    } else {
		unsigned int screen_val = get_pixel_4_32(i, y);
		int r2 = (screen_val & 0xff0000) >> 16,
		    g2 = (screen_val & 0x00ff00) >>  8,
		    b2 = (screen_val & 0x0000ff) >>  0;
		int r = (srcval32 & 0xff0000) >> 16,
		    g = (srcval32 & 0x00ff00) >>  8,
		    b = (srcval32 & 0x0000ff) >>  0;
		r = (((r - r2) * av) >> 8) + r2;
		g = (((g - g2) * av) >> 8) + g2;
		b = (((b - b2) * av) >> 8) + b2;
		alphabuf4[loopc] = closestMatchUsingTable(r,g,b);
	    }
	}
    }

    int tmpdepth = srcdepth;

    unsigned char TmpRow[640];

    // Save the double buffer row if we are drawaing the mouse
    if (!set_back_buffer) {
	memcpy(TmpRow,db_line_ptrs[y],320);
    }

    vga16_blt8BitBufToScr(x1,y,alphabuf4,x2-x1+1,1);

    // restore the double buffer row if drawing the mouse
    if (!set_back_buffer) {
	memcpy(db_line_ptrs[y],TmpRow,320);
    }

    srcdepth = tmpdepth;
}


void QGfxVga16::fillRect( int rx,int ry,int w,int h )
{
    BEGIN_PROFILING

    setAlphaType(IgnoreAlpha);
    ismasking=FALSE;
    if ( w <= 0 || h <= 0 ) return;

    VGA16_GFX_START(QRect(rx+xoffs, ry+yoffs, w+1, h+1))

#ifdef QWS_EXPERIMENTAL_FASTPATH
    // ### fix for 8bpp
    // This seems to be reliable now, at least for 16bpp

    if (ncliprect == 1 && cbrush.style()==SolidPattern) {
	// Fast path
	    useBrush();
	    int x1,y1,x2,y2;
	    rx+=xoffs;
	    ry+=yoffs;
	    x2=rx+w-1;
	    y2=ry+h-1;
	    if(rx>cliprect[0].right() || ry>cliprect[0].bottom() ||
	       x2<cliprect[0].left() || y2<cliprect[0].top()) {
		VGA16_GFX_END
	        return;
	    }
	    x1=cliprect[0].left() > rx ? cliprect[0].left() : rx;
	    y1=cliprect[0].top() > ry ? cliprect[0].top() : ry;
	    x2=cliprect[0].right() > x2 ? x2 : cliprect[0].right();
	    y2=cliprect[0].bottom() > y2 ? y2 : cliprect[0].bottom();

	    if((x2<x1) || (y2<y1)) {
		VGA16_GFX_END
		return;
	    }

	    drawrect_4(x1,y1,x2,y2,pixel);
	    VGA16_GFX_END
	    return;
    }
#endif // QWS_EXPERIMENTAL_FASTPATH

    if( (cbrush.style()!=QBrush::NoBrush) &&
	(cbrush.style()!=QBrush::SolidPattern) ) {
	srcwidth=cbrushpixmap->width();
	srcheight=cbrushpixmap->height();
	if(cbrushpixmap->depth()==1) {
	    if(opaque) {
		setSource(cbrushpixmap);
		setAlphaType(IgnoreAlpha);
		useBrush();
		srcclut[0]=clut[pixel]; // pixel
		QBrush tmp=cbrush;
		cbrush=QBrush(backcolor);
		useBrush();
		srcclut[1]=clut[pixel]; // pixel;
		cbrush=tmp;
	    } else {
		useBrush();
		srccol=pixel;
		srctype=SourcePen;
		setAlphaType(LittleEndianMask);
		setAlphaSource(cbrushpixmap->scanLine(0),
			       cbrushpixmap->bytesPerLine());
	    }
	} else {
	    setSource(cbrushpixmap);
	    setAlphaType(IgnoreAlpha);
	}
	tiledBlt(rx,ry,w,h);
    } else if(cbrush.style()!=NoBrush) {
	useBrush();
	rx += xoffs;
	ry += yoffs;
	// Gross clip
	if ( rx < clipbounds.left() ) {
	    w -= clipbounds.left()-rx;
	    rx = clipbounds.left();
	}
	if ( ry < clipbounds.top() ) {
	    h -= clipbounds.top()-ry;
	    ry = clipbounds.top();
	}
	if ( rx+w-1 > clipbounds.right() )
	    w = clipbounds.right()-rx+1;
	if ( ry+h-1 > clipbounds.bottom() )
	    h = clipbounds.bottom()-ry+1;
	if ( w > 0 && h > 0 ) {
	    for (int j=0; j<h; j++,ry++)
		hline(rx,rx+w-1,ry);
	}
    }
    VGA16_GFX_END
}


void QGfxVga16::drawPolyline( const QPointArray &a,int index, int npoints )
{
    BEGIN_PROFILING

    //int m=QMIN( index+npoints-1, int(a.size())-1 );
    VGA16_GFX_START(clipbounds)
    int loopc;
    int end;
    end=(index+npoints) > (int)a.size() ? a.size() : index+npoints;
    for(loopc=index+1;loopc<end;loopc++) {
	    drawLine(a[loopc-1].x(),a[loopc-1].y(),
		     a[loopc].x(),a[loopc].y());
    }
    // ### beware XOR mode vertices
    VGA16_GFX_END
}


void QGfxVga16::drawPolygon( const QPointArray &pa, bool winding, int index, int npoints )
{
    BEGIN_PROFILING

    useBrush();
    VGA16_GFX_START(clipbounds)
    if ( cbrush.style()!=QBrush::NoBrush )
	scan(pa,winding,index,npoints);
    drawPolyline(pa, index, npoints);
    if (pa[index] != pa[index+npoints-1]) {
	drawLine(pa[index].x(), pa[index].y(),
		pa[index+npoints-1].x(),pa[index+npoints-1].y());
    }
    VGA16_GFX_END
}


void QGfxVga16::processSpans( int n, QPoint* point, int* width )
{
    BEGIN_PROFILING

    while (n--) {
	int x=point->x()+xoffs;
	if ( *width > 0 ) {
	    if(patternedbrush) {
		// ###
	    } else {
		hline(x,x+*width-1,point->y()+yoffs);
	    }
	}
	point++;
	width++;
    }
}

void QGfxVga16::scroll( int rx,int ry,int w,int h,int sx, int sy )
{
    BEGIN_PROFILING

    if (!w || !h)
	return;

    int dy = sy - ry;
    int dx = sx - rx;

    if (dx == 0 && dy == 0)
	return;

    VGA16_GFX_START(QRect(QMIN(rx+xoffs,sx+xoffs), QMIN(ry+yoffs,sy+yoffs), w+QABS(dx)+1, h+QABS(dy)+1))

    srcbits=buffer;
    src_normal_palette=TRUE;
    srclinestep=linestep();
    srcdepth=4;
    srctype=SourceImage;
    setAlphaType(IgnoreAlpha);
    ismasking=FALSE;
    setSourceWidgetOffset( xoffs, yoffs );
    blt(rx,ry,w,h,sx,sy);
    src_normal_palette=FALSE;

    VGA16_GFX_END
}

void QGfxVga16::blt( int rx,int ry,int w,int h,int sx,int sy )
{
    BEGIN_PROFILING

    if ( !w || !h ) return;

    int osrcdepth=srcdepth;

    if(srctype==SourcePen) {
	srclinestep=0;//w;
	srcdepth=0;
	usePen();
    }

    rx += xoffs;
    ry += yoffs;
    QRect cr;

    QRect cursRect(rx, ry, w+1, h+1);

    VGA16_GFX_START(cursRect);

    sx += srcwidgetoffs.x();
    sy += srcwidgetoffs.y();


    int dl = linestep();
    int sl = srclinestep;
    int dj = 1;
    int dry = 1;
    int tj;
    int j;
    if(sy < ry) {
	// Back-to-front
	dj = -dj;
	dl = -dl;
	sl = -sl;
	dry = -dry;
	j = h-1;
	ry=(ry+h)-1;
	tj = -1;
    } else {
	j = 0;
	tj = h;
    }

    unsigned char *srcline = srcScanLine(j+sy);

    if ( alphatype == InlineAlpha || alphatype == SolidAlpha ||
	 alphatype == SeparateAlpha ) {
	alphabuf = new unsigned int[w];
    }

    // allows us to optimise GET_MASK a little
    if (alphatype==LittleEndianMask) {
	amonolittletest = TRUE;
    } else if (alphatype == BigEndianMask) {
	amonolittletest = FALSE;
    }

    for (; j!=tj; j+=dj,ry+=dry,srcline+=sl) {
	bool plot;
	{
	    PROFILE_BLOCK(inclip1)
	    plot=inClip(rx,ry,&cr);
	}
	int x=rx;
	for (;;) {
	    int x2 = cr.right();
	    if ( x2 > rx+w-1 ) {
		x2 = rx+w-1;
		if ( x2 < x ) break;
	    }
	    if (plot) {

		if ((srcbits == buffer) && (srctype == SourceImage)) {

	    	    vga16_bltScrToScr(x,ry,x-rx+sx,j+sy,x2-x+1,1);

	    	} else {

		    unsigned char *srcptr = srcline;
		    if ( srctype == SourceImage ) {
			if ( srcdepth == 1) {
			    srcptr=find_pointer(srcbits,(x-rx)+sx,
					 j+sy, x2-x, srclinestep,
					 monobitcount, monobitval,
					 !src_little_endian, FALSE);
			} else {
			    srcptr = srcline + (x-rx+sx)*srcdepth/8;
			}
		    }

		    switch ( alphatype ) {
		      case LittleEndianMask:
		      case BigEndianMask:
			maskp=find_pointer(alphabits,(x-rx)+sx,
					   j+sy, x2-x, alphalinestep,
					   amonobitcount,amonobitval,
					   alphatype==BigEndianMask, FALSE);
			// Fall through
		      case IgnoreAlpha:
			himageline_4( ry, x, x2, srcptr, FALSE);
			break;
		      case InlineAlpha:
		      case SolidAlpha:
			hAlphaLineUnclipped(x,x2,ry,srcptr,0);
			break;
		      case SeparateAlpha:
			// Separate alpha table
			unsigned char * alphap=alphabits+(j*alphalinestep)
					       +(x-rx);
			hAlphaLineUnclipped(x,x2,ry,srcptr,alphap);
		    }

		}

	    }
	    if ( x >= rx+w-1 )
		break;
	    x=x2+1;
	    {
		PROFILE_BLOCK(inclip2)
		plot=inClip(x,ry,&cr,plot);
	    }
	}
    }

    if ( alphabuf ) {
	delete [] alphabuf;
	alphabuf = 0;
    }

    srcdepth=osrcdepth;

    VGA16_GFX_END
}


#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS)
void QGfxVga16::stretchBlt( int rx,int ry,int w,int h,int sw,int sh )
{
    BEGIN_PROFILING

    Q_UNUSED(rx);
    Q_UNUSED(ry);
    Q_UNUSED(w);
    Q_UNUSED(h);
    Q_UNUSED(sw);
    Q_UNUSED(sh);
    qDebug("Can't cope with stretchblt in VGA16");
}
#endif


void QGfxVga16::tiledBlt( int rx,int ry,int w,int h )
{
    BEGIN_PROFILING

    VGA16_GFX_START(QRect(rx+xoffs, ry+yoffs, w+1, h+1))

    useBrush();
    unsigned char * savealphabits=alphabits;

    int offx = brushoffs.x();
    int offy = brushoffs.y();

    // from qpainter_qws.cpp
    if ( offx < 0 )
        offx = srcwidth - -offx % srcwidth;
    else
        offx = offx % srcwidth;
    if ( offy < 0 )
        offy = srcheight - -offy % srcheight;
    else
        offy = offy % srcheight;

    int yPos, xPos, drawH, drawW, yOff, xOff;
    yPos = ry;
    yOff = offy;
    while( yPos < ry + h ) {
        drawH = srcheight - yOff;    // Cropping first row
        if ( yPos + drawH > ry + h )        // Cropping last row
            drawH = ry + h - yPos;
        xPos = rx;
        xOff = offx;
        while( xPos < rx + w ) {
            drawW = srcwidth - xOff; // Cropping first column
            if ( xPos + drawW > rx + w )    // Cropping last column
                drawW = rx + w - xPos;
	    blt(xPos, yPos, drawW, drawH, xOff, yOff);
	    alphabits=savealphabits;
            xPos += drawW;
            xOff = 0;
        }
        yPos += drawH;
        yOff = 0;
    }

    VGA16_GFX_END
}




QVga16Screen::QVga16Screen( int display_id )
    : QLinuxFbScreen( display_id )
{
    BEGIN_PROFILING
}

bool QVga16Screen::useOffscreen() { return true; }
int QVga16Screen::pixmapDepth() const { return 8; }
int QVga16Screen::pixmapOffsetAlignment() { return 128; }
int QVga16Screen::pixmapLinestepAlignment() { return 128; }

bool QVga16Screen::connect( const QString &displaySpec )
{
    BEGIN_PROFILING

    fb_fix_screeninfo finfo;
    QRegExp r( QString::fromLatin1("/dev/fb[0-9]+") );
    int len;
    int m = r.match( displaySpec, 0, &len );

    QString dev = (m>=0) ? displaySpec.mid( m, len ) : QString("/dev/fb0");

    int fd = open( dev, O_RDWR );
    int res = ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
    close( fd );
    if ( res || finfo.type != FB_TYPE_VGA_PLANES )
	return FALSE;

    if ( !QLinuxFbScreen::connect( displaySpec ) )
	return FALSE;

    if (-1 == ioperm (0x3c0, 0x20, 1)) {
	qDebug( "IO permissions problem (for VGA16 you need to run as root)\n" );
	return FALSE;
    }


#define SCREEN_BUFFER_SIZE	((640*480/2)+512) // 512 bytes for vga_register_values
    unsigned char *shared_memory;

#ifdef QT_NO_QWS_MULTIPROCESS
    shared_memory = (unsigned char *)malloc(SCREEN_BUFFER_SIZE);
#else
    shmId = shmget( (key_t)0xBEEFDEAD, SCREEN_BUFFER_SIZE, IPC_CREAT | 0666);
    if ( shmId != -1 ) {
	shared_memory = (unsigned char *)shmat( shmId, (void *)0, 0 );
    } else {
	shmctl( shmId, IPC_RMID, 0 );
	shmId = shmget( (key_t)0xBEEFDEAD, SCREEN_BUFFER_SIZE, IPC_CREAT | 0666);
	shared_memory = (unsigned char *)shmat( shmId, (void *)0, 0 );
    }
#endif

    if ( (int)shared_memory == -1 || shared_memory == 0 ) {
	qDebug( "shared memory or malloc failure\n" );
	return FALSE;
    }

    vga_register_values = shared_memory;
    screen_double_buffer = shared_memory + 512;

    if (screen_double_buffer == NULL) {
	qDebug( "error getting screen_double_buffer memory\n" );
	return FALSE;
    }

    unsigned char *db_line_ptr = screen_double_buffer;

    for (int i = 0; i < 480; i++)
    {
	db_line_ptrs[i] = db_line_ptr;
	db_line_ptr += 640/2;
    }

    return TRUE;
}


/*

void QLinuxFbScreen::disconnect()
{
    shmdt( (char*)screen_double_buffer );
    shmctl( shmId, IPC_RMID, 0 );

    munmap((char*)data,mapsize);
    close(fd);
    // a reasonable screensaver timeout
    printf( "\033[9;15]" );
    fflush( stdout );
}

*/


QVga16Screen::~QVga16Screen()
{
    BEGIN_PROFILING
}

bool QVga16Screen::initDevice()
{
    BEGIN_PROFILING

    QLinuxFbScreen::initDevice();

    // turn off the blinking cursor
    write(1, "\033[?25l", 6);

    // set the registers with default values
    set_read_map_vga16(0x00,1);
    set_memory_mode_vga16(0x06,1);
    set_enable_set_reset_vga16(0x0F, 1);
    set_rotate_vga16(0x00, 1);
    set_mode_vga16(0x00, 1);
    set_write_planes_vga16(0x0F, 1);
    set_set_reset_vga16(0x00, 1);
    set_bit_mask_vga16(0xFF, 1);

    // clear the screen
    unsigned char *base_ptr = base() - dataoffset;
    memset(base_ptr, *base_ptr, 640*480/8);
    // clear the shared memory
    memset(screen_double_buffer, 0, 640*480/2);

    return TRUE;
}

int QVga16Screen::initCursor(void* e, bool init)
{
    BEGIN_PROFILING

#ifndef QT_NO_QWS_CURSOR
    qt_screencursor=new QVga16Cursor();
    qt_screencursor->init((SWCursorData*)e - 1,init);
    return sizeof(SWCursorData*);
#endif
    return 0;
}

void QVga16Screen::shutdownDevice()
{
    BEGIN_PROFILING
    // ProfiledApp.DumpRawProfileStats(stdout);

    QLinuxFbScreen::shutdownDevice();
}

int QVga16Screen::alloc(unsigned int r, unsigned int g, unsigned int b)
{
    BEGIN_PROFILING

    return closestMatchUsingTable(r,g,b);
}

QGfx * QVga16Screen::createGfx(unsigned char * b,int w,int h,int d_arg,int linestep)
{
    BEGIN_PROFILING

    if (d_arg == 4) {
	d = d_arg;
    	QGfx *ret = new QGfxVga16(b,w,h);

	unsigned char *fb_line_ptr = b;

	int x_offset = (640 - w) / 2;
	int y_offset = (480 - h) / 2;

	// Remove the centering of the frame buffer apply from
	// qgfxlinux_qws.cpp as it is wrong for VGA16.
	fb_line_ptr -= dataoffset;
	// apply correct centering (taking in to account it uses bit-planes)
	fb_line_ptr += (x_offset/8) + (y_offset*640/8);

	for (int i = 0; i < 480; i++)
	{
		fb_line_ptrs[i] = fb_line_ptr;
		fb_line_ptr += 640/8;
	}

	if (ret) {
    	    ret->setLineStep(linestep);

	    VGA16_CriticalSection cs;

	    // set the registers with default values
	    // memset(vga_register_values, 0, 512);
	    set_read_map_vga16(0x00,1);
	    set_memory_mode_vga16(0x06,1);
	    set_enable_set_reset_vga16(0x0F, 1);
	    set_rotate_vga16(0x00, 1);
	    set_mode_vga16(0x00, 1);
	    set_write_planes_vga16(0x0F, 1);
	    set_set_reset_vga16(0x00, 1);
	    set_bit_mask_vga16(0xFF, 1);

    	    return ret;
       	}
    }

    return QLinuxFbScreen::createGfx(b,w,h,d_arg,linestep);
}


#ifndef QT_NO_QWS_CURSOR


void QVga16Cursor::set(const QImage& image, int hotx, int hoty)
{
    BEGIN_PROFILING

    restoreUnder();
    data->hotx = hotx;
    data->hoty = hoty;
    data->width = image.width();
    data->height = image.height();
    memcpy(data->cursor, image.bits(), image.numBytes());
    data->colors = image.numColors();
    if ( gfx && gfx->bitDepth() <= 8 ) {
	for (int i = 0; i < image.numColors(); i++) {
	    int r = qRed( image.colorTable()[i] );
	    int g = qGreen( image.colorTable()[i] );
	    int b = qBlue( image.colorTable()[i] );
	    data->translut[i] = closestMatchUsingTable(r,g,b);
	}
    }
    for (int i = 0; i < image.numColors(); i++) {
	data->clut[i] = image.colorTable()[i];
    }
    data->bound = QRect( data->x - data->hotx, data->y - data->hoty,
		   data->width+1, data->height+1 );
    drawCursor(data->bound);
}

void QVga16Cursor::move(int x, int y)
{
    BEGIN_PROFILING

    restoreUnder();

    data->x = x;
    data->y = y;

    data->bound = QRect( data->x - data->hotx, data->y - data->hoty,
		   data->width+1, data->height+1 );

    drawCursor(data->bound);
}

void QVga16Cursor::restoreUnder()
{
    BEGIN_PROFILING

    if (!data || !data->enable)
	return;

    int x = data->x - data->hotx;
    int y = data->y - data->hoty;

    if ( data->width && data->height ) {
	vga16_exposeDoubleBufferedRegion(x, y, x + data->width - 1, y + data->height - 1);
    }
}


void QVga16Cursor::drawCursor(QRect &r)
{
    BEGIN_PROFILING

    int x = data->x - data->hotx;
    int y = data->y - data->hoty;

    if (!r.intersects(data->bound))
	return;

    if ( !cursor || data->width != cursor->width() || data->height != cursor->height() ) {
	delete cursor;
	cursor = new QImage( data->cursor, data->width, data->height, 8,
			 data->clut, data->colors, QImage::IgnoreEndian );
    }
    if ( gfx && data->width && data->height ) {
	qt_sw_cursor = FALSE;   // prevent recursive call from blt
	gfx->setSource( cursor );
	gfx->setAlphaType(QGfx::InlineAlpha);

	set_back_buffer = FALSE;
	force_set_pixel = TRUE;
	gfx->blt(x,y,data->width,data->height,0,0);
	force_set_pixel = FALSE;
	set_back_buffer = TRUE;

	qt_sw_cursor = TRUE;
    }
}

#endif // QT_NO_QWS_CURSOR

#endif // QT_NO_QWS_VGA16
