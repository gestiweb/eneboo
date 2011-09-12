/***************************************************************************
 AQSAction_p.h
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

#ifndef AQSACTION_P_H_
#define AQSACTION_P_H_

#include "AQSObject_p.h"
#include "AQSIconSet_p.h"
#include <qaction.h>

class AQSAction : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Action, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setIconSet(QIconSet *);
  virtual void setIconSet(AQSIconSet *);
  QIconSet *iconSet() const ;
  virtual void setText(const QString &);
  virtual void setMenuText(const QString &);
  virtual void setToolTip(const QString &);
  virtual void setStatusTip(const QString &);
  virtual void setWhatsThis(const QString &);
  virtual void setAccel(const QKeySequence &);
  virtual void setToggleAction(bool);
  bool isToggleAction() const;
  bool isOn() const;
  bool isEnabled() const;
  bool isVisible() const;
  virtual bool addTo(QWidget *);
  virtual bool removeFrom(QWidget *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[3].append(QString::fromLatin1("QString,QKeySequence,QObject*"));
    candidates[4].append(QString::fromLatin1("QString,QKeySequence,QObject*,QString"));
    candidates[4].append(QString::fromLatin1("QIconSet*,QString,QKeySequence,QObject*"));
    candidates[5].append(QString::fromLatin1("QIconSet*,QString,QKeySequence,QObject*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QObject*"))
      return new QAction(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new QAction(argValue<QObject *>(args[0]),
                         *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString,QKeySequence,QObject*"))
      return new QAction(*(argValue<QString *>(args[0])),
                         *(argValue<QKeySequence *>(args[1])),
                         argValue<QObject *>(args[2]));
    if (sgt == QString::fromLatin1("QString,QKeySequence,QObject*,QString"))
      return new QAction(*(argValue<QString *>(args[0])),
                         *(argValue<QKeySequence *>(args[1])),
                         argValue<QObject *>(args[2]),
                         *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("QIconSet*,QString,QKeySequence,QObject*"))
      return new QAction(*argValue<QIconSet *>(args[0]),
                         *(argValue<QString *>(args[1])),
                         *(argValue<QKeySequence *>(args[2])),
                         argValue<QObject *>(args[3]));
    if (sgt == QString::fromLatin1("QIconSet*,QString,QKeySequence,QObject*,QString"))
      return new QAction(*argValue<QIconSet *>(args[0]),
                         *(argValue<QString *>(args[1])),
                         *(argValue<QKeySequence *>(args[2])),
                         argValue<QObject *>(args[3]),
                         *(argValue<QString *>(args[4])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[3].append(QString::fromLatin1("QString,QKeySequence,QObject*"));
    candidates[4].append(QString::fromLatin1("QString,QKeySequence,QObject*,QString"));
    candidates[4].append(QString::fromLatin1("QIconSet*,QString,QKeySequence,QObject*"));
    candidates[5].append(QString::fromLatin1("QIconSet*,QString,QKeySequence,QObject*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSAction::setIconSet(QIconSet *arg0)
{
  AQ_CALL_VOID(setIconSet(*arg0));
}
inline void AQSAction::setIconSet(AQSIconSet *arg0)
{
  AQ_CALL_VOID(setIconSet(*arg0));
}
inline QIconSet *AQSAction::iconSet() const
{
  AQ_CALL_RET_PTR(iconSet(), QIconSet);
}
inline void AQSAction::setText(const QString &arg0)
{
  AQ_CALL_VOID(setText(arg0));
}
inline void AQSAction::setMenuText(const QString &arg0)
{
  AQ_CALL_VOID(setMenuText(arg0));
}
inline void AQSAction::setToolTip(const QString &arg0)
{
  AQ_CALL_VOID(setToolTip(arg0));
}
inline void AQSAction::setStatusTip(const QString &arg0)
{
  AQ_CALL_VOID(setStatusTip(arg0));
}
inline void AQSAction::setWhatsThis(const QString &arg0)
{
  AQ_CALL_VOID(setWhatsThis(arg0));
}
inline void AQSAction::setAccel(const QKeySequence &arg0)
{
  AQ_CALL_VOID(setAccel(arg0));
}
inline void AQSAction::setToggleAction(bool arg0)
{
  AQ_CALL_VOID(setToggleAction(arg0));
}
inline bool AQSAction::isToggleAction() const
{
  AQ_CALL_RET_V(isToggleAction(), bool);
}
inline bool AQSAction::isOn() const
{
  AQ_CALL_RET_V(isOn(), bool);
}
inline bool AQSAction::isEnabled() const
{
  AQ_CALL_RET_V(isEnabled(), bool);
}
inline bool AQSAction::isVisible() const
{
  AQ_CALL_RET_V(isVisible(), bool);
}
inline bool AQSAction::addTo(QWidget *arg0)
{
  AQ_CALL_RET_V(addTo(arg0), bool);
}
inline bool AQSAction::removeFrom(QWidget *arg0)
{
  AQ_CALL_RET_V(removeFrom(arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSACTION_P_H_ */
// @AQSOBJECT@
