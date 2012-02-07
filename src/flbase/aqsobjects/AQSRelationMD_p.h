/***************************************************************************
 AQSRelationMD_p.h
 -------------------
 begin                : 29/03/2011
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

#ifndef AQSRELATIONMD_P_H_
#define AQSRELATIONMD_P_H_

#include "AQSVoidPtr_p.h"
#include "AQObjects.h"

class AQSRelationMD : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(RelationMD, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setField(const QString &);
  QString field() const;
  QString foreignTable() const;
  QString foreignField() const;
  int cardinality() const;
  bool deleteCascade() const;
  bool updateCascade() const;
  bool checkIn();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[4].append(QString::fromLatin1("QString,QString,int,bool"));
    candidates[5].append(QString::fromLatin1("QString,QString,int,bool,bool"));
    candidates[6].append(QString::fromLatin1("QString,QString,int,bool,bool,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString,QString,int"))
      return new AQRelationMD(*(argValue<QString *>(args[0])),
                              *(argValue<QString *>(args[1])),
                              args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QString,QString,int,bool"))
      return new AQRelationMD(*(argValue<QString *>(args[0])),
                              *(argValue<QString *>(args[1])),
                              args[2].variant().toInt(),
                              args[3].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,int,bool,bool"))
      return new AQRelationMD(*(argValue<QString *>(args[0])),
                              *(argValue<QString *>(args[1])),
                              args[2].variant().toInt(),
                              args[3].variant().toBool(),
                              args[4].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,int,bool,bool,bool"))
      return new AQRelationMD(*(argValue<QString *>(args[0])),
                              *(argValue<QString *>(args[1])),
                              args[2].variant().toInt(),
                              args[3].variant().toBool(),
                              args[4].variant().toBool(),
                              args[5].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[4].append(QString::fromLatin1("QString,QString,int,bool"));
    candidates[5].append(QString::fromLatin1("QString,QString,int,bool,bool"));
    candidates[6].append(QString::fromLatin1("QString,QString,int,bool,bool,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSRelationMD::setField(const QString &arg0)
{
  AQ_CALL_VOID(setField(arg0));
}
inline QString AQSRelationMD::field() const
{
  AQ_CALL_RET_V(field(), QString);
}
inline QString AQSRelationMD::foreignTable() const
{
  AQ_CALL_RET_V(foreignTable(), QString);
}
inline QString AQSRelationMD::foreignField() const
{
  AQ_CALL_RET_V(foreignField(), QString);
}
inline int AQSRelationMD::cardinality() const
{
  AQ_CALL_RET_V(cardinality(), int);
}
inline bool AQSRelationMD::deleteCascade() const
{
  AQ_CALL_RET_V(deleteCascade(), bool);
}
inline bool AQSRelationMD::updateCascade() const
{
  AQ_CALL_RET_V(updateCascade(), bool);
}
inline bool AQSRelationMD::checkIn()
{
  AQ_CALL_RET_V(checkIn(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSRELATIONMD_P_H_ */
// @AQOBJECT_VOID@
