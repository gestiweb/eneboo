/***************************************************************************
 AQSHeader_p.h
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

#ifndef AQSHEADER_P_H_
#define AQSHEADER_P_H_

#include "AQSWidget_p.h"
#include "AQSIconSet_p.h"
#include <qheader.h>

class AQSHeader : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Header, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int addLabel(const QString &, int = -1);
  int addLabel(QIconSet *, const QString &, int = -1);
  int addLabel(AQSIconSet *, const QString &, int = -1);
  void removeLabel(int);
  void setLabel(int, const QString &, int = -1);
  void setLabel(int, QIconSet *, const QString &, int = -1);
  void setLabel(int, AQSIconSet *, const QString &, int = -1);
  QString label(int) const;
  void setClickEnabled(bool, int = -1);
  void setResizeEnabled(bool, int = -1);
  void setStretchEnabled(bool, int);
  void setStretchEnabled(bool);
  bool isClickEnabled(int = -1) const;
  bool isResizeEnabled(int = -1) const;
  bool isStretchEnabled(int) const;
  void resizeSection(int, int);
  int sectionSize(int) const;
  int sectionPos(int) const;
  int sectionAt(int) const;
  int headerWidth() const;
  QRect sectionRect(int) const;
  int mapToSection(int) const;
  int mapToIndex(int) const;
  void moveSection(int, int);
  void setSortIndicator(int, uint);
  int sortIndicatorSection() const;
  uint sortIndicatorOrder() const;
  void adjustHeaderSize();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("int,QWidget*"));
    candidates[3].append(QString::fromLatin1("int,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QHeader;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QHeader(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QHeader(argValue<QWidget *>(args[0]),
                         *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("int"))
      return new QHeader(args[0].variant().toInt());
    if (sgt == QString::fromLatin1("int,QWidget*"))
      return new QHeader(args[0].variant().toInt(),
                         argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("int,QWidget*,QString"))
      return new QHeader(args[0].variant().toInt(),
                         argValue<QWidget *>(args[1]),
                         *(argValue<QString *>(args[2])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("int,QWidget*"));
    candidates[3].append(QString::fromLatin1("int,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSHeader::addLabel(const QString &arg0,  int arg1)
{
  AQ_CALL_RET_V(addLabel(arg0, arg1), int);
}
inline int AQSHeader::addLabel(QIconSet *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_RET_V(addLabel(*arg0, arg1, arg2), int);
}
inline int AQSHeader::addLabel(AQSIconSet *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_RET_V(addLabel(*arg0, arg1, arg2), int);
}
inline void AQSHeader::removeLabel(int arg0)
{
  AQ_CALL_VOID(removeLabel(arg0));
}
inline void AQSHeader::setLabel(int arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(setLabel(arg0, arg1, arg2));
}
inline void AQSHeader::setLabel(int arg0,  QIconSet *arg1,  const QString &arg2,  int arg3)
{
  AQ_CALL_VOID(setLabel(arg0, *arg1, arg2, arg3));
}
inline void AQSHeader::setLabel(int arg0, AQSIconSet *arg1,  const QString &arg2,  int arg3)
{
  AQ_CALL_VOID(setLabel(arg0, *arg1, arg2, arg3));
}
inline QString AQSHeader::label(int arg0) const
{
  AQ_CALL_RET_V(label(arg0), QString);
}
inline void AQSHeader::setClickEnabled(bool arg0,  int arg1)
{
  AQ_CALL_VOID(setClickEnabled(arg0, arg1));
}
inline void AQSHeader::setResizeEnabled(bool arg0,  int arg1)
{
  AQ_CALL_VOID(setResizeEnabled(arg0, arg1));
}
inline void AQSHeader::setStretchEnabled(bool arg0,  int arg1)
{
  AQ_CALL_VOID(setStretchEnabled(arg0, arg1));
}
inline void AQSHeader::setStretchEnabled(bool arg0)
{
  AQ_CALL_VOID(setStretchEnabled(arg0));
}
inline bool AQSHeader::isClickEnabled(int arg0) const
{
  AQ_CALL_RET_V(isClickEnabled(arg0), bool);
}
inline bool AQSHeader::isResizeEnabled(int arg0) const
{
  AQ_CALL_RET_V(isResizeEnabled(arg0), bool);
}
inline bool AQSHeader::isStretchEnabled(int arg0) const
{
  AQ_CALL_RET_V(isStretchEnabled(arg0), bool);
}
inline void AQSHeader::resizeSection(int arg0,  int arg1)
{
  AQ_CALL_VOID(resizeSection(arg0, arg1));
}
inline int AQSHeader::sectionSize(int arg0) const
{
  AQ_CALL_RET_V(sectionSize(arg0), int);
}
inline int AQSHeader::sectionPos(int arg0) const
{
  AQ_CALL_RET_V(sectionPos(arg0), int);
}
inline int AQSHeader::sectionAt(int arg0) const
{
  AQ_CALL_RET_V(sectionAt(arg0), int);
}
inline int AQSHeader::headerWidth() const
{
  AQ_CALL_RET_V(headerWidth(), int);
}
inline QRect AQSHeader::sectionRect(int arg0) const
{
  AQ_CALL_RET_V(sectionRect(arg0), QRect);
}
inline int AQSHeader::mapToSection(int arg0) const
{
  AQ_CALL_RET_V(mapToSection(arg0), int);
}
inline int AQSHeader::mapToIndex(int arg0) const
{
  AQ_CALL_RET_V(mapToIndex(arg0), int);
}
inline void AQSHeader::moveSection(int arg0,  int arg1)
{
  AQ_CALL_VOID(moveSection(arg0, arg1));
}
inline void AQSHeader::setSortIndicator(int arg0,  uint arg1)
{
  AQ_CALL_VOID(setSortIndicator(arg0, static_cast<Qt::SortOrder>(arg1)));
}
inline int AQSHeader::sortIndicatorSection() const
{
  AQ_CALL_RET_V(sortIndicatorSection(), int);
}
inline uint AQSHeader::sortIndicatorOrder() const
{
  AQ_CALL_RET_V(sortIndicatorOrder(), uint);
}
inline void AQSHeader::adjustHeaderSize()
{
  AQ_CALL_VOID(adjustHeaderSize());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSHEADER_P_H_ */
// @AQSWIDGET@
