/****************************************************************************
**
** Definition of Qt/Embedded mouse driver
**
** Created : 20020220
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

#ifndef QMOUSE_QWS_H
#define QMOUSE_QWS_H

#ifndef QT_H
#include "qobject.h"
#include "qpointarray.h"
#endif // QT_H

class QWSPointerCalibrationData
{
public:
    enum Location { TopLeft = 0, BottomLeft = 1, BottomRight = 2, TopRight = 3,
		    Center = 4, LastLocation = Center };
    QPoint devPoints[5];
    QPoint screenPoints[5];
};

class QWSMouseHandler
{
public:
    QWSMouseHandler( const QString &driver = QString::null, const QString &device = QString::null );
    virtual ~QWSMouseHandler();

    virtual void clearCalibration() {}
    virtual void calibrate( QWSPointerCalibrationData * ) {}
    virtual void getCalibration( QWSPointerCalibrationData * ) {}

    void limitToScreen( QPoint &pt );
    void mouseChanged(const QPoint& pos, int bstate);
    const QPoint &pos() const { return mousePos; }

protected:
    QPoint &mousePos;
};


class QWSCalibratedMouseHandler : public QWSMouseHandler
{
public:
    QWSCalibratedMouseHandler( const QString &driver = QString::null, const QString &device = QString::null );

    virtual void clearCalibration();
    virtual void calibrate( QWSPointerCalibrationData * );
    virtual void getCalibration( QWSPointerCalibrationData * );

    bool sendFiltered( const QPoint &, int button );
    QPoint transform( const QPoint & );

protected:
    void readCalibration();
    void writeCalibration();
    void setFilterSize( int );

private:
    int a, b, c;
    int d, e, f;
    int s;
    QPointArray samples;
    unsigned int currSample;
    unsigned int numSamples;
};

#endif // QMOUSE_QWS_H
