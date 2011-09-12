/***************************************************************************
 AQSFieldMD_p.h
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

#ifndef AQSFIELDMD_P_H_
#define AQSFIELDMD_P_H_

#include "AQSVoidPtr_p.h"
#include "AQObjects.h"

class AQSFieldMD : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(FieldMD, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString name() const;
  void setName(const QString &);
  QString alias() const;
  bool allowNull() const;
  bool isPrimaryKey() const;
  bool isCompoundKey() const;
  int type() const;
  int length() const;
  bool calculated() const;
  void setCalculated(bool);
  bool editable() const;
  void setEditable(bool);
  bool visible() const;
  bool visibleGrid() const;
  bool generated() const;
  void setVisible(bool);
  void setVisibleGrid(bool);
  int partInteger() const;
  int partDecimal() const;
  bool isCounter() const;
  bool isIndex() const;
  bool isUnique() const;
  void addRelationMD(FLRelationMetaData *);
  FLRelationMetaData *relationM1() const;
  void setAssociatedField(FLFieldMetaData *, const QString &);
  void setAssociatedField(const QString &, const QString &);
  FLFieldMetaData *associatedField() const;
  QString associatedFieldFilterTo() const;
  QString associatedFieldName() const;
  QVariant defaultValue();
  bool outTransaction();
  QString regExpValidator();
  QStringList optionsList();
  void setOptionsList(const QString &);
  bool isCheck() const;
  bool hasOptionsList() const;
  void setMetadata(FLTableMetaData *);
  FLTableMetaData *metadata() const;
  uint flDecodeType(int);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[5].append(QString::fromLatin1("QString,QString,bool,bool,int"));
    candidates[6].append(QString::fromLatin1("QString,QString,bool,bool,int,int"));
    candidates[7].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool"));
    candidates[8].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool"));
    candidates[9].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool"));
    candidates[10].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int"));
    candidates[11].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int"));
    candidates[12].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool"));
    candidates[13].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool"));
    candidates[14].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool"));
    candidates[15].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant"));
    candidates[16].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool"));
    candidates[17].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString"));
    candidates[18].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString,bool"));
    candidates[19].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString,bool,bool"));
    candidates[20].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString,bool,bool,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt(),
                           args[11].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt(),
                           args[11].variant().toBool(),
                           args[12].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt(),
                           args[11].variant().toBool(),
                           args[12].variant().toBool(),
                           args[13].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt(),
                           args[11].variant().toBool(),
                           args[12].variant().toBool(),
                           args[13].variant().toBool(),
                           *(argValue<QVariant *>(args[14])));
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt(),
                           args[11].variant().toBool(),
                           args[12].variant().toBool(),
                           args[13].variant().toBool(),
                           *(argValue<QVariant *>(args[14])),
                           args[15].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt(),
                           args[11].variant().toBool(),
                           args[12].variant().toBool(),
                           args[13].variant().toBool(),
                           *(argValue<QVariant *>(args[14])),
                           args[15].variant().toBool(),
                           *(argValue<QString *>(args[16])));
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt(),
                           args[11].variant().toBool(),
                           args[12].variant().toBool(),
                           args[13].variant().toBool(),
                           *(argValue<QVariant *>(args[14])),
                           args[15].variant().toBool(),
                           *(argValue<QString *>(args[16])),
                           args[17].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString,bool,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt(),
                           args[11].variant().toBool(),
                           args[12].variant().toBool(),
                           args[13].variant().toBool(),
                           *(argValue<QVariant *>(args[14])),
                           args[15].variant().toBool(),
                           *(argValue<QString *>(args[16])),
                           args[17].variant().toBool(),
                           args[18].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString,bool,bool,bool"))
      return new AQFieldMD(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toBool(),
                           args[3].variant().toBool(),
                           args[4].variant().toInt(),
                           args[5].variant().toInt(),
                           args[6].variant().toBool(),
                           args[7].variant().toBool(),
                           args[8].variant().toBool(),
                           args[9].variant().toInt(),
                           args[10].variant().toInt(),
                           args[11].variant().toBool(),
                           args[12].variant().toBool(),
                           args[13].variant().toBool(),
                           *(argValue<QVariant *>(args[14])),
                           args[15].variant().toBool(),
                           *(argValue<QString *>(args[16])),
                           args[17].variant().toBool(),
                           args[18].variant().toBool(),
                           args[19].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[5].append(QString::fromLatin1("QString,QString,bool,bool,int"));
    candidates[6].append(QString::fromLatin1("QString,QString,bool,bool,int,int"));
    candidates[7].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool"));
    candidates[8].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool"));
    candidates[9].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool"));
    candidates[10].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int"));
    candidates[11].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int"));
    candidates[12].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool"));
    candidates[13].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool"));
    candidates[14].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool"));
    candidates[15].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant"));
    candidates[16].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool"));
    candidates[17].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString"));
    candidates[18].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString,bool"));
    candidates[19].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString,bool,bool"));
    candidates[20].append(QString::fromLatin1("QString,QString,bool,bool,int,int,bool,bool,bool,int,int,bool,bool,bool,QVariant,bool,QString,bool,bool,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSFieldMD::name() const
{
  AQ_CALL_RET_V(name(), QString);
}
inline void AQSFieldMD::setName(const QString &arg0)
{
  AQ_CALL_VOID(setName(arg0));
}
inline QString AQSFieldMD::alias() const
{
  AQ_CALL_RET_V(alias(), QString);
}
inline bool AQSFieldMD::allowNull() const
{
  AQ_CALL_RET_V(allowNull(), bool);
}
inline bool AQSFieldMD::isPrimaryKey() const
{
  AQ_CALL_RET_V(isPrimaryKey(), bool);
}
inline bool AQSFieldMD::isCompoundKey() const
{
  AQ_CALL_RET_V(isCompoundKey(), bool);
}
inline int AQSFieldMD::type() const
{
  AQ_CALL_RET_V(type(), int);
}
inline int AQSFieldMD::length() const
{
  AQ_CALL_RET_V(length(), int);
}
inline bool AQSFieldMD::calculated() const
{
  AQ_CALL_RET_V(calculated(), bool);
}
inline void AQSFieldMD::setCalculated(bool arg0)
{
  AQ_CALL_VOID(setCalculated(arg0));
}
inline bool AQSFieldMD::editable() const
{
  AQ_CALL_RET_V(editable(), bool);
}
inline void AQSFieldMD::setEditable(bool arg0)
{
  AQ_CALL_VOID(setEditable(arg0));
}
inline bool AQSFieldMD::visible() const
{
  AQ_CALL_RET_V(visible(), bool);
}
inline bool AQSFieldMD::visibleGrid() const
{
  AQ_CALL_RET_V(visibleGrid(), bool);
}
inline bool AQSFieldMD::generated() const
{
  AQ_CALL_RET_V(generated(), bool);
}
inline void AQSFieldMD::setVisible(bool arg0)
{
  AQ_CALL_VOID(setVisible(arg0));
}
inline void AQSFieldMD::setVisibleGrid(bool arg0)
{
  AQ_CALL_VOID(setVisibleGrid(arg0));
}
inline int AQSFieldMD::partInteger() const
{
  AQ_CALL_RET_V(partInteger(), int);
}
inline int AQSFieldMD::partDecimal() const
{
  AQ_CALL_RET_V(partDecimal(), int);
}
inline bool AQSFieldMD::isCounter() const
{
  AQ_CALL_RET_V(isCounter(), bool);
}
inline bool AQSFieldMD::isIndex() const
{
  AQ_CALL_RET_V(isIndex(), bool);
}
inline bool AQSFieldMD::isUnique() const
{
  AQ_CALL_RET_V(isUnique(), bool);
}
inline void AQSFieldMD::addRelationMD(FLRelationMetaData *arg0)
{
  AQ_CALL_VOID(addRelationMD(arg0));
}
inline FLRelationMetaData *AQSFieldMD::relationM1() const
{
  AQ_CALL_RET(relationM1());
}
inline void AQSFieldMD::setAssociatedField(FLFieldMetaData *arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setAssociatedField(arg0, arg1));
}
inline void AQSFieldMD::setAssociatedField(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setAssociatedField(arg0, arg1));
}
inline FLFieldMetaData *AQSFieldMD::associatedField() const
{
  AQ_CALL_RET(associatedField());
}
inline QString AQSFieldMD::associatedFieldFilterTo() const
{
  AQ_CALL_RET_V(associatedFieldFilterTo(), QString);
}
inline QString AQSFieldMD::associatedFieldName() const
{
  AQ_CALL_RET_V(associatedFieldName(), QString);
}
inline QVariant AQSFieldMD::defaultValue()
{
  AQ_CALL_RET_V(defaultValue(), QVariant);
}
inline bool AQSFieldMD::outTransaction()
{
  AQ_CALL_RET_V(outTransaction(), bool);
}
inline QString AQSFieldMD::regExpValidator()
{
  AQ_CALL_RET_V(regExpValidator(), QString);
}
inline QStringList AQSFieldMD::optionsList()
{
  AQ_CALL_RET_V(optionsList(), QStringList);
}
inline void AQSFieldMD::setOptionsList(const QString &arg0)
{
  AQ_CALL_VOID(setOptionsList(arg0));
}
inline bool AQSFieldMD::isCheck() const
{
  AQ_CALL_RET_V(isCheck(), bool);
}
inline bool AQSFieldMD::hasOptionsList() const
{
  AQ_CALL_RET_V(hasOptionsList(), bool);
}
inline void AQSFieldMD::setMetadata(FLTableMetaData *arg0)
{
  AQ_CALL_VOID(setMetadata(arg0));
}
inline FLTableMetaData *AQSFieldMD::metadata() const
{
  AQ_CALL_RET(metadata());
}
inline uint AQSFieldMD::flDecodeType(int arg0)
{
  AQ_CALL_RET_V(flDecodeType(arg0), uint);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFIELDMD_P_H_ */
// @AQOBJECT_VOID@
