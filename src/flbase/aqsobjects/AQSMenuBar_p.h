/***************************************************************************
 AQSMenuBar_p.h
 -------------------
 begin                : 30/01/2011
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

#ifndef AQSMENUBAR_P_H_
#define AQSMENUBAR_P_H_

#include "AQSFrame_p.h"
#include "AQSPixmap_p.h"
#include <qmenubar.h>

class AQSMenuBar : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(MenuBar, Frame);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int heightForWidth(int) const;
  int insertItem(const QString &, int = -1, int = -1);
  int insertItem(const QString &, QPopupMenu *, int = -1, int = -1);
  int insertItem(QPixmap *, int = -1, int = -1);
  int insertItem(AQSPixmap *, int = -1, int = -1);
  int insertItem(QPixmap *, QPopupMenu *, int = -1, int = -1);
  int insertItem(AQSPixmap *, QPopupMenu *, int = -1, int = -1);
  int insertSeparator(int = -1);
  void removeItem(int);
  void removeItemAt(int);
  void clear();
  uint count() const;
  QKeySequence accel(int) const;
  void setAccel(const QKeySequence &, int);
  QString text(int) const;
  QPixmap *pixmap(int) const;
  void setWhatsThis(int, const QString &);
  QString whatsThis(int) const;
  void changeItem(int, const QString &);
  void changeItem(int, QPixmap *);
  void changeItem(int, AQSPixmap *);
  bool isItemEnabled(int) const;
  void setItemEnabled(int, bool);
  bool isItemChecked(int) const;
  void setItemChecked(int, bool);
  bool isItemVisible(int) const;
  void setItemVisible(int, bool);
  bool setItemParameter(int, int);
  int itemParameter(int) const;
  void activateItemAt(int);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QMenuBar;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QMenuBar(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QMenuBar(argValue<QWidget *>(args[0]),
                          *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSMenuBar::heightForWidth(int arg0) const
{
  AQ_CALL_RET_V(heightForWidth(arg0), int);
}
inline int AQSMenuBar::insertItem(const QString &arg0,  int arg1,  int arg2)
{
  AQ_CALL_RET_V(insertItem(arg0, arg1, arg2), int);
}
inline int AQSMenuBar::insertItem(const QString &arg0,  QPopupMenu *arg1,  int arg2,  int arg3)
{
  AQ_CALL_RET_V(insertItem(arg0, arg1, arg2, arg3), int);
}
inline int AQSMenuBar::insertItem(QPixmap *arg0,  int arg1,  int arg2)
{
  AQ_CALL_RET_V(insertItem(*arg0, arg1, arg2), int);
}
inline int AQSMenuBar::insertItem(AQSPixmap *arg0,  int arg1,  int arg2)
{
  AQ_CALL_RET_V(insertItem(*arg0, arg1, arg2), int);
}
inline int AQSMenuBar::insertItem(QPixmap *arg0,  QPopupMenu *arg1,  int arg2,  int arg3)
{
  AQ_CALL_RET_V(insertItem(*arg0, arg1, arg2, arg3), int);
}
inline int AQSMenuBar::insertItem(AQSPixmap *arg0,  QPopupMenu *arg1,  int arg2,  int arg3)
{
  AQ_CALL_RET_V(insertItem(*arg0, arg1, arg2, arg3), int);
}
inline int AQSMenuBar::insertSeparator(int arg0)
{
  AQ_CALL_RET_V(insertSeparator(arg0), int);
}
inline void AQSMenuBar::removeItem(int arg0)
{
  AQ_CALL_VOID(removeItem(arg0));
}
inline void AQSMenuBar::removeItemAt(int arg0)
{
  AQ_CALL_VOID(removeItemAt(arg0));
}
inline void AQSMenuBar::clear()
{
  AQ_CALL_VOID(clear());
}
inline uint AQSMenuBar::count() const
{
  AQ_CALL_RET_V(count(), uint);
}
inline QKeySequence AQSMenuBar::accel(int arg0) const
{
  AQ_CALL_RET_V(accel(arg0), QKeySequence);
}
inline void AQSMenuBar::setAccel(const QKeySequence &arg0,  int arg1)
{
  AQ_CALL_VOID(setAccel(arg0, arg1));
}
inline QString AQSMenuBar::text(int arg0) const
{
  AQ_CALL_RET_V(text(arg0), QString);
}
inline QPixmap *AQSMenuBar::pixmap(int arg0) const
{
  AQ_CALL_RET(pixmap(arg0));
}
inline void AQSMenuBar::setWhatsThis(int arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setWhatsThis(arg0, arg1));
}
inline QString AQSMenuBar::whatsThis(int arg0) const
{
  AQ_CALL_RET_V(whatsThis(arg0), QString);
}
inline void AQSMenuBar::changeItem(int arg0,  const QString &arg1)
{
  AQ_CALL_VOID(changeItem(arg0, arg1));
}
inline void AQSMenuBar::changeItem(int arg0,  QPixmap *arg1)
{
  AQ_CALL_VOID(changeItem(arg0, *arg1));
}
inline void AQSMenuBar::changeItem(int arg0, AQSPixmap *arg1)
{
  AQ_CALL_VOID(changeItem(arg0, *arg1));
}
inline bool AQSMenuBar::isItemEnabled(int arg0) const
{
  AQ_CALL_RET_V(isItemEnabled(arg0), bool);
}
inline void AQSMenuBar::setItemEnabled(int arg0,  bool arg1)
{
  AQ_CALL_VOID(setItemEnabled(arg0, arg1));
}
inline bool AQSMenuBar::isItemChecked(int arg0) const
{
  AQ_CALL_RET_V(isItemChecked(arg0), bool);
}
inline void AQSMenuBar::setItemChecked(int arg0,  bool arg1)
{
  AQ_CALL_VOID(setItemChecked(arg0, arg1));
}
inline bool AQSMenuBar::isItemVisible(int arg0) const
{
  AQ_CALL_RET_V(isItemVisible(arg0), bool);
}
inline void AQSMenuBar::setItemVisible(int arg0,  bool arg1)
{
  AQ_CALL_VOID(setItemVisible(arg0, arg1));
}
inline bool AQSMenuBar::setItemParameter(int arg0,  int arg1)
{
  AQ_CALL_RET_V(setItemParameter(arg0, arg1), bool);
}
inline int AQSMenuBar::itemParameter(int arg0) const
{
  AQ_CALL_RET_V(itemParameter(arg0), int);
}
inline void AQSMenuBar::activateItemAt(int arg0)
{
  AQ_CALL_VOID(activateItemAt(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSMENUBAR_P_H_ */
// @AQSWIDGET@
