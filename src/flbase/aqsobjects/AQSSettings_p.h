/***************************************************************************
 AQSSettings_p.h
 -------------------
 begin                : 18/04/2011
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

#ifndef AQSSETTINGS_P_H_
#define AQSSETTINGS_P_H_

#include "AQSVoidPtr_p.h"
#include "AQObjects.h"

class AQSSettings : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(Settings, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QStringList readListEntry(const QString &, bool* = 0);
  QString readEntry(const QString &, const QString& = QString::null, bool* = 0);
  int readNumEntry(const QString &, int = 0, bool* = 0);
  double readDoubleEntry(const QString &, double = 0, bool* = 0);
  bool readBoolEntry(const QString &, bool = false, bool* = 0);
  bool writeEntry(const QString &, bool);
  bool writeEntry(const QString &, double);
  bool writeEntry(const QString &, int);
  bool writeEntry(const QString &, const QString &);
  bool writeEntryList(const QString &, const QStringList &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQSettings;
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QStringList AQSSettings::readListEntry(const QString &arg0,  bool *arg1)
{
  AQ_CALL_RET_V(readListEntry(arg0, arg1), QStringList);
}
inline QString AQSSettings::readEntry(const QString &arg0,  const QString &arg1,  bool *arg2)
{
  AQ_CALL_RET_V(readEntry(arg0, arg1, arg2), QString);
}
inline int AQSSettings::readNumEntry(const QString &arg0,  int arg1,  bool *arg2)
{
  AQ_CALL_RET_V(readNumEntry(arg0, arg1, arg2), int);
}
inline double AQSSettings::readDoubleEntry(const QString &arg0,  double arg1,  bool *arg2)
{
  AQ_CALL_RET_V(readDoubleEntry(arg0, arg1, arg2), double);
}
inline bool AQSSettings::readBoolEntry(const QString &arg0,  bool arg1,  bool *arg2)
{
  AQ_CALL_RET_V(readBoolEntry(arg0, arg1, arg2), bool);
}
inline bool AQSSettings::writeEntry(const QString &arg0,  bool arg1)
{
  AQ_CALL_RET_V(writeEntry(arg0, arg1), bool);
}
inline bool AQSSettings::writeEntry(const QString &arg0,  double arg1)
{
  AQ_CALL_RET_V(writeEntry(arg0, arg1), bool);
}
inline bool AQSSettings::writeEntry(const QString &arg0,  int arg1)
{
  AQ_CALL_RET_V(writeEntry(arg0, arg1), bool);
}
inline bool AQSSettings::writeEntry(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(writeEntry(arg0, arg1), bool);
}
inline bool AQSSettings::writeEntryList(const QString &arg0,  const QStringList &arg1)
{
  AQ_CALL_RET_V(writeEntryList(arg0, arg1), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSETTINGS_P_H_ */
// @AQOBJECT_VOID@
