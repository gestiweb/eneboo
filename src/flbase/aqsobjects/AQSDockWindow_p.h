/***************************************************************************
 AQSDockWindow_p.h
 -------------------
 begin                : 02/04/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
 email                : mail@infosial.com
 ***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
 Este  programa es software libre. Puede redistribuirlo y/o modificarlo
 bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
 versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#ifndef AQSDOCKWINDOW_P_H_
#define AQSDOCKWINDOW_P_H_

#include "AQSFrame_p.h"
#include <qdockwindow.h>

class AQSDockWindow : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(DockWindow, Frame);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setWidget(QWidget *);
  QWidget *widget() const;
  QDockArea *area() const;
  uint place() const;
  virtual void setCloseMode(int);
  bool isCloseEnabled() const;
  virtual void setResizeEnabled(bool);
  virtual void setMovingEnabled(bool);
  bool isResizeEnabled() const;
  bool isMovingEnabled() const;
  virtual void setHorizontallyStretchable(bool);
  virtual void setVerticallyStretchable(bool);
  bool isHorizontallyStretchable() const;
  bool isVerticallyStretchable() const;
  bool isStretchable() const;
  virtual void setOffset(int);
  virtual void setFixedExtentWidth(int);
  virtual void setFixedExtentHeight(int);
  QSize fixedExtent() const;
  virtual void setNewLine(bool);
  bool newLine() const;
  uint orientation() const;
  QBoxLayout *boxLayout();
  virtual void setOpaqueMoving(bool);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDockWindow::Place"));
    candidates[2].append(QString::fromLatin1("QDockWindow::Place,QWidget*"));
    candidates[3].append(QString::fromLatin1("QDockWindow::Place,QWidget*,QString"));
    candidates[4].append(QString::fromLatin1("QDockWindow::Place,QWidget*,QString,WFlags"));
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDockWindow;
    if (sgt == QString::fromLatin1("QDockWindow::Place"))
      return new QDockWindow(static_cast<QDockWindow::Place>(args[0].variant().toUInt()));
    if (sgt == QString::fromLatin1("QDockWindow::Place,QWidget*"))
      return new QDockWindow(static_cast<QDockWindow::Place>(args[0].variant().toUInt()),
                             argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QDockWindow::Place,QWidget*,QString"))
      return new QDockWindow(static_cast<QDockWindow::Place>(args[0].variant().toUInt()),
                             argValue<QWidget *>(args[1]),
                             *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QDockWindow::Place,QWidget*,QString,WFlags"))
      return new QDockWindow(static_cast<QDockWindow::Place>(args[0].variant().toUInt()),
                             argValue<QWidget *>(args[1]),
                             *(argValue<QString *>(args[2])),
                             static_cast<WFlags>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QDockWindow(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QDockWindow(argValue<QWidget *>(args[0]),
                             *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QDockWindow(argValue<QWidget *>(args[0]),
                             *(argValue<QString *>(args[1])),
                             static_cast<WFlags>(args[2].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDockWindow::Place"));
    candidates[2].append(QString::fromLatin1("QDockWindow::Place,QWidget*"));
    candidates[3].append(QString::fromLatin1("QDockWindow::Place,QWidget*,QString"));
    candidates[4].append(QString::fromLatin1("QDockWindow::Place,QWidget*,QString,WFlags"));
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSDockWindow::setWidget(QWidget *arg0)
{
  AQ_CALL_VOID(setWidget(arg0));
}
inline QWidget *AQSDockWindow::widget() const
{
  AQ_CALL_RET(widget());
}
inline QDockArea *AQSDockWindow::area() const
{
  AQ_CALL_RET(area());
}
inline uint AQSDockWindow::place() const
{
  AQ_CALL_RET_V(place(), uint);
}
inline void AQSDockWindow::setCloseMode(int arg0)
{
  AQ_CALL_VOID(setCloseMode(arg0));
}
inline bool AQSDockWindow::isCloseEnabled() const
{
  AQ_CALL_RET_V(isCloseEnabled(), bool);
}
inline void AQSDockWindow::setResizeEnabled(bool arg0)
{
  AQ_CALL_VOID(setResizeEnabled(arg0));
}
inline void AQSDockWindow::setMovingEnabled(bool arg0)
{
  AQ_CALL_VOID(setMovingEnabled(arg0));
}
inline bool AQSDockWindow::isResizeEnabled() const
{
  AQ_CALL_RET_V(isResizeEnabled(), bool);
}
inline bool AQSDockWindow::isMovingEnabled() const
{
  AQ_CALL_RET_V(isMovingEnabled(), bool);
}
inline void AQSDockWindow::setHorizontallyStretchable(bool arg0)
{
  AQ_CALL_VOID(setHorizontallyStretchable(arg0));
}
inline void AQSDockWindow::setVerticallyStretchable(bool arg0)
{
  AQ_CALL_VOID(setVerticallyStretchable(arg0));
}
inline bool AQSDockWindow::isHorizontallyStretchable() const
{
  AQ_CALL_RET_V(isHorizontallyStretchable(), bool);
}
inline bool AQSDockWindow::isVerticallyStretchable() const
{
  AQ_CALL_RET_V(isVerticallyStretchable(), bool);
}
inline bool AQSDockWindow::isStretchable() const
{
  AQ_CALL_RET_V(isStretchable(), bool);
}
inline void AQSDockWindow::setOffset(int arg0)
{
  AQ_CALL_VOID(setOffset(arg0));
}
inline void AQSDockWindow::setFixedExtentWidth(int arg0)
{
  AQ_CALL_VOID(setFixedExtentWidth(arg0));
}
inline void AQSDockWindow::setFixedExtentHeight(int arg0)
{
  AQ_CALL_VOID(setFixedExtentHeight(arg0));
}
inline QSize AQSDockWindow::fixedExtent() const
{
  AQ_CALL_RET_V(fixedExtent(), QSize);
}
inline void AQSDockWindow::setNewLine(bool arg0)
{
  AQ_CALL_VOID(setNewLine(arg0));
}
inline bool AQSDockWindow::newLine() const
{
  AQ_CALL_RET_V(newLine(), bool);
}
inline uint AQSDockWindow::orientation() const
{
  AQ_CALL_RET_V(orientation(), uint);
}
inline QBoxLayout *AQSDockWindow::boxLayout()
{
  AQ_CALL_RET(boxLayout());
}
inline void AQSDockWindow::setOpaqueMoving(bool arg0)
{
  AQ_CALL_VOID(setOpaqueMoving(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOCKWINDOW_P_H_ */
// @AQSWIDGET@
