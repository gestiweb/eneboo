/***************************************************************************
 AQSPackager_p.h
 -------------------
 begin                : 14/03/2011
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

#ifndef AQSPACKAGER_P_H_
#define AQSPACKAGER_P_H_

#include "AQSVoidPtr_p.h"
#include "AQObjects.h"

class AQSPackager : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(Packager, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool pack(const QString &, bool = true, bool = false, bool = false, bool = true, bool = false);
  bool unpack(const QString &);
  QString output() const;
  QString outputPackage() const;
  void setEncodeUtf8(bool = true);
  QStringList errorMessages() const;
  QStringList logMessages() const;
  void setFilter(const QStringList &);
  QStringList filter() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQPackager;
    if (sgt == QString::fromLatin1("QString"))
      return new AQPackager(*(argValue<QString *>(args[0])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSPackager::pack(const QString &arg0,  bool arg1,  bool arg2,  bool arg3,  bool arg4,  bool arg5)
{
  AQ_CALL_RET_V(pack(arg0, arg1, arg2, arg3, arg4, arg5), bool);
}
inline bool AQSPackager::unpack(const QString &arg0)
{
  AQ_CALL_RET_V(unpack(arg0), bool);
}
inline QString AQSPackager::output() const
{
  AQ_CALL_RET_V(output(), QString);
}
inline QString AQSPackager::outputPackage() const
{
  AQ_CALL_RET_V(outputPackage(), QString);
}
inline void AQSPackager::setEncodeUtf8(bool arg0)
{
  AQ_CALL_VOID(setEncodeUtf8(arg0));
}
inline QStringList AQSPackager::errorMessages() const
{
  AQ_CALL_RET_V(errorMessages(), QStringList);
}
inline QStringList AQSPackager::logMessages() const
{
  AQ_CALL_RET_V(logMessages(), QStringList);
}
inline void AQSPackager::setFilter(const QStringList &arg0)
{
  AQ_CALL_VOID(setFilter(arg0));
}
inline QStringList AQSPackager::filter() const
{
  AQ_CALL_RET_V(filter(), QStringList);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPACKAGER_P_H_ */
// @AQOBJECT_VOID@
