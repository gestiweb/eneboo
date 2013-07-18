/***************************************************************************
 AQSTableMD_p.h
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

#ifndef AQSTABLEMD_P_H_
#define AQSTABLEMD_P_H_

#include "AQSObject_p.h"
#include "AQObjects.h"

class AQSTableMD : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(TableMD, Object);

protected:
  void specializedInternalFinish() {
    if (!wrap_ && o_ && !o_->inCache())
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString name() const;
  void setName(const QString &);
  void setAlias(const QString &);
  void setQuery(const QString &);
  QString alias() const;
  QString query() const;
  bool isQuery() const;
  void addFieldMD(FLFieldMetaData *);
  void removeFieldMD(const QString &);
  QString primaryKey(bool = false) const;
  QString fieldNameToAlias(const QString &) const;
  QString fieldAliasToName(const QString &) const;
  int fieldType(const QString &) const;
  bool fieldIsPrimaryKey(const QString &) const;
  bool fieldIsIndex(const QString &) const;
  bool fieldIsCounter(const QString &) const;
  bool fieldAllowNull(const QString &) const;
  bool fieldIsUnique(const QString &) const;
  QString fieldTableM1(const QString &) const;
  QString fieldForeignFieldM1(const QString &) const;
  FLRelationMetaData *relation(const QString &, const QString &, const QString &) const;
  int fieldLength(const QString &) const;
  int fieldPartInteger(const QString &) const;
  int fieldPartDecimal(const QString &) const;
  bool fieldCalculated(const QString &) const;
  bool fieldVisible(const QString &) const;
  FLFieldMetaData *field(const QString &) const;
  QString fieldList(bool) const;
  QString fieldsNames() const;
  QStringList fieldsNamesUnlock() const;
  bool concurWarn() const;
  void setConcurWarn(bool = true);
  bool detectLocks() const;
  void setDetectLocks(bool = true);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString,QString"))
      return new AQTableMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString,QString,QString"))
      return new AQTableMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           *(argValue<QString *>(args[2])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSTableMD::name() const
{
  AQ_CALL_RET_V(name(), QString);
}
inline void AQSTableMD::setName(const QString &arg0)
{
  AQ_CALL_VOID(setName(arg0));
}
inline void AQSTableMD::setAlias(const QString &arg0)
{
  AQ_CALL_VOID(setAlias(arg0));
}
inline void AQSTableMD::setQuery(const QString &arg0)
{
  AQ_CALL_VOID(setQuery(arg0));
}
inline QString AQSTableMD::alias() const
{
  AQ_CALL_RET_V(alias(), QString);
}
inline QString AQSTableMD::query() const
{
  AQ_CALL_RET_V(query(), QString);
}
inline bool AQSTableMD::isQuery() const
{
  AQ_CALL_RET_V(isQuery(), bool);
}
inline void AQSTableMD::addFieldMD(FLFieldMetaData *arg0)
{
  AQ_CALL_VOID(addFieldMD(arg0));
}
inline void AQSTableMD::removeFieldMD(const QString &arg0)
{
  AQ_CALL_VOID(removeFieldMD(arg0));
}
inline QString AQSTableMD::primaryKey(bool arg0) const
{
  AQ_CALL_RET_V(primaryKey(arg0), QString);
}
inline QString AQSTableMD::fieldNameToAlias(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldNameToAlias(arg0), QString);
}
inline QString AQSTableMD::fieldAliasToName(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldAliasToName(arg0), QString);
}
inline int AQSTableMD::fieldType(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldType(arg0), int);
}
inline bool AQSTableMD::fieldIsPrimaryKey(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldIsPrimaryKey(arg0), bool);
}
inline bool AQSTableMD::fieldIsIndex(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldIsIndex(arg0), bool);
}
inline bool AQSTableMD::fieldIsCounter(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldIsCounter(arg0), bool);
}
inline bool AQSTableMD::fieldAllowNull(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldAllowNull(arg0), bool);
}
inline bool AQSTableMD::fieldIsUnique(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldIsUnique(arg0), bool);
}
inline QString AQSTableMD::fieldTableM1(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldTableM1(arg0), QString);
}
inline QString AQSTableMD::fieldForeignFieldM1(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldForeignFieldM1(arg0), QString);
}
inline FLRelationMetaData *AQSTableMD::relation(const QString &arg0,  const QString &arg1,  const QString &arg2) const
{
  AQ_CALL_RET(relation(arg0, arg1, arg2));
}
inline int AQSTableMD::fieldLength(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldLength(arg0), int);
}
inline int AQSTableMD::fieldPartInteger(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldPartInteger(arg0), int);
}
inline int AQSTableMD::fieldPartDecimal(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldPartDecimal(arg0), int);
}
inline bool AQSTableMD::fieldCalculated(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldCalculated(arg0), bool);
}
inline bool AQSTableMD::fieldVisible(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldVisible(arg0), bool);
}
inline FLFieldMetaData *AQSTableMD::field(const QString &arg0) const
{
  AQ_CALL_RET(field(arg0));
}
inline QString AQSTableMD::fieldList(bool arg0) const
{
  AQ_CALL_RET_V(fieldList(arg0), QString);
}
inline QString AQSTableMD::fieldsNames() const
{
  AQ_CALL_RET_V(fieldsNames(), QString);
}
inline QStringList AQSTableMD::fieldsNamesUnlock() const
{
  AQ_CALL_RET_V(fieldsNamesUnlock(), QStringList);
}
inline bool AQSTableMD::concurWarn() const
{
  AQ_CALL_RET_V(concurWarn(), bool);
}
inline void AQSTableMD::setConcurWarn(bool arg0)
{
  AQ_CALL_VOID(setConcurWarn(arg0));
}
inline bool AQSTableMD::detectLocks() const
{
  AQ_CALL_RET_V(detectLocks(), bool);
}
inline void AQSTableMD::setDetectLocks(bool arg0)
{
  AQ_CALL_VOID(setDetectLocks(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTABLEMD_P_H_ */
// @AQOBJECT@
