/****************************************************************************
**
** Definition of QWSManager class. This manages QWS top-level windows.
**
** Created : 20000308
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

#ifndef QWSMANAGER_QWS_H
#define QWSMANAGER_QWS_H

#ifndef QT_H
#include "qpixmap.h"
#include "qobject.h"
#include "qwsdecoration_qws.h"
#endif // QT_H

#ifndef QT_NO_QWS_MANAGER

class QPixmap;
class QWidget;
class QPopupMenu;
class QRegion;
class QMouseEvent;
class QWSButton;
class QWSManager;

class QWSManager : public QObject
{
    Q_OBJECT

public:
    QWSManager(QWidget *);
    ~QWSManager();

    static QWSDecoration *newDefaultDecoration();

    QRegion region();
    QWidget *widget() { return managed; }
    void maximize();

    static QWidget *grabbedMouse() { return active; }

protected slots:
    void menuActivated(int);
    void styleMenuActivated(int);

protected:
    void handleMove();
    virtual QWSDecoration::Region pointInRegion(const QPoint &);

    virtual bool event(QEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *) {}
    virtual void paintEvent(QPaintEvent *);

    void menu(const QPoint &);
    void close();
    void minimize();
    void toggleMaximize();

    QWSDecoration::Region activeRegion;
    QWidget *managed;
    QPopupMenu *popup;
    QRect   normalSize;
    QWSButton *menuBtn;
    QWSButton *closeBtn;
    QWSButton *minimizeBtn;
    QWSButton *maximizeBtn;

    int dx;
    int dy;
    int skipCount;

    static QWidget *active;
    static QPoint mousePos;
private:
    friend class QWidget;
    inline void releaseGrab() { active = 0; activeRegion = QWSDecoration::None; }
};

class QWSButton
{
public:
    QWSButton(QWSManager *m, QWSDecoration::Region t, bool tb = false);

    enum State { MouseOver = 0x01, Clicked = 0x02, On = 0x04 };
    int state() { return flags; }
    void setMouseOver(bool);
    void setClicked(bool);
    void setOn(bool);

protected:
    void paint();

private:
    int  flags;
    bool toggle;
    QWSDecoration::Region type;
    QWSManager *manager;
};

// class QWSDefaultDecoration : public QWSDecoration;
#include "qwsdefaultdecoration_qws.h"

#endif // QT_NO_QWS_MANAGER

#endif // QWSMANAGER_QWS_H
