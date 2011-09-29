/****************************************************************************
** $Id: qt/qgl_mac.cpp   3.3.8   edited Jan 11 16:03 $
**
** Implementation of OpenGL classes for Qt
**
** Created : 970112
**
** Copyright (C) 1992-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the opengl module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Macintosh may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qgl.h"

#if defined(Q_WS_MAC)
#include <AGL/agl.h>
#include <AGL/aglRenderers.h>
#include <OpenGL/gl.h>

#include <private/qfontdata_p.h>
#include <private/qfontengine_p.h>
#include <qt_mac.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qintdict.h>
#if !defined(QMAC_OPENGL_DOUBLEBUFFER)
#include <qobjectlist.h>
#endif

/*****************************************************************************
  QGLFormat UNIX/AGL-specific code
 *****************************************************************************/
bool QGLFormat::hasOpenGL()
{
    return TRUE;
}


bool QGLFormat::hasOpenGLOverlays()
{
    return TRUE;
}



/*****************************************************************************
  QGLContext AGL-specific code
 *****************************************************************************/
void qgl_delete_d(const QGLWidget *); //qgl.cpp
QPoint posInWindow(QWidget *); //qwidget_mac.cpp
bool QGLContext::chooseContext(const QGLContext* shareContext)
{
    cx = NULL;
    GDHandle dev = GetMainDevice(); //doesn't handle multiple heads, fixme!
    vi = chooseMacVisual(dev);
    if(!vi)
	return FALSE;

    AGLPixelFormat fmt = (AGLPixelFormat)vi;
    GLint res;
    aglDescribePixelFormat(fmt, AGL_LEVEL, &res);
    glFormat.setPlane(res);
    if(deviceIsPixmap())
	res = 0;
    else
	aglDescribePixelFormat(fmt, AGL_DOUBLEBUFFER, &res);
    glFormat.setDoubleBuffer(res);
    aglDescribePixelFormat(fmt, AGL_DEPTH_SIZE, &res);
    glFormat.setDepth(res);
    aglDescribePixelFormat(fmt, AGL_RGBA, &res);
    glFormat.setRgba(res);
    aglDescribePixelFormat(fmt, AGL_ALPHA_SIZE, &res);
    glFormat.setAlpha(res);
    aglDescribePixelFormat(fmt, AGL_ACCUM_RED_SIZE, &res);
    glFormat.setAccum(res);
    aglDescribePixelFormat(fmt, AGL_STENCIL_SIZE, &res);
    glFormat.setStencil(res);
    aglDescribePixelFormat(fmt, AGL_STEREO, &res);
    glFormat.setStereo(res);

    if(shareContext && (!shareContext->isValid() || !shareContext->cx)) {
#if defined(QT_CHECK_NULL)
	    qWarning("QGLContext::chooseContext(): Cannot share with invalid context");
#endif
	    shareContext = 0;
    }

    // sharing between rgba and color-index will give wrong colors
    if(shareContext && ( format().rgba() != shareContext->format().rgba()))
	shareContext = 0;
    AGLContext ctx = aglCreateContext(fmt, (AGLContext) (shareContext ? shareContext->cx : NULL));
    if(!ctx) {
	GLenum err = aglGetError();
	if(err == AGL_BAD_MATCH || err == AGL_BAD_CONTEXT) {
	    if(shareContext && shareContext->cx) {
		qWarning("QOpenGL: context sharing mismatch!");
		if(!(ctx = aglCreateContext(fmt, NULL)))
		    return FALSE;
		shareContext = NULL;
	    }
	}
	if(!ctx) {
	    qDebug("QOpenGL: unable to create QGLContext");
	    return FALSE;
	}
    }
    d->sharing = shareContext && shareContext->cx;

    if((cx = (void *)ctx)) {
#ifdef QMAC_ONE_PIXEL_LOCK
	if(deviceIsPixmap()) {
	    QPixmap *pm = (QPixmap *)d->paintDevice;
	    PixMapHandle mac_pm = GetGWorldPixMap((GWorldPtr)pm->handle());
	    aglSetOffScreen(ctx, pm->width(), pm->height(),
			    GetPixRowBytes(mac_pm), GetPixBaseAddr(mac_pm));
#else
#error "Not ready to handle that case, tror jeg!"
#endif
	} else {
	    aglSetDrawable(ctx, GetWindowPort((WindowPtr)d->paintDevice->handle()));
	}

        { //sync the vrefresh
            const GLint sync = 1;
            aglSetInteger((AGLContext)cx, AGL_SWAP_INTERVAL, &sync);
            if(!aglIsEnabled((AGLContext)cx, AGL_SWAP_INTERVAL))
                aglEnable((AGLContext)cx, AGL_SWAP_INTERVAL);
        }
	return TRUE;
    }
    return FALSE;
}


/*!
  \fn void *QGLContext::chooseMacVisual(GDHandle device)

  <strong>Mac only</strong>: This virtual function tries to find a
  visual that matches the format using the given \a device handle,
  reducing the demands if the original request cannot be met.

  The algorithm for reducing the demands of the format is quite
  simple-minded, so override this method in your subclass if your
  application has specific requirements on visual selection.

  \sa chooseContext()
*/

void *QGLContext::chooseMacVisual(GDHandle)
{
    GLint attribs[20], cnt=0;
    if(deviceIsPixmap()) {
	attribs[cnt++] = AGL_PIXEL_SIZE;
	attribs[cnt++] = ((QPixmap*)d->paintDevice)->depth();
    }
    if(glFormat.rgba()) {
	attribs[cnt++] = AGL_RGBA;
	attribs[cnt++] = AGL_DEPTH_SIZE;
	attribs[cnt++] = deviceIsPixmap() ? ((QPixmap*)d->paintDevice)->depth() : 32;
    } else {
	attribs[cnt++] = AGL_DEPTH_SIZE;
	attribs[cnt++] = 8;
    }
    if(glFormat.alpha()) {
	attribs[cnt++] = AGL_ALPHA_SIZE;
	attribs[cnt++] = 8;
    }
    if(glFormat.stencil()) {
	attribs[cnt++] = AGL_STENCIL_SIZE;
	attribs[cnt++] = 4;
    }
    if(deviceIsPixmap()) {
	attribs[cnt++] = AGL_OFFSCREEN;
    } else {
	if( glFormat.doubleBuffer())
	    attribs[cnt++] = AGL_DOUBLEBUFFER;
//	attribs[cnt++] = AGL_ACCELERATED;
    }
    if(glFormat.stereo())
	attribs[cnt++] = AGL_STEREO;

    attribs[cnt] = AGL_NONE;
    AGLPixelFormat fmt;
    fmt = aglChoosePixelFormat(NULL, 0, attribs);
    if(!fmt && glFormat.stereo()) {
	// not commonly supported, try a fall-back
	attribs[--cnt] = AGL_NONE;
	fmt = aglChoosePixelFormat(NULL, 0, attribs);
    }
    if(!fmt) {
	GLenum err = aglGetError();
	qWarning("QGLContext: unable to get a pixel format (error %d)", (int)err);
    }
#if 0
    else {
	GLint res;
	int x = 0;
	for( AGLPixelFormat fmt2 = fmt; fmt2; fmt2 = aglNextPixelFormat(fmt2)) {
	    aglDescribePixelFormat( fmt2, AGL_RENDERER_ID, &res);
	    GLint res2;
	    aglDescribePixelFormat( fmt2, AGL_ACCELERATED, &res2);
	    qDebug("%d) 0x%08x 0x%08x %d", x++, (int)res, (int)AGL_RENDERER_GENERIC_ID, (int)res2);
	}
    }
#endif
    return fmt;
}

void QGLContext::reset()
{
    if( !d->valid)
	return;
    if(cx)
	aglDestroyContext( (AGLContext)cx);
    cx = 0;
    if( vi)
	aglDestroyPixelFormat((AGLPixelFormat)vi);
    vi = 0;
    d->oldR = QRect(1, 1, 1, 1);
    d->crWin = FALSE;
    d->sharing = FALSE;
    d->valid = FALSE;
    d->transpColor = QColor();
    d->initDone = FALSE;
}

void QGLContext::makeCurrent()
{
    if( !d->valid) {
#if defined(QT_CHECK_STATE)
	qWarning("QGLContext::makeCurrent(): Cannot make invalid context current.");
#endif
	return;
    }

    aglSetCurrentContext((AGLContext)cx);
    fixBufferRect();
    aglUpdateContext((AGLContext)cx);
    currentCtx = this;
}

void QGLContext::fixBufferRect()
{
    if(d->paintDevice->devType() == QInternal::Widget &&
       !((QWidget*)d->paintDevice)->isTopLevel()) {
	if(!aglIsEnabled((AGLContext)cx, AGL_BUFFER_RECT))
	   aglEnable((AGLContext)cx, AGL_BUFFER_RECT);

	bool update = FALSE;
	QWidget *w = (QWidget *)d->paintDevice;
	QRegion clp = w->clippedRegion();
	const QRect clp_r = clp.boundingRect();
	if(clp_r.isEmpty()) {
#if QT_MACOSX_VERSION >= 0x1020
	    if(aglIsEnabled((AGLContext)cx, AGL_CLIP_REGION))
		aglDisable((AGLContext)cx, AGL_CLIP_REGION);
#endif
	    if(!d->oldR.isNull()) {
		update = TRUE;
		d->oldR = clp_r;
		GLint offs[4] = { 0, 0, 0, 0 };
		aglSetInteger((AGLContext)cx, AGL_BUFFER_RECT, offs);
	    }
	} else {
	    QPoint mp(posInWindow(w));
	    int window_height = w->topLevelWidget()->height();
	    window_height -= window_height - w->topLevelWidget()->clippedRegion(FALSE).boundingRect().height(); //mask?
	    QRect newr = QRect(mp.x(), window_height - (mp.y() + w->height()),
			       w->width(), w->height());
	    if(d->oldR != newr) {
		update = TRUE;
		d->oldR = newr;
		GLint offs[4] = { newr.x(), newr.y(), newr.width(), newr.height() };
		aglSetInteger((AGLContext)cx, AGL_BUFFER_RECT, offs);
	    }
#if QT_MACOSX_VERSION >= 0x1020
	    update = TRUE;
	    if(!aglIsEnabled((AGLContext)cx, AGL_CLIP_REGION))
		aglEnable((AGLContext)cx, AGL_CLIP_REGION);
	    aglSetInteger((AGLContext)cx, AGL_CLIP_REGION, (const GLint *)clp.handle(TRUE));
#endif
	}
	if(update) {
	    aglUpdateContext((AGLContext)cx);
	    QMacSavedPortInfo::flush(w);
	}
    }
}

void QGLContext::doneCurrent()
{
    if( aglGetCurrentContext() != (AGLContext) cx)
	return;
    currentCtx = 0;
    aglSetCurrentContext(NULL);
}


void QGLContext::swapBuffers() const
{
    if( !d->valid)
	return;
    aglSwapBuffers((AGLContext)cx);
}

QColor QGLContext::overlayTransparentColor() const
{
    return QColor();		// Invalid color
}

static QColor cmap[256];
static bool cmap_init = FALSE;
uint QGLContext::colorIndex( const QColor&c) const
{
    int ret = -1;
    if(!cmap_init) {
	cmap_init = TRUE;
	for(int i = 0; i < 256; i++)
	    cmap[i] = QColor();
    } else {
	for(int i = 0; i < 256; i++) {
	    if(cmap[i].isValid() && cmap[i] == c) {
		ret = i;
		break;
	    }
	}
    }
    if(ret == -1) {
	for(ret = 0; ret < 256; ret++)
	    if(!cmap[ret].isValid())
		break;
	if(ret == 256) {
	    ret = -1;
	    qDebug("whoa, that's no good..");
	} else {
	    cmap[ret] = c;

	    GLint vals[4];
	    vals[0] = ret;
	    vals[1] = c.red();
	    vals[2] = c.green();
	    vals[3] = c.blue();
	    aglSetInteger((AGLContext)cx, AGL_COLORMAP_ENTRY, vals);
	}
    }
    return (uint)(ret == -1 ? 0 : ret);
}

void QGLContext::generateFontDisplayLists(const QFont & fnt, int listBase)
{
    Style fstyle = normal; //from MacTypes.h
    if(fnt.bold())
	fstyle |= bold;
    if(fnt.italic())
	fstyle |= italic;
    if(fnt.underline())
	fstyle |= underline;
    Str255 name;
    FMGetFontFamilyName((FMFontFamily)((UInt32)fnt.handle()), name);
    short fnum;
    GetFNum(name, &fnum);
    aglUseFont((AGLContext) cx, (int)fnum, fstyle, QFontInfo(fnt).pointSize(), 0, 256, listBase);
}

/*****************************************************************************
  QGLWidget AGL-specific code
 *****************************************************************************/

void QGLWidget::init(QGLContext *ctx, const QGLWidget* shareWidget)
{
    slcx = glcx = 0;
    autoSwap = TRUE;

    gl_pix = NULL;
    req_format = ctx->format();
    pending_fix = 0;
#if defined(QMAC_OPENGL_DOUBLEBUFFER)
    dblbuf = QMAC_OPENGL_DOUBLEBUFFER;
#else
    dblbuf = 1;
#endif

    glcx_dblbuf = 2;
    clp_serial = 0;
    macInternalDoubleBuffer(FALSE); //just get things going
    macInternalRecreateContext(ctx, shareWidget ? shareWidget->context() : NULL, FALSE);

    if(isValid() && context()->format().hasOverlay()) {
	olcx = new QGLContext(QGLFormat::defaultOverlayFormat(), this);
        if(!olcx->create(shareWidget ? shareWidget->overlayContext() : 0)) {
	    delete olcx;
	    olcx = 0;
	    glcx->glFormat.setOverlay(FALSE);
	}
    }
    else {
	olcx = 0;
    }
}


void QGLWidget::reparent(QWidget* parent, WFlags f, const QPoint& p,
			  bool showIt)
{
    QWidget::reparent(parent, f, p, showIt);
#if 0
    fixReparented();
    if(showIt)
	show();
#endif
}

void QGLWidget::macWidgetChangedWindow()
{
    if(!macInternalDoubleBuffer(FALSE)) {
        if(glcx) {
	    aglSetDrawable((AGLContext)glcx->cx, GetWindowPort((WindowPtr)handle()));
            glcx->fixBufferRect();
        }
    }
}

void QGLWidget::setMouseTracking(bool enable)
{
    QWidget::setMouseTracking(enable);
}


void QGLWidget::resizeEvent(QResizeEvent *)
{
    if(!isValid())
	return;
    makeCurrent();
    if(!glcx->initialized())
	glInit();
    aglUpdateContext((AGLContext)glcx->cx);
    resizeGL(width(), height());

    if(olcx) {
	makeOverlayCurrent();
	resizeOverlayGL(width(), height());
    }
}

const QGLContext* QGLWidget::overlayContext() const
{
    return olcx;
}


void QGLWidget::makeOverlayCurrent()
{
    if(olcx) {
	olcx->makeCurrent();
	if(!olcx->initialized()) {
	    initializeOverlayGL();
	    olcx->setInitialized(TRUE);
	}
    }
}


void QGLWidget::updateOverlayGL()
{
    if(olcx) {
	makeOverlayCurrent();
	paintOverlayGL();
	if(olcx->format().doubleBuffer()) {
	    if(autoSwap)
		olcx->swapBuffers();
	}
	else {
	    glFlush();
	}
    }
}

void QGLWidget::setContext(QGLContext *context,
			    const QGLContext* shareContext,
			    bool deleteOldContext)
{
    if(context == 0) {
#if defined(QT_CHECK_NULL)
	qWarning("QGLWidget::setContext: Cannot set null context");
#endif
	return;
    }

    const QPaintDevice *me = this;
    if(macInternalDoubleBuffer()) {
	me = gl_pix;
    } else if(!context->deviceIsPixmap() && context->device() != me) {
#if defined(QT_CHECK_STATE)
	qWarning("QGLWidget::setContext: Context must refer to this widget");
#endif
	return;
    }

    if(glcx)
	glcx->doneCurrent();
    QGLContext* oldcx = glcx;
    glcx = context;
    slcx = shareContext;
    glcx_dblbuf = dblbuf;

    if(!glcx->isValid()) {
	const QGLContext *share = shareContext;
#if 0
	if(!share && !deleteOldContext)
	    share = oldcx;
#endif
	glcx->create(share);
    }
    if(deleteOldContext)
	delete oldcx;
}

bool QGLWidget::renderCxPm(QPixmap*)
{
    return FALSE;
}

const QGLColormap & QGLWidget::colormap() const
{
    return cmap;
}

void QGLWidget::setColormap(const QGLColormap &)
{
}

void QGLWidget::cleanupColormaps()
{
}

bool QGLWidget::macInternalDoubleBuffer(bool fix)
{
#if !defined(QMAC_OPENGL_DOUBLEBUFFER)
    bool need_fix = FALSE;
    if(isTopLevel()) {
	dblbuf = 0;
	if(clippedSerial() != clp_serial) {
	    if(children()) {
		QRect myrect(rect());
		register QObject *obj;
		for(QObjectListIt it(*children()); (obj = it.current()); ++it) {
		    if(obj->isWidgetType()) {
			QWidget *w = (QWidget*)obj;
			if(w->isVisible() && !w->isTopLevel() && myrect.intersects(QRect(w->pos(), w->size()))) {
			    dblbuf = 1;
			    break;
			}
		    }
		}
	    }
	    clp_serial = clippedSerial();
        }
    } else if(clippedSerial() != clp_serial) {
	QRegion rgn = clippedRegion();
	clp_serial = clippedSerial();
	if(rgn.isNull() || rgn.isEmpty()) { //don't double buffer, we'll just make the area empty
	    dblbuf = 0;
	} else {
	    QRect rct(posInWindow(this), size());
	    if(!isTopLevel())
		rct &= topLevelWidget()->rect();
	    dblbuf = (rgn != QRegion(rct));
	}
    }
    if(glcx_dblbuf != dblbuf)
	need_fix = TRUE;
    else if(dblbuf && (!gl_pix || gl_pix->size() != size()))
	need_fix = TRUE;
    if(pending_fix || need_fix) {
	if(fix)
	    macInternalRecreateContext(new QGLContext(req_format,this));
	else
	    pending_fix = TRUE;
    }
#else
    Q_UNUSED(fix);
#endif
    return (bool)dblbuf;
}

void QGLWidget::macInternalRecreateContext(QGLContext *ctx, const QGLContext *share_ctx,
					   bool update)
{
    if(QApplication::closingDown())
	return;
    if (!ctx->device())
	ctx->setDevice(this);
    if(glcx && QMacBlockingFunction::blocking()) { //nah, let's do it "later"
	if(!dblbuf) {
	    glcx->fixBufferRect();
	} else if(gl_pix && gl_pix->size() != size()) {
	    aglSetDrawable((AGLContext)glcx->cx, NULL);
	    gl_pix->resize(size());
	    PixMapHandle mac_pm = GetGWorldPixMap((GWorldPtr)gl_pix->handle());
	    aglSetOffScreen((AGLContext)glcx->cx, gl_pix->width(), gl_pix->height(),
			    GetPixRowBytes(mac_pm), GetPixBaseAddr(mac_pm));
	}
	pending_fix = TRUE;
	return;
    }

    QGLContext* oldcx = glcx;
    glcx_dblbuf = dblbuf;
    pending_fix = FALSE;
    if(dblbuf) {
	setBackgroundMode(NoBackground);
	if(gl_pix && glcx_dblbuf == dblbuf) { //currently double buffered, just resize
	    int w = width(), h = height();
	    if(gl_pix->size() != size()) {
		aglSetDrawable((AGLContext)glcx->cx, NULL);
		gl_pix->resize(w, h);
		PixMapHandle mac_pm = GetGWorldPixMap((GWorldPtr)gl_pix->handle());
		aglSetOffScreen((AGLContext)glcx->cx, gl_pix->width(), gl_pix->height(),
				GetPixRowBytes(mac_pm), GetPixBaseAddr(mac_pm));
	    }
	} else {
	    if(gl_pix && glcx && glcx->cx) {
		aglSetDrawable((AGLContext)glcx->cx, NULL);
		delete gl_pix;
	    }
	    gl_pix = new QPixmap(width(), height(), QPixmap::BestOptim);
	    if(oldcx)
		qgl_delete_d(this);
	    setContext(ctx, share_ctx ? share_ctx : slcx, FALSE);
	}
    } else {
	setEraseColor(black);
	if(oldcx)
	    qgl_delete_d(this);
	setContext(ctx, share_ctx ? share_ctx : slcx, FALSE);
	glcx->fixBufferRect();
    }
    if(update)
	repaint();
    if(oldcx && oldcx != glcx)
	delete oldcx;
}

void QGLWidget::setRegionDirty(bool b) //Internally we must put this off until "later"
{
    QWidget::setRegionDirty(b);
    QTimer::singleShot(1, this, SLOT(macInternalFixBufferRect()));
}

void QGLWidget::macInternalFixBufferRect()
{
    glcx->fixBufferRect();
    update();
}
#endif
