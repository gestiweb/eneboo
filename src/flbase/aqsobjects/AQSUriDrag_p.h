/***************************************************************************
 AQSUriDrag_p.h
 -------------------
 begin                : 22/02/2011
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

#ifndef AQSURIDRAG_P_H_
#define AQSURIDRAG_P_H_

#include "AQSStoredDrag_p.h"

class AQSUriDrag : public AQSStoredDrag
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(UriDrag, StoredDrag);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setFileNames(const QStringList &);
  void setUnicodeUris(const QStringList &);
  QString uriToLocalFile(const char *);
  QCString localFileToUri(const QString &);
  QString uriToUnicodeUri(const char *);
  QCString unicodeUriToUri(const QString &);
  bool canDecode(const QMimeSource *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QUriDrag;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QUriDrag(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QUriDrag(argValue<QWidget *>(args[0]),
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
inline void AQSUriDrag::setFileNames(const QStringList &arg0)
{
  AQ_CALL_VOID(setFileNames(arg0));
}
inline void AQSUriDrag::setUnicodeUris(const QStringList &arg0)
{
  AQ_CALL_VOID(setUnicodeUris(arg0));
}
inline QString AQSUriDrag::uriToLocalFile(const char *arg0)
{
  AQ_CALL_RET_V(uriToLocalFile(arg0), QString);
}
inline QCString AQSUriDrag::localFileToUri(const QString &arg0)
{
  AQ_CALL_RET_V(localFileToUri(arg0), QCString);
}
inline QString AQSUriDrag::uriToUnicodeUri(const char *arg0)
{
  AQ_CALL_RET_V(uriToUnicodeUri(arg0), QString);
}
inline QCString AQSUriDrag::unicodeUriToUri(const QString &arg0)
{
  AQ_CALL_RET_V(unicodeUriToUri(arg0), QCString);
}
inline bool AQSUriDrag::canDecode(const QMimeSource *arg0)
{
  AQ_CALL_RET_V(canDecode(arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSURIDRAG_P_H_ */
// @AQSOBJECT@
