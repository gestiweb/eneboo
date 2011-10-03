/****************************************************************************
** $Id: qt/qmacstyle_mac.h   3.3.8   edited Jan 11 14:46 $
**
** Definition of ...
**
** Created : 981231
**
** Copyright (C) 1998-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech ASA of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QMACSTYLE_MAC_H
#define QMACSTYLE_MAC_H

#ifndef QT_H
#include "qwindowsstyle.h"
#endif // QT_H

#if defined( Q_WS_MAC ) && !defined( QT_NO_STYLE_MAC ) 

class QPalette;

#if defined(QT_PLUGIN)
#define Q_EXPORT_STYLE_MAC
#else
#define Q_EXPORT_STYLE_MAC Q_EXPORT
#endif

class QMacStylePrivate;

class Q_EXPORT_STYLE_MAC QMacStyle : public QWindowsStyle
{
    Q_OBJECT
public:
    QMacStyle( );
    virtual ~QMacStyle();

    void polish( QWidget * w );
    void unPolish( QWidget * w );
    void polish( QApplication* );

    void drawItem( QPainter *p, const QRect &r,
		   int flags, const QColorGroup &g, bool enabled,
		   const QPixmap *pixmap, const QString &text,
		   int len = -1, const QColor *penColor = 0 ) const;

    void drawPrimitive( PrimitiveElement pe,
			QPainter *p,
			const QRect &r,
			const QColorGroup &cg,
			SFlags flags = Style_Default,
			const QStyleOption& = QStyleOption::Default ) const;

    void drawControl( ControlElement element,
		      QPainter *p,
		      const QWidget *widget,
		      const QRect &r,
		      const QColorGroup &cg,
		      SFlags how = Style_Default,
		      const QStyleOption& = QStyleOption::Default ) const;

    void drawComplexControl( ComplexControl control,
			     QPainter* p,
			     const QWidget* w,
			     const QRect& r,
			     const QColorGroup& cg,
			     SFlags flags = Style_Default,
			     SCFlags sub = SC_None,
			     SCFlags subActive = SC_None,
			     const QStyleOption& = QStyleOption::Default ) const;


    int pixelMetric( PixelMetric metric,
		     const QWidget *widget = 0 ) const;


    virtual QRect querySubControlMetrics( ComplexControl control,
					  const QWidget *w,
					  SubControl sc,
					  const QStyleOption& = QStyleOption::Default ) const;

    virtual QRect subRect( SubRect, const QWidget *w ) const;

    SubControl querySubControl( ComplexControl control,
				const QWidget *widget,
				const QPoint &pos,
				const QStyleOption& = QStyleOption::Default ) const;

    virtual int styleHint(StyleHint sh, const QWidget *, const QStyleOption &, 
			  QStyleHintReturn *) const;

    QSize sizeFromContents( ContentsType contents,
			    const QWidget *w,
			    const QSize &contentsSize,
			    const QStyleOption& = QStyleOption::Default ) const;

    enum FocusRectPolicy { FocusEnabled, FocusDisabled, FocusDefault };
    static void setFocusRectPolicy( QWidget *w, FocusRectPolicy policy);
    static FocusRectPolicy focusRectPolicy( QWidget *w );

    enum WidgetSizePolicy { SizeSmall, SizeLarge, SizeNone, SizeDefault };
    static void setWidgetSizePolicy( QWidget *w, WidgetSizePolicy policy);
    static WidgetSizePolicy widgetSizePolicy( QWidget *w );

protected:
    bool event(QEvent *);

private:        // Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QMacStyle( const QMacStyle & );
    QMacStyle& operator=( const QMacStyle & );
#endif

protected:
    QMacStylePrivate *d;
};

#endif // Q_WS_MAC

#endif // QMACSTYLE_H
