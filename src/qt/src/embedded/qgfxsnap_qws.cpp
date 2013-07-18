/****************************************************************************
**
** Implementation of QSNAPScreen and QGfxSNAP classes for Embedded Qt
**
** Copyright (C) SciTech Software, Inc.  All rights reserved.
**
** Author:  Kendall Bennett
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
** This module requires the SciTech SNAP SDK to build and also requires
** the SciTech SNAP runtime device driver modules to be present in order
** to run.
**
** See http://www.scitechsoft.com or email sales@scitechsoft.com for
** information about SciTech SNAP Commercial License Agreements.
**
**********************************************************************/

#ifndef QT_NO_QWS_SNAP

#include "qgfxsnap_qws.h"
#include "qgfxraster_qws.h"

// TODO: Can we eliminate these external references here, and just set up gfx_optype
//       ourselves instead of using these values that are not referenced anywhere?

extern volatile int *optype;
extern volatile int *lastop;

// An integer, optype, is stored in shared memory and is set to one
// of these values.

#define OPTYPE_ACCEL        1

// Mix table to translate from non-standard Qt ROP codes to standard 4-bit
// two way hardware ROP codes.
static int mixTable[] = {
    GA_R2_COPYSRC,      // S    - CopyROP
    GA_R2_MERGESRC,     // DSo  - OrROP
    GA_R2_XORSRC,       // DSx  - XorROP
    GA_R2_MASKNOTSRC,   // DSna - NotAndROP
    GA_R2_NOTCOPYSRC,   // Sn   - NotCopyROP
    GA_R2_MERGENOTSRC,  // DSno - NotOrROP
    GA_R2_NOTXORSRC,    // DSxn - NotXorROP
    GA_R2_MASKSRC,      // DSa  - AndROP
    GA_R2_NOT,          // Dn   - NotROP
    GA_R2_BLACK,        // 0    - ClearROP
    GA_R2_WHITE,        // 1    - SetROP
    GA_R2_NOP,          // D    - NopROP
    GA_R2_MASKSRCNOT,   // SDna - AndNotROP
    GA_R2_MERGESRCNOT,  // SDno - OrNotROP
    GA_R2_NOTMASKSRC,   // DSan - NandROP
    GA_R2_NOTMERGESRC,  // DSon - NorROP
    };

/*!
\struct QGfxSNAP_State
\brief The QGfxSNAP_State class manages the shared rendering state

Main structure to manage the shared rendering state for all processes
connected to the main server process.
*/

struct QGfxSNAP_State {
    int         optype;
    int         lastop;
    N_int32     mix;
    GA_color    foreColor;
    GA_color    backColor;
    GA_buffer   drawBuf;
    };

/*!
\class QGfxSNAP
\brief The QGfxSNAP class manages rendering to SNAP Graphics devices

Main class to create a graphics rendering object for the specific color
depth and type
*/

template <const int depth, const int type>
class QGfxSNAP : public QGfxRaster<depth,type> {

public:
    QGfxSNAP(unsigned char *buffer,int width,int height,QGfxSNAP_State *cntState,
        GA_2DStateFuncs &hwState2d,GA_2DRenderFuncs &hwDraw2d,
        GA_2DStateFuncs &state2d,GA_2DRenderFuncs &draw2d);

//    virtual void drawPoint(int,int);
//    virtual void drawPoints(const QPointArray &,int,int);
//    virtual void drawPolyline(const QPointArray &,int,int);
    virtual void fillRect(int,int,int,int);
    virtual void blt(int,int,int,int,int,int);
    virtual void scroll(int,int,int,int,int,int);
//    virtual void stretchBlt(int,int,int,int,int,int);
//    virtual void tiledBlt(int,int,int,int);
    virtual void drawLine(int,int,int,int);
    virtual void sync();

    // TODO: We need a way to override and accelerate the optimised
    //       horizontal and vertical line functions. Right now they are
    //       not virtual functions in the QGfxRaster class, so we either
    //       need to make them virtual (ala the QNX6 HLine) or find some
    //       other way to hook this out.
//    void vline(int,int,int);
//    void hline(int,int,int);
//    void hlineUnclipped(int,int,unsigned char* );

private:
    bool checkDest();
    bool checkSourceDest();

private:
    GA_buffer           drawBuf;
    ulong               srcOffset;
    QGfxSNAP_State      *cntState;
    GA_2DStateFuncs     &hwState2d;
    GA_2DRenderFuncs    &hwDraw2d;
    GA_2DStateFuncs     &state2d;
    GA_2DRenderFuncs    &draw2d;
};

/*!
Constructor for the QGfxSNAP class
*/

template<const int depth,const int type>
QGfxSNAP<depth,type>::QGfxSNAP(
    unsigned char * buffer,
    int width,
    int height,
    QGfxSNAP_State *cntState,
    GA_2DStateFuncs &hwState2d,
    GA_2DRenderFuncs &hwDraw2d,
    GA_2DStateFuncs &state2d,
    GA_2DRenderFuncs &draw2d)
    : QGfxRaster<depth,type>(buffer,width,height),
        cntState(cntState),
        hwState2d(hwState2d), hwDraw2d(hwDraw2d),
        state2d(state2d), draw2d(draw2d)
{
}

/*!
Internal function to check that the destination buffer is in graphics
memory or not. If it is in graphics memory, we go ahead and set the
destination draw buffer to that location.
*/

template<const int depth,const int type>
inline bool QGfxSNAP<depth,type>::checkDest()
{
    ulong   offset;

    if (!gfx_screen->onCard(buffer,offset))
        return false;
    drawBuf.dwSize = sizeof(drawBuf);
    drawBuf.Offset = offset;
    drawBuf.Stride = linestep();
    drawBuf.Width = pixelWidth();
    drawBuf.Height = pixelHeight();
    return true;
}

/*!
Internal function to check that the source and destination buffers are
in graphics memory. If they are, we set up for drawing and return true.

TODO: We can handle system memory blits with hardware ROP's etc, so we
      need to add code to allow a hardware destination buffer but a
      software source buffer!
*/

template<const int depth,const int type>
inline bool QGfxSNAP<depth,type>::checkSourceDest()
{
    if (!checkDest())
        return false;
    return gfx_screen->onCard(srcbits,srcOffset);
}

/*!
This is called to draw a rectangle. We try and draw the rectangle
in hardware if possible.
*/

template<const int depth,const int type>
void QGfxSNAP<depth,type>::fillRect(
    int left,
    int top,
    int width,
    int height)
{
    // Check that it is not trivially clipped
    if (ncliprect < 1)
        return;

    // Check that there is no pattern applied to the rectangle
    if (cbrush.style() != SolidPattern) {
        QGfxRaster<depth,type>::fillRect(left,top,width,height);
        return;
        }

    // Check that the buffer is a hardware buffer, punting to software if not
    if (!checkDest()) {
        QGfxRaster<depth,type>::fillRect(left,top,width,height);
        return;
        }

    // Get exclusive write access to the display and get ready for drawing
    QWSDisplay::grab(true);
    GFX_START(QRect(left+xoffs, top+yoffs, width+1, height+1));

    // Set the required hardware state if is has changed
    if (memcmp(&cntState->drawBuf,&drawBuf,sizeof(drawBuf)) != 0) {
        cntState->drawBuf = drawBuf;
        state2d.SetDrawBuffer(&drawBuf);
        }
    if (mixTable[myrop] != cntState->mix)
        state2d.SetMix(cntState->mix = mixTable[myrop]);
    useBrush();
    if (pixel != cntState->foreColor)
        state2d.SetForeColor(cntState->foreColor = pixel);

    // Draw the rectangle clipped to all clip rectangles
    int i;
    int right = (left += xoffs) + (width - 1);
    int bottom = (top += yoffs) + (height - 1);
    int numrects = ncliprect;

    for (i = 0; i < numrects; i++) {
        QRect clip(cliprect[i]);
        if (left <= clip.right() && top <= clip.bottom() && right >= clip.left() && bottom >= clip.top()) {
            int clipLeft = clip.left() > left ? clip.left() : left;
            int clipTop = clip.top() > top ? clip.top() : top;
            int clipRight = clip.right() > right ? right : clip.right();
            int clipBottom = clip.bottom() > bottom ? bottom : clip.bottom();
            draw2d.DrawRect(clipLeft,clipTop,(clipRight-clipLeft)+1,(clipBottom-clipTop)+1);
            }
        }

    // Indicate that we have done an accelerated drawing function, so that
    // software rendering code will need to do a sync() before accessing
    // video memory.
    (*gfx_optype) = OPTYPE_ACCEL;

    // Clean up and exit
    GFX_END;
    QWSDisplay::ungrab();
}

/*!
Function to sort rectangle clip list for down and right blit operations.
Since the rectangles are always guaranteed to be non-overlapping, we can
make this fast by only comparing the (top,left) coordinates of the
rectangles.
*/

static int cmp_down_right(
    const void *p1,const void *p2)
{
    const QRect *r1 = (const QRect*)p1;
    const QRect *r2 = (const QRect*)p2;
    if (r1->left() >= r2->left()) {
        if (r1->top() >= r2->top())
            return -1;  // R1
        else
            return 1;   // R2
        }
    else {
        if (r1->top() > r2->top())
            return -1;  // R1
        else
            return 1;   // R2
        }
}

/*!
Function to sort rectangle clip list for down and left blit operations
*/

static int cmp_down_left(
    const void *p1,const void *p2)
{
    const QRect *r1 = (const QRect*)p1;
    const QRect *r2 = (const QRect*)p2;
    if (r1->left() > r2->left()) {
        if (r1->top() > r2->top())
            return -1;  // R1
        else
            return 1;   // R2
        }
    else {
        if (r1->top() >= r2->top())
            return -1;  // R1
        else
            return 1;   // R2
        }
}

/*!
Function to sort rectangle clip list for up and right blit operations
*/

static int cmp_up_right(
    const void *p1,const void *p2)
{
    const QRect *r1 = (const QRect*)p1;
    const QRect *r2 = (const QRect*)p2;
    if (r1->left() >= r2->left()) {
        if (r1->top() <= r2->top())
            return -1;  // R1
        else
            return 1;   // R2
        }
    else {
        if (r1->top() < r2->top())
            return -1;  // R1
        else
            return 1;   // R2
        }
}

/*!
Function to sort rectangle clip list for up and left blit operations
*/

static int cmp_up_left(
    const void *p1,const void *p2)
{
    const QRect *r1 = (const QRect*)p1;
    const QRect *r2 = (const QRect*)p2;
    if (r1->left() >= r2->left()) {
        if (r1->top() < r2->top())
            return -1;  // R1
        else
            return 1;   // R2
        }
    else {
        if (r1->top() <= r2->top())
            return -1;  // R1
        else
            return 1;   // R2
        }
}

/*!
Function to handle the common case of blting a rectangle a small distance
within the same drawing surface (for example when scrolling a listbox).
*/

template <const int depth, const int type>
void QGfxSNAP<depth,type>::scroll(
    int left,
    int top,
    int width,
    int height,
    int srcLeft,
    int srcTop)
{
    // Check to see if we can trivially reject the blit
    if (ncliprect < 1)
        return;
    if (!width || !height)
        return;
    int dy = srcTop - top;
    int dx = srcLeft - left;
    if (dx == 0 && dy == 0)
        return;

    // Check that the buffers are in hardware, punting to software if not
    srcbits = buffer;
    if (!checkSourceDest()) {
        QGfxRaster<depth,type>::scroll(left,top,width,height,srcLeft,srcTop);
        return;
        }

    // Setup pixel coordinates and loop variables
    left += xoffs;
    top += yoffs;
    srcLeft += xoffs;
    srcTop += yoffs;
    int right = left + width - 1;
    int bottom = top + height - 1;
    int mix = mixTable[myrop];
    int i,numrects = ncliprect;

    // If we have to do complex clipping, we need to sort the clip rectangle list
    // for the correct order based on the direction of the scroll. If we don't do that
    // we end up with incorrect results!
    if (numrects > 1) {
        if (srcTop < top) {
            // Down direction
            if (srcLeft < left) {
                // Down and right direction
                qsort(cliprect, numrects, sizeof(cliprect[0]), cmp_down_right);
                }
            else {
                // Down and left direction
                qsort(cliprect, numrects, sizeof(cliprect[0]), cmp_down_left);
                }
            }
        else {
            // Up direction
            if (srcLeft < left) {
                // Up and right direction
                qsort(cliprect, numrects, sizeof(cliprect[0]), cmp_up_right);
                }
            else {
                // Up and left direction.
                qsort(cliprect, numrects, sizeof(cliprect[0]), cmp_up_left);
                }
            }
        }

    // Get exclusive write access to the display and get ready for drawing
    QWSDisplay::grab(true);
    GFX_START(QRect(QMIN(left+xoffs,srcLeft+xoffs), QMIN(top+yoffs,srcTop+yoffs), width+QABS(dx)+1, height+QABS(dy)+1));

    // Set the required hardware state if is has changed
    if (memcmp(&cntState->drawBuf,&drawBuf,sizeof(drawBuf)) != 0) {
        cntState->drawBuf = drawBuf;
        state2d.SetDrawBuffer(&drawBuf);
        }

    // Now clip the blit to each clip rectangle do the blit
    for (i = 0; i < numrects; i++) {
        QRect clip(cliprect[i]);
        if (left <= clip.right() && top <= clip.bottom() && right >= clip.left() && bottom >= clip.top()) {
            int clipLeft = clip.left() > left ? clip.left() : left;
            int clipTop = clip.top() > top ? clip.top() : top;
            int clipRight = clip.right() > right ? right : clip.right();
            int clipBottom = clip.bottom() > bottom ? bottom : clip.bottom();
            int clipSrcLeft = srcLeft + (clipLeft - left);
            int clipSrcTop = srcTop + (clipTop - top);
            draw2d.BitBlt(clipSrcLeft,clipSrcTop,(clipRight-clipLeft)+1,(clipBottom-clipTop)+1,clipLeft,clipTop,mix);
            }
        }

    // Indicate that we have done an accelerated drawing function, so that
    // software rendering code will need to do a sync() before accessing
    // video memory.
    (*gfx_optype) = OPTYPE_ACCEL;

    // Clean up and exit
    GFX_END;
    QWSDisplay::ungrab();
}

/*!
This is called to copy a bitmap from one place to another. We try and perform the blit
in hardware if possible.

// TODO: This function I believe will handle color bitmap blits of
//       different color depths from system memory!! Below are the variables that driver
//       this...
srcbits
srclinestep
srcdepth
srcwidth
srcheight
*/
template<const int depth,const int type>
inline void QGfxSNAP<depth,type>::blt(
    int left,
    int top,
    int width,
    int height,
    int srcLeft,
    int srcTop)
{
    // Check that it is not trivially clipped
    if (ncliprect < 1)
        return;

    // Punt to the scroll function if the blit is within the same surface!
    if (srcbits == buffer) {
        QGfxRaster<depth,type>::scroll(left,top,width,height,srcLeft,srcTop);
        return;
        }

    // Punt to software for stuff we do not support
    if (alphatype != IgnoreAlpha || srctype == SourcePen) {
        QGfxRaster<depth,type>::blt(left,top,width,height,srcLeft,srcTop);
        return;
        }
    if (srcdepth != 8 && srcdepth != 16 && srcdepth != 24 && srcdepth != 32) {
        QGfxRaster<depth,type>::blt(left,top,width,height,srcLeft,srcTop);
        return;
        }

    // Check that the buffers are in hardware, punting to software if not
    if (!checkSourceDest()) {
        QGfxRaster<depth,type>::blt(left,top,width,height,srcLeft,srcTop);
        return;
        }

    // Setup pixel coordinates and loop variables
    left += xoffs;
    top += yoffs;
    srcLeft += srcwidgetoffs.x();
    srcTop += srcwidgetoffs.y();
    int right = left + width - 1;
    int bottom = top + height - 1;
    int mix = mixTable[myrop];
    int i,numrects = ncliprect;

    // Get exclusive write access to the display and get ready for drawing
    QWSDisplay::grab(true);
    GFX_START(QRect(left, top, width+1, height+1));

    // Set the required hardware state if is has changed
    if (memcmp(&cntState->drawBuf,&drawBuf,sizeof(drawBuf)) != 0) {
        cntState->drawBuf = drawBuf;
        state2d.SetDrawBuffer(&drawBuf);
        }

    // Now clip the blit to each clip rectangle do the blit
    for (i = 0; i < numrects; i++) {
        QRect clip(cliprect[i]);
        if (left <= clip.right() && top <= clip.bottom() && right >= clip.left() && bottom >= clip.top()) {
            int so = srcOffset;
            int sl = srclinestep;

            int clipLeft = clip.left() > left ? clip.left() : left;
            int clipTop = clip.top() > top ? clip.top() : top;
            int clipRight = clip.right() > right ? right : clip.right();
            int clipBottom = clip.bottom() > bottom ? bottom : clip.bottom();
            int clipSrcLeft = srcLeft + (clipLeft - left);
            int clipSrcTop = srcTop + (clipTop - top);
            draw2d.BitBltLin(so,sl,clipSrcLeft,clipSrcTop,(clipRight-clipLeft)+1,(clipBottom-clipTop)+1,clipLeft,clipTop,mix);
            }
        }

    // Indicate that we have done an accelerated drawing function, so that
    // software rendering code will need to do a sync() before accessing
    // video memory.
    (*gfx_optype) = OPTYPE_ACCEL;

    // Clean up and exit
    GFX_END;
    QWSDisplay::ungrab();
}

/*!
Function to handle drawing a line from (x1,y1) to (x2,y2)
*/

template<const int depth,const int type>
void QGfxSNAP<depth,type>::drawLine(
    int x1,
    int y1,
    int x2,
    int y2)
{
    // Check to see if we can trivially reject the blit
    if (ncliprect < 1)
        return;

    // We only handle solid lines in here
    if (cpen.style() != SolidLine) {
        QGfxRaster<depth,type>::drawLine(x1,y1,x2,y2);
        return;
        }

    // Check that the buffer is a hardware buffer, punting to software if not
    if (!checkDest()) {
        QGfxRaster<depth,type>::drawLine(x1,y1,x2,y2);
        return;
        }

    // Compute line coordinates and deltas
    x1 += xoffs;
    y1 += yoffs;
    x2 += xoffs;
    y2 += yoffs;
    int dx = abs(x2-x1);
    int dy = abs(y2-y1);

    // Get exclusive write access to the display and get ready for drawing
    QWSDisplay::grab(true);
    GFX_START(QRect(x1, y1 < y2 ? y1 : y2, dx+1, QABS(dy)+1));

    // Set the required hardware state if is has changed
    if (memcmp(&cntState->drawBuf,&drawBuf,sizeof(drawBuf)) != 0) {
        cntState->drawBuf = drawBuf;
        state2d.SetDrawBuffer(&drawBuf);
        }
    if (mixTable[myrop] != cntState->mix)
        state2d.SetMix(cntState->mix = mixTable[myrop]);
    usePen();
    if (pixel != cntState->foreColor)
        state2d.SetForeColor(cntState->foreColor = pixel);

    // Draw the line clipped to all clip rectangles
    int i,numrects = ncliprect;

    for (i = 0; i < numrects; i++) {
        QRect *clip = &cliprect[i];
        draw2d.DrawClippedLineInt(x1,y1,x2,y2,true,clip->left(),clip->top(),clip->right()+1,clip->bottom()+1);
        }

    // Indicate that we have done an accelerated drawing function, so that
    // software rendering code will need to do a sync() before accessing
    // video memory.
    (*gfx_optype) = OPTYPE_ACCEL;

    // Clean up and exit
    GFX_END;
    QWSDisplay::ungrab();
}

/*!
This is called by the software renderer when it's about to draw
something - it needs to be sure that the hardware engine has finished
drawing since otherwise the two graphics operations could collide
*/

template<const int depth,const int type>
void QGfxSNAP<depth,type>::sync()
{
    if (hwState2d.WaitTillIdle)
        hwState2d.WaitTillIdle();
}

/*!
\fn QSNAPScreen::QSNAPScreen( int display_id )
Constructs a QSNAPScreen; passes \a display_id to the QScreen
constructor.
*/

QSNAPScreen::QSNAPScreen( int display_id )
    : QScreen( display_id ), stateBuf(NULL), dc(NULL), unloadRef2d(false)
{
}

/*!
\fn QSNAPScreen::~QSNAPScreen()
Destroys a QSNAPScreen.
*/

QSNAPScreen::~QSNAPScreen()
{
}

/*!
Internal function called to clean up on fatal errors so that the console and
display mode will be properly restored.
*/

void QSNAPScreen::fatalCleanup()
{
    if (stateBuf) {
        PM_restoreConsoleState(stateBuf,hwndConsole);
        PM_closeConsole(hwndConsole);
        free(stateBuf);
        }
    if (dc)
        GA_unloadDriver(dc);
}

/*!
\fn N_uint16 QSNAPScreen::findMode()
Internal function to search for a particular display mode to see if it is
supported by the SNAP drivers.
*/

N_uint16 QSNAPScreen::findMode(
    int x,
    int y,
    int bits)
{
    GA_modeInfo modeInfo;
    N_uint16    *modes;

    // Find the SNAP display mode that matches what we want
    for (modes = dc->AvailableModes; *modes != 0xFFFF; modes++) {
        modeInfo.dwSize = sizeof(modeInfo);
        if (init.GetVideoModeInfo(*modes,&modeInfo) != 0)
            continue;
        if (modeInfo.Attributes & gaIsTextMode)
            continue;
        if ((!modeInfo.Attributes & gaHaveLinearBuffer))
            continue;
        if (modeInfo.BitsPerPixel == 15) {
            // Qt Embedded does not appear to support 15bpp modes!!
            continue;
            }
        if (modeInfo.XResolution == x && modeInfo.YResolution == y && modeInfo.BitsPerPixel == bits)
            return *modes | gaLinearBuffer;
        }
    return 0xFFFF;
}

/*!
Main function to initialise the software rasteriser module and the internal
library helper functions.
*/

int QSNAPScreen::initSoftwareRasterizer(void)
{
    GA_buffer   drawBuf;

    // Load and initialsie the software rasteriser module
    memset(&state2d,0,sizeof(state2d));
    memset(&draw2d,0,sizeof(draw2d));
    unloadRef2d = false;
    if ((ref2d = GA_getCurrentRef2d(dc->DeviceIndex)) == NULL) {
        unloadRef2d = true;
        if (!GA_loadRef2d(dc,true,&modeInfo,-1,&ref2d))
            return false;
        }
    hwState2d.dwSize = sizeof(hwState2d);
    GA_queryFunctions(dc,GA_GET_2DSTATEFUNCS,&hwState2d);
    hwDraw2d.dwSize = sizeof(hwDraw2d);
    GA_queryFunctions(dc,GA_GET_2DRENDERFUNCS,&hwDraw2d);
    driver.dwSize = sizeof(driver);
    REF2D_queryFunctions(ref2d,GA_GET_DRIVERFUNCS,&driver);
    state2d.dwSize = sizeof(state2d);
    REF2D_queryFunctions(ref2d,GA_GET_2DSTATEFUNCS,&state2d);
    draw2d.dwSize = sizeof(draw2d);
    REF2D_queryFunctions(ref2d,GA_GET_2DRENDERFUNCS,&draw2d);

    // Make the primary display active for drawing for the server instance
    if (isServer) {
        drawBuf.dwSize = sizeof(drawBuf);
        drawBuf.Offset = 0;
        drawBuf.Stride = modeInfo.BytesPerScanLine;
        drawBuf.Width  = modeInfo.XResolution;
        drawBuf.Height = modeInfo.YResolution;
        if (ref2d->SetDrawBuffer(&drawBuf,dc->LinearMem,modeInfo.BitsPerPixel,&modeInfo.PixelFormat,dc,false) != 0)
            return false;
        }
    return true;
}

/*!
Sets the framebuffer to a new resolution and bit depth. The width is
in \a nw, the height is in \a nh, and the depth is \a nd. After doing
this any currently-existing gfx's will be invalid and the screen
should be completely redrawn. In a multiple-process Embedded Qt
situation you will need to signal all other applications to do a
setMode and then a redraw.
*/

void QSNAPScreen::setMode(
    int x,
    int y,
    int bits)
{
    // Find the SNAP display mode that matches what we want
    if ((cntMode = findMode(x,y,bits)) == 0xFFFF) {
        fatalCleanup();
        qFatal("Unable to find matching SNAP display mode (%dx%dx%d)!", x,y,bits);
        }

    // Now initialise the display mode, but only do this for the server instance!
    if (isServer) {
        N_int32 virtualX,virtualY,bytesPerLine;
        virtualX = virtualY = bytesPerLine = -1;
        if (init.SetVideoMode(cntMode,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL) != 0) {
            fatalCleanup();
            qFatal("Unable to set SNAP display mode!");
            }
        }

    // Get the current display mode information
    modeInfo.dwSize = sizeof(modeInfo);
    init.GetCurrentVideoModeInfo(&modeInfo);

    // Load and initialsie the software rasteriser module
    if (!initSoftwareRasterizer()) {
        fatalCleanup();
        qFatal("Unable to initialise software rasteriser!");
        }

    // Now read in current color palette and initialize the
    // Qt screen color lookup table.
    if (modeInfo.BitsPerPixel <= 8) {
        GA_palette pal[256];
        screencols = (modeInfo.BitsPerPixel == 8) ? 256 : 16;
        driver.GetPaletteData(pal,screencols,0);
        for (int i = 0; i < screencols; i++)
            screenclut[i] = qRgb(pal[i].Red,pal[i].Green,pal[i].Blue);
        }
    else
        screencols = 0;

    // Tell QScreen where to find the framebuffer. We need to do this again
    // *after* the mode set, as this may well change on some cards.
    mapsize = dc->LinearSize;
    data = (uchar*)dc->LinearMem;

    // Setup internal QScreen variables that are needed
    dw = w = modeInfo.XResolution;
    dh = h = modeInfo.YResolution;
    d = modeInfo.BitsPerPixel;
    lstep = modeInfo.BytesPerScanLine;
    size = h * lstep;
}

/*!
This is called by Qt/Embedded clients to connect to the shared SNAP Graphics driver.
The \a displaySpec parameter is ignored for SNAP Graphics.

This function actually gets called *before* the initDevice() function. For that
reason we do much in the initDevice() function but instead do nearly everything
in here. We first check to see if the shared driver is already loaded, and
if so we just connect to it. If it is not, then we go ahead and do extra
setup stuff that is only done when the Qt Embedded server starts up.
*/

bool QSNAPScreen::connect(
    const QString &)
{
    // If this is the first instance to be loaded, initialise the server and grab
    // the console etc.
    if ((isServer = !GA_isSharedDriverLoaded()) != 0) {
        hwndConsole = PM_openConsole(0,0,640,480,8,true);
        if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
            qFatal("Out of memory!");
        PM_saveConsoleState(stateBuf,hwndConsole);
        }

    // Now attempt to load the SNAP Graphics driver. This will load a shared copy
    // for the second application that is connecting to the server.
    if ((dc = GA_loadDriver(0,true)) == NULL) {
        qDebug("Unable to load the SciTech SNAP driver (%s)", GA_errorMsg(GA_status()));
        return FALSE;
        }
    init.dwSize = sizeof(init);
    if (!GA_queryFunctions(dc,GA_GET_INITFUNCS,&init)) {
        fatalCleanup();
        qFatal("Unable to get device driver functions!");
        }
    driver.dwSize = sizeof(driver);
    if (!GA_queryFunctions(dc,GA_GET_DRIVERFUNCS,&driver)) {
        fatalCleanup();
        qFatal("Unable to get device driver functions!");
        }
    dpms.dwSize = sizeof(dpms);
    if (!GA_queryFunctions(dc,GA_GET_DPMSFUNCS,&dpms)) {
        fatalCleanup();
        qFatal("Unable to get DPMS functions!");
        }

    // Detect if we have DPMS capabilities
    DPMSStates = 0;
    if (dpms.DPMSdetect) {
        dpms.DPMSdetect(&DPMSStates);
        if (!(DPMSStates & DPMS_off))
            DPMSStates = 0;
        }

    // Do extra initialisation specific to the first or server instance
    if (isServer)
        prevMode = init.GetVideoMode();

    // Set the default mode to 1024x768x16, scaling back to 800x600x16 then
    // 640x480x16 and finally 640x480x8. If the size is specified by the
    // QWS_SIZE environment variable, we will use that size instead.
    if (findMode(w = 1024,h = 768,d = 16) == 0xFFFF) {
        if (findMode(w = 800,h = 600,d = 16) == 0xFFFF) {
            if (findMode(w = 640,h = 480,d = 16) == 0xFFFF) {
                if (findMode(w = 640,h = 480,d = 8) == 0xFFFF) {
                    fatalCleanup();
                    qFatal("Unable to find default display mode!");
                    }
                }
            }
        }
    const char* qwssize;
    if ((qwssize = getenv("QWS_SIZE")) != NULL) {
        // First try to get the resolution and color depth, and if that fails
        // just try to get the resolution without the color depth
        if (sscanf(qwssize,"%dx%dx%d",&w,&h,&d) != 3)
            sscanf(qwssize,"%dx%d",&w,&h);
        }

    // Now set the display mode
    setMode(w,h,d);

    // Setup up access to offscreen memory management variables
    setupOffScreen();

    // Return success!
    initted = true;
    return TRUE;
}

/*!
This function is called after the connect() function has been called
but before initDevice(), and allows our driver to steal some shared
memory for our own internal state management code.
*/

int QSNAPScreen::sharedRamSize(
    void *end)
{
    // Allocate a block of shared memory to manage the shared state
    // for all connected processes.
    cntState = (QGfxSNAP_State*)end - 1;

    // Connect up the necessary shared variables that the generic code needs
    optype = &cntState->optype;
    lastop = &cntState->lastop;
    screen_optype = (int*)optype;
    screen_lastop = (int*)lastop;
    return sizeof(QGfxSNAP_State);
}

/*!
\fn bool QSNAPScreen::initDevice()
This is called by the Qt/Embedded server at startup time. We don't
do much in here as we do most of the fun stuff up above in the connect()
function. However we do initialise any shared memory blocks for sharing
information between the server and connecting clients.
*/

bool QSNAPScreen::initDevice()
{
    // Initialise the default color palette
    if (modeInfo.BitsPerPixel <= 8) {
        GA_palette pal[256];
        screencols = (modeInfo.BitsPerPixel == 8) ? 256 : 16;
        if (screencols == 16) {
            // Default 16 colour palette
            uchar reds[16]   = { 0x00,0x7F,0xBF,0xFF,0xFF,0xA2,0x00,0xFF,0xFF,0x00,0x7F,0x7F,0x00,0x00,0x00,0x82 };
            uchar greens[16] = { 0x00,0x7F,0xBF,0xFF,0x00,0xC5,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x7F,0x7F,0x7F };
            uchar blues[16]  = { 0x00,0x7F,0xBF,0xFF,0x00,0x11,0xFF,0x00,0xFF,0xFF,0x00,0x7F,0x7F,0x7F,0x00,0x00 };
            for (int i = 0; i < 16; i++) {
                pal[i].Red = reds[i];
                pal[i].Green = greens[i];
                pal[i].Blue = blues[i];
                screenclut[i] = qRgb(pal[i].Red,pal[i].Green,pal[i].Blue);
                }
            }
        else {
            if (grayscale) {
                // Build grayscale palette
                for (int i = 0; i < screencols; i++) {
                    pal[i].Red = pal[i].Green = pal[i].Blue = i;
                    screenclut[i] = qRgb(pal[i].Red,pal[i].Green,pal[i].Blue);
                    }
                }
            else {
                // 6x6x6 216 color cube
                int i = 0;
                for (int ir = 0x0; ir <= 0xff; ir += 0x33) {
                    for (int ig = 0x0; ig <= 0xff; ig += 0x33) {
                        for (int ib = 0x0; ib <= 0xff; ib += 0x33) {
                            pal[i].Red = ir;
                            pal[i].Green = ig;
                            pal[i].Blue = ib;
                            screenclut[i] = qRgb(pal[i].Red,pal[i].Green,pal[i].Blue);
                            i++;
                            }
                        }
                    }
                // Fill in rest with 0
                for (int j = 0; j < 40; j++) {
                    pal[i].Red = 0;
                    pal[i].Green = 0;
                    pal[i].Blue = 0;
                    screenclut[i] = 0;
                    i++;
                    }
                }
            }
        driver.SetPaletteData(pal,screencols,0,gaDontWait);
        }

    // Initialise the shared state variables
    cntState->optype = 0;
    cntState->lastop = 0;
    cntState->mix = -1;
    cntState->foreColor = 0xFFFFFFFF;
    cntState->backColor = 0xFFFFFFFF;
    memset(&cntState->drawBuf,0,sizeof(cntState->drawBuf));

    // Initialise the offscreen memory manager
    if (useOffscreen) {
        *entryp = 0;
        *lowest = mapsize;
        insert_entry(*entryp,*lowest,*lowest);  // dummy entry to mark start
        }
    initted = true;
    return true;
}

/*!
This is used to initialize the software cursor - \a end_of_location
points to the address after the area where the cursor image can be stored.
\a init is true for the first application this method is called from
(the Qt/Embedded server), false otherwise.
*/

int QSNAPScreen::initCursor(
    void *end_of_location,
    bool init)
{
    // TODO: Add support for hardware cursors!!
    return QScreen::initCursor(end_of_location,init);
}

/*!
\fn void QSNAPScreen::disconnect()
This simply disconnects this client from the shared SNAP Graphics driver.
*/

void QSNAPScreen::disconnect()
{
    // Unload the ref2d and device context as necessary. Ref2d is not
    // reference counted, so we have to specifically unload it only
    // if it was actually loaded. One day we should change this to be
    // reference counted also.
    if (unloadRef2d)
        GA_unloadRef2d(dc);
    GA_unloadDriver(dc);
}

/*!
\fn void QSNAPScreen::shutdownDevice()
This is called by the Qt/Embedded server when it shuts down. Here we
unload the SNAP graphics driver and restore the Linux console back to
normal.
*/

void QSNAPScreen::shutdownDevice()
{
    N_int32 virtualX,virtualY,bytesPerLine;

    // Reset the original display mode before closing the console
    virtualX = virtualY = bytesPerLine = -1;
    init.SetVideoMode(prevMode | gaDontClear,&virtualX,&virtualY,&bytesPerLine,&maxMem,0,NULL);

    // Restore the console state, close the console and free state buffer
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    free(stateBuf);
}

/*!
The offscreen memory manager's list of entries is stored at the bottom
of the offscreen memory area and consistes of a series of QPoolEntry's,
each of which keep track of a block of allocated memory. Unallocated memory
is implicitly indicated by the gap between blocks indicated by QPoolEntry's.
The memory manager looks through any unallocated memory before the end
of currently-allocated memory to see if a new block will fit in the gap;
if it doesn't it allocated it from the end of currently-allocated memory.
Memory is allocated from the top of the framebuffer downwards; if it hits
the list of entries then offscreen memory is full and further allocations
are made from main RAM (and hence unaccelerated). Allocated memory can
be seen as a sort of upside-down stack; lowest keeps track of the
bottom of the stack.
*/

void QSNAPScreen::delete_entry(
    int pos)
{
    if (pos > *entryp || pos < 0) {
        qDebug("Attempt to delete odd pos! %d %d",pos,*entryp);
        return;
        }
#ifdef DEBUG_CACHE
    qDebug( "Remove entry: %d", pos );
#endif
    QPoolEntry * qpe = &entries[pos];
    if(qpe->start <= *lowest) {
        // Lowest goes up again
        *lowest = entries[pos-1].start;
#ifdef DEBUG_CACHE
	qDebug( "   moved lowest to %d", *lowest );
#endif
        }
    (*entryp)--;
    if (pos == *entryp)
        return;
    int size = (*entryp) - pos;
    memmove(&entries[pos], &entries[pos+1], size * sizeof(QPoolEntry));
}

/*!
Insert an entry into the offscreen memory pool.
*/

void QSNAPScreen::insert_entry(
    int pos,
    uint start,
    uint end)
{
    if (pos > *entryp) {
        qDebug("Attempt to insert odd pos! %d %d",pos,*entryp);
        return;
        }
#ifdef DEBUG_CACHE
    qDebug( "Insert entry: %d, %d -> %d", pos, start, end );
#endif
    if ( start < *lowest ) {
	*lowest = start;
#ifdef DEBUG_CACHE
	qDebug( "    moved lowest to %d", *lowest );
#endif
        }
    if (pos == *entryp) {
        entries[pos].start = start;
        entries[pos].end = end;
        (*entryp)++;
        return;
        }
    int size = (*entryp) - pos;
    memmove(&entries[pos+1], &entries[pos], size * sizeof(QPoolEntry));
    entries[pos].start = start;
    entries[pos].end = end;
    (*entryp)++;
}

/*!
Initialise the offscreen memory manager variables.
*/

void QSNAPScreen::setupOffScreen()
{
    // We really should be using the buffer manager instead, since that
    // will allow hardware to be used that does not support linear blits
    // (as well as allow for more ref2d optimisations!). For now we use the
    // current mechanism to get this working.
    //
    // Note also that storing variables in offscreen video memory is a
    // *really*, *really* bad idea. This is because video memory is terribly
    // slow for read operations (usually on the order of 10-20Mb/s even on
    // the fastest AGP cards!), so doing this is a performance killer. We do
    // it for now once again just to get this working properly. Ideally the
    // offscreen memory pool should be managed in an expandable shared
    // system memory block.
    useOffscreen = false;
    if (hwDraw2d.BitBltLin && ((mapsize - size) >= 16384)) {
        // Figure out position of offscreen memory
        // Set up pool entries pointer table and 64-bit align it
        ulong pos = (ulong)data + ((size + 8) & ~0x7);
        entryp = (int*)(pos+0);
        lowest = (uint*)(pos+4);
        entries = (QPoolEntry*)(pos+8);
        cacheStart = pos + 8 + sizeof(QPoolEntry);
        useOffscreen = true;
        }
}

/*!
Requests a block of offscreen graphics card memory from the memory
manager; it will be aligned at pixmapOffsetAlignment(). If no memory
is free 0 will be returned, otherwise a pointer to the data within
the framebuffer. QScreen::onCard can be used to retrieve a byte offset
from the start of graphics card memory from this pointer. The display
is locked while memory is allocated and unallocated in order to
preserve the memory pool's integrity, so cache and uncache should not
be called if the screen is locked.

\a amount is the amount of memory to allocate, \a optim gives the optimization
level (same values as QPixmap::Optimization).
*/

uchar * QSNAPScreen::cache(
    int amount,
    int optim)
{
    // Check to see if we should cache the bitmap in memory
    if (!useOffscreen || entryp == 0 || optim == int(QPixmap::NoOptim) )
        return NULL;

    // Get exclusive access to video memory
    qt_fbdpy->grab(true);

    // Check that we have enough room to allocate a new block
    uint startp = cacheStart + (*entryp+1) * sizeof(QPoolEntry);
    if (startp >= *lowest) {
#ifdef DEBUG_CACHE
	qDebug( "No room for pool entry in VRAM" );
#endif
	qt_fbdpy->ungrab();
	return NULL;
        }

    // Get pixemap offset alignemtn in bytes
    int byteAlign = pixmapOffsetAlignment() / 8;

    // Try to find a gap in the allocated blocks.
    int hold = (*entryp-1);
    for (int i = 0; i < hold-1; i++) {
        int freestart = entries[i+1].end;
        int freeend = entries[i].start;
        if (freestart != freeend) {
            while (freestart % byteAlign)
                freestart++;
            int len = freeend - freestart;
            if (len >= amount) {
                insert_entry(i+1, freestart, freestart+amount);
                qt_fbdpy->ungrab();
                return data + freestart;
                }
            }
        }


    // No free blocks in already-taken memory; get some more
    // if we can
    uint newlowest = (*lowest) - amount;
    if (newlowest % byteAlign) {
        newlowest -= byteAlign;
        while (newlowest % byteAlign)
            newlowest++;
        }
    if (startp >= newlowest) {
        qt_fbdpy->ungrab();
#ifdef DEBUG_CACHE
	qDebug( "No VRAM available for %d bytes", amount);
#endif
        return NULL;
        }
    insert_entry(*entryp, newlowest, *lowest);
    qt_fbdpy->ungrab();
    return data + newlowest;
}

/*!
Delete a block of memory \a c allocated from graphics card memory.
*/

void QSNAPScreen::uncache(
    uchar *c)
{
    // Get exclusive access to video memory
    qt_fbdpy->grab(true);
    hwState2d.WaitTillIdle();
    ulong pos = (ulong)c;
    pos -= ((ulong)data);
    uint hold = (*entryp);
    for (uint i = 0; i < hold; i++) {
        if (entries[i].start == pos) {
            delete_entry(i);
            qt_fbdpy->ungrab();
            return;
            }
        }
    qt_fbdpy->ungrab();
    qDebug("Attempt to delete unknown offset %ld",pos);
}

/*!
\fn QGfx * QSNAPScreen::createGfx
This is called to create the graphics rendering object for the
specific resolution and color depth.
*/

QGfx * QSNAPScreen::createGfx(
    unsigned char * b,
    int w,
    int h,
    int d,
    int linestep)
{
    QGfx * ret = NULL;
    if (onCard(b)) {
        switch (d) {
#ifndef QT_NO_QWS_DEPTH_8
            case 8:
                ret = new QGfxSNAP<8,0>(b,w,h,cntState,hwState2d,hwDraw2d,state2d,draw2d);
                break;
#endif
#ifndef QT_NO_QWS_DEPTH_16
            case 16:
                ret = new QGfxSNAP<16,0>(b,w,h,cntState,hwState2d,hwDraw2d,state2d,draw2d);
                break;
#endif
#ifndef QT_NO_QWS_DEPTH_24
            case 24:
                ret = new QGfxSNAP<24,0>(b,w,h,cntState,hwState2d,hwDraw2d,state2d,draw2d);
                break;
#endif
#ifndef QT_NO_QWS_DEPTH_32
            case 32:
                ret = new QGfxSNAP<32,0>(b,w,h,cntState,hwState2d,hwDraw2d,state2d,draw2d);
                break;
#endif
            }
        if (ret) {
            ret->setLineStep(linestep);
            return ret;
            }
        }
    return QScreen::createGfx(b,w,h,d,linestep);
}

/*!
\fn void QSNAPScreen::save()
Save the state of the graphics card. It's called by the Qt/Embedded server
when the virtual console is switched.
*/

void QSNAPScreen::save()
{
    // TODO: We may need to do stuff in here to save the screen
}

/*!
\fn void QSNAPScreen::restore()
This is called when the virtual console is switched back to Qt/Embedded
and restores the graphics screen.
*/

void QSNAPScreen::restore()
{
    // TODO: We may need to do stuff in here to restore the screen
}

/*!
\fn void QSNAPScreen::blank()
This is to power down the display monitor using DPMS.
*/

void QSNAPScreen::blank(
    bool on)
{
    if (DPMSStates)
        dpms.DPMSsetState(on ? DPMS_on : DPMS_off);
}

/*!
\fn void QSNAPScreen::set()
In paletted graphics modes, this sets color index \a i to the specified RGB
value, (\a r, \a g, \a b).
*/

void QSNAPScreen::set(
    unsigned int i,
    unsigned int r,
    unsigned int g,
    unsigned int b)
{
    GA_palette pal;

    pal.Red = r;
    pal.Green = g;
    pal.Blue = b;
    driver.SetPaletteData(&pal,1,i,false);
    screenclut[i] = qRgb( r, g, b );
}

#endif // QT_NO_QWS_SNAP

