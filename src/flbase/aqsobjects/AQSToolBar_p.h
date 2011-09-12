/***************************************************************************
 AQSToolBar_p.h
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

#ifndef AQSTOOLBAR_P_H_
#define AQSTOOLBAR_P_H_

#include "AQSDockWindow_p.h"
#include <qtoolbar.h>

class AQSToolBar : public AQSDockWindow
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ToolBar, DockWindow);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void addSeparator();
  QMainWindow *mainWindow() const;
  virtual void setStretchableWidget(QWidget *);
  virtual void setLabel(const QString &);
  virtual void clear();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QString,QMainWindow*,QWidget*"));
    candidates[4].append(QString::fromLatin1("QString,QMainWindow*,QWidget*,bool"));
    candidates[5].append(QString::fromLatin1("QString,QMainWindow*,QWidget*,bool,QString"));
    candidates[6].append(QString::fromLatin1("QString,QMainWindow*,QWidget*,bool,QString,WFlags"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QMainWindow*"));
    candidates[2].append(QString::fromLatin1("QMainWindow*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString,QMainWindow*,QWidget*"))
      return new QToolBar(*(argValue<QString *>(args[0])),
                          argValue<QMainWindow *>(args[1]),
                          argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("QString,QMainWindow*,QWidget*,bool"))
      return new QToolBar(*(argValue<QString *>(args[0])),
                          argValue<QMainWindow *>(args[1]),
                          argValue<QWidget *>(args[2]),
                          args[3].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QMainWindow*,QWidget*,bool,QString"))
      return new QToolBar(*(argValue<QString *>(args[0])),
                          argValue<QMainWindow *>(args[1]),
                          argValue<QWidget *>(args[2]),
                          args[3].variant().toBool(),
                          *(argValue<QString *>(args[4])));
    if (sgt == QString::fromLatin1("QString,QMainWindow*,QWidget*,bool,QString,WFlags"))
      return new QToolBar(*(argValue<QString *>(args[0])),
                          argValue<QMainWindow *>(args[1]),
                          argValue<QWidget *>(args[2]),
                          args[3].variant().toBool(),
                          *(argValue<QString *>(args[4])),
                          static_cast<WFlags>(args[5].variant().toUInt()));
    if (sgt.isEmpty())
      return new QToolBar;
    if (sgt == QString::fromLatin1("QMainWindow*"))
      return new QToolBar(argValue<QMainWindow *>(args[0]));
    if (sgt == QString::fromLatin1("QMainWindow*,QString"))
      return new QToolBar(argValue<QMainWindow *>(args[0]),
                          *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QString,QMainWindow*,QWidget*"));
    candidates[4].append(QString::fromLatin1("QString,QMainWindow*,QWidget*,bool"));
    candidates[5].append(QString::fromLatin1("QString,QMainWindow*,QWidget*,bool,QString"));
    candidates[6].append(QString::fromLatin1("QString,QMainWindow*,QWidget*,bool,QString,WFlags"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QMainWindow*"));
    candidates[2].append(QString::fromLatin1("QMainWindow*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSToolBar::addSeparator()
{
  AQ_CALL_VOID(addSeparator());
}
inline QMainWindow *AQSToolBar::mainWindow() const
{
  AQ_CALL_RET(mainWindow());
}
inline void AQSToolBar::setStretchableWidget(QWidget *arg0)
{
  AQ_CALL_VOID(setStretchableWidget(arg0));
}
inline void AQSToolBar::setLabel(const QString &arg0)
{
  AQ_CALL_VOID(setLabel(arg0));
}
inline void AQSToolBar::clear()
{
  AQ_CALL_VOID(clear());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTOOLBAR_P_H_ */
// @AQSWIDGET@
