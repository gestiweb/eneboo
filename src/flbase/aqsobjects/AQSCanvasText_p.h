/***************************************************************************
 AQSCanvasText_p.h
 -------------------
 begin                : 06/10/2011
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

#ifndef AQSCANVASTEXT_P_H_
#define AQSCANVASTEXT_P_H_

#include "AQSCanvasItem_p.h"

class AQSCanvasText : public AQSCanvasItem
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasText, CanvasItem);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setText(const QString &);
  void setFont(const QFont &);
  void setColor(QColor *);
  void setColor(AQSColor *);
  QString text() const;
  QFont font() const;
  QColor *color() const;
  int textFlags() const;
  void setTextFlags(int);
  virtual QRect boundingRect() const;
  virtual int rtti() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QCanvas*"));
    candidates[2].append(QString::fromLatin1("QString,QCanvas*"));
    candidates[3].append(QString::fromLatin1("QString,QFont,QCanvas*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QCanvas*"))
      return new QCanvasText(argValue<QCanvas *>(args[0]));
    if (sgt == QString::fromLatin1("QString,QCanvas*"))
      return new QCanvasText(*(argValue<QString *>(args[0])),
                             argValue<QCanvas *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QFont,QCanvas*"))
      return new QCanvasText(*(argValue<QString *>(args[0])),
                             *(argValue<QFont *>(args[1])),
                             argValue<QCanvas *>(args[2]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QCanvas*"));
    candidates[2].append(QString::fromLatin1("QString,QCanvas*"));
    candidates[3].append(QString::fromLatin1("QString,QFont,QCanvas*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSCanvasText::setText(const QString &arg0)
{
  AQ_CALL_VOID(setText(arg0));
}
inline void AQSCanvasText::setFont(const QFont &arg0)
{
  AQ_CALL_VOID(setFont(arg0));
}
inline void AQSCanvasText::setColor(QColor *arg0)
{
  AQ_CALL_VOID(setColor(*arg0));
}
inline void AQSCanvasText::setColor(AQSColor *arg0)
{
  AQ_CALL_VOID(setColor(*arg0));
}
inline QString AQSCanvasText::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
inline QFont AQSCanvasText::font() const
{
  AQ_CALL_RET_V(font(), QFont);
}
inline QColor *AQSCanvasText::color() const
{
  AQ_CALL_RET_PTR(color(), QColor);
}
inline int AQSCanvasText::textFlags() const
{
  AQ_CALL_RET_V(textFlags(), int);
}
inline void AQSCanvasText::setTextFlags(int arg0)
{
  AQ_CALL_VOID(setTextFlags(arg0));
}
inline QRect AQSCanvasText::boundingRect() const
{
  AQ_CALL_RET_V(boundingRect(), QRect);
}
inline int AQSCanvasText::rtti() const
{
  AQ_CALL_RET_V(rtti(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASTEXT_P_H_ */
// @AQSVOIDPTR@
