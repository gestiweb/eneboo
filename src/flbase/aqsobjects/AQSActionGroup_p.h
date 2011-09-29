/***************************************************************************
 AQSActionGroup_p.h
 -------------------
 begin                : 05/04/2011
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

#ifndef AQSACTIONGROUP_P_H_
#define AQSACTIONGROUP_P_H_

#include "AQSAction_p.h"
#include <qaction.h>

class AQSActionGroup : public AQSAction
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ActionGroup, Action);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setExclusive(bool);
  bool isExclusive() const;
  void add(QAction *);
  void addSeparator();
  virtual bool addTo(QWidget *);
  void setUsesDropDown(bool);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[3].append(QString::fromLatin1("QObject*,QString,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QObject*"))
      return new QActionGroup(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new QActionGroup(argValue<QObject *>(args[0]),
                              *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QObject*,QString,bool"))
      return new QActionGroup(argValue<QObject *>(args[0]),
                              *(argValue<QString *>(args[1])),
                              args[2].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[3].append(QString::fromLatin1("QObject*,QString,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSActionGroup::setExclusive(bool arg0)
{
  AQ_CALL_VOID(setExclusive(arg0));
}
inline bool AQSActionGroup::isExclusive() const
{
  AQ_CALL_RET_V(isExclusive(), bool);
}
inline void AQSActionGroup::add(QAction *arg0)
{
  AQ_CALL_VOID(add(arg0));
}
inline void AQSActionGroup::addSeparator()
{
  AQ_CALL_VOID(addSeparator());
}
inline bool AQSActionGroup::addTo(QWidget *arg0)
{
  AQ_CALL_RET_V(addTo(arg0), bool);
}
inline void AQSActionGroup::setUsesDropDown(bool arg0)
{
  AQ_CALL_VOID(setUsesDropDown(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSACTIONGROUP_P_H_ */
// @AQSOBJECT@
