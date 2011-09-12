/***************************************************************************
 AQSTab_p.h
 -------------------
 begin                : 06/02/2011
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

#ifndef AQSTAB_P_H_
#define AQSTAB_P_H_

#include "AQSIconSet_p.h"
#include <qtabbar.h>

class AQSTab : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(Tab, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setText(const QString &);
  QString text() const;
  void setIconSet(QIconSet *);
  void setIconSet(AQSIconSet *);
  QIconSet *iconSet() const;
  void setRect(const QRect &);
  QRect rect() const;
  void setEnabled(bool);
  bool isEnabled() const;
  void setIdentifier(int);
  int identifier() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QIconSet*"));
    candidates[2].append(QString::fromLatin1("QIconSet*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QTab;
    if (sgt == QString::fromLatin1("QString"))
      return new QTab(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QIconSet*"))
      return new QTab(*argValue<QIconSet *>(args[0]));
    if (sgt == QString::fromLatin1("QIconSet*,QString"))
      return new QTab(*argValue<QIconSet *>(args[0]),
                      *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QIconSet*"));
    candidates[2].append(QString::fromLatin1("QIconSet*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSTab::setText(const QString &arg0)
{
  AQ_CALL_VOID(setText(arg0));
}
inline QString AQSTab::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
inline void AQSTab::setIconSet(QIconSet *arg0)
{
  AQ_CALL_VOID(setIconSet(*arg0));
}
inline void AQSTab::setIconSet(AQSIconSet *arg0)
{
  AQ_CALL_VOID(setIconSet(*arg0));
}
inline QIconSet *AQSTab::iconSet() const
{
  AQ_CALL_RET(iconSet());
}
inline void AQSTab::setRect(const QRect &arg0)
{
  AQ_CALL_VOID(setRect(arg0));
}
inline QRect AQSTab::rect() const
{
  AQ_CALL_RET_V(rect(), QRect);
}
inline void AQSTab::setEnabled(bool arg0)
{
  AQ_CALL_VOID(setEnabled(arg0));
}
inline bool AQSTab::isEnabled() const
{
  AQ_CALL_RET_V(isEnabled(), bool);
}
inline void AQSTab::setIdentifier(int arg0)
{
  AQ_CALL_VOID(setIdentifier(arg0));
}
inline int AQSTab::identifier() const
{
  AQ_CALL_RET_V(identifier(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTAB_P_H_ */
// @AQSVOIDPTR@
